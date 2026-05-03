# stm32-sdk

Bare-metal C++20 экосистема для STM32 (CMSIS only).

## Использование

```bash
pip install ./tools/stmtool
stmtool project create my-project --chip STM32F407VG
cd my-project
stmtool build --native
stmtool flash
```

## Правила кода

- Только CMSIS, HAL/LL запрещены
- C++20 (gnu++20), C11 (gnu11), cxx.cpp с gnu++11
- Google Code Style (clang-format)
- Conventional Commits на английском
- Без сырых указателей на MMIO -- ссылки (`GPIO_TypeDef&`)
- Config structs без значений по умолчанию -- все задается явно
- Глобальные объекты периферии (не static local, не указатели)

## C++20 модули

Архитектура: interface modules (.cppm) + implementation modules (.cppm).

### Паттерн модуля

```cpp
module;
#include <cstdint>              // STL -- ТОЛЬКО через #include в global module fragment
#include "cmsis/stm32f4xx.h"    // CMSIS -- тоже #include (макросы не экспортируются)
#ifdef STM32_USE_FREERTOS
#include "FreeRTOS.h"
#endif
export module driver.stm32f4.i2c;

import driver.types;             // наши модули -- через import
import driver.i2c;
import driver.reg;
```

### Почему #include, а не import

- `import <cstdint>;` (header units) -- GCC 15 arm-none-eabi не поддерживает
- `import std;` -- C++23, не поддерживается
- CMake не поддерживает header units
- #include в global module fragment -- стандартный C++20 паттерн

### TU-local ограничения для шаблонов

CMSIS `__STATIC_INLINE` функции (NVIC_SetPriority, __DSB) -- `static inline` = TU-local.
Шаблоны (Uart<>) не могут их вызывать напрямую. Решение: wrappers с module linkage:

```cpp
namespace driver::stm32f4::detail {
void dsb() { __DSB(); }
void nvicEnableIRQ(IRQn_Type irq) { NVIC_EnableIRQ(irq); }
}
```

Не-шаблонные классы (GpioPin, I2c, Spi) могут вызывать CMSIS функции напрямую.

### Нельзя смешивать #include <span> и import

GCC 15 дает redefinition errors если main.cpp делает `#include <span>` и `import` модуля с `<span>` в global fragment. Решение: не включать STL-заголовки в main.cpp, использовать brace-init `{ptr, len}` для span.

## Структура

- `sdk/core/` -- ARM CMSIS core, Cortex-M runtime, newlib, linker scripts
- `sdk/hal/stm32f4/` -- STM32F4 device headers, vector tables, memory layout
- `sdk/cmake/` -- stm32_sdk.cmake, stm32_toolchain.cmake, families/
- `sdk/rtos/` -- FreeRTOS (heap_4, 16KB), RAII wrappers (rtos.hpp)
- `sdk/drivers/include/driver/` -- модули:
  - `types.cppm`, `reg.cppm`, `circular_buffer.cppm` -- утилиты
  - `interface/i_*.cppm` -- интерфейсы (IGpioPin, IUart, II2c, ISpi, IFlash)
  - `stm32f4/*.cppm` -- реализации (GpioPin, Uart<>, I2c, Spi, InternalFlash, clock)
- `sdk/sensors/` -- sensor interfaces + implementations (MPU6050)
- `templates/` -- project templates (bare-metal/blink, freertos/blink, freertos/mpu6050-uart)
- `tools/stmtool/` -- Python CLI tool (Typer + Rich)

## Паттерны драйверов

### Clock enable

- GpioPin: включает тактирование GPIO порта в конструкторе (if/else на адрес)
- I2C/UART: тактирование через `__initialize_hardware()` override в main.cpp (template constraint)

### Config

Все Config без дефолтов. Пользователь обязан задать все поля:

```cpp
I2c::Config{ .clockSpeed = 400000, .fastMode = true }
Uart<>::Config{ .baudrate = 115200, .dataBits = 8, .stopBits = 1, .parity = Parity::None }
```

### GpioConfig -- consteval валидация

```cpp
GpioConfig{ 12, PinMode::Output, PullMode::None, OutputSpeed::Low, OutputType::PushPull }
```

consteval конструктор -- ошибка компиляции если PinMode::None или pin > 15.

### Uart -- template buffer sizes

```cpp
Uart<512, 256> uart{...};  // RxBufSize=512, TxBufSize=256
```

static_assert: power of 2, minimum 16.

## FreeRTOS паттерны

### Глобальные объекты

```cpp
extern "C" void __initialize_hardware() {
    SystemCoreClockUpdate();
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN | RCC_APB1ENR_USART2EN;
    __DSB();
}

namespace {
GpioPin g_led{*GPIOD, {12, PinMode::Output, ...}};
I2c g_i2c1{*I2C1, {...}};
Uart<> g_uart2{*USART2, USART2_IRQn, {...}};
}
```

`__initialize_hardware()` -- weak из SDK, override в main.cpp. Вызывается до конструкторов.

### Объекты после startScheduler

CachedSensor и rtos::Task -- `static` (MSP reused для ISR после scheduler start).

### ISR handler

```cpp
extern "C" void USART2_IRQHandler() { g_uart2.irqHandler(); }
```

Прямая ссылка на глобальный объект (не указатель). Null-check семафоров в ISR защищает от spurious interrupt.

## CMake

- `-nostartfiles` обязателен (без него newlib crt0 переопределяет наш _start)
- `nano.specs` -- без поддержки %f (нужен `-u _printf_float` или integer formatting)
- stm32_drivers: OBJECT library, не линкует stm32_hal/stm32_core (INTERFACE sources duplication)
- Include dirs от CMSIS добавляются напрямую в stm32_drivers

## Выбор чипа

`-DSTM32_CHIP=<name>` где name = STM32F4xxYZ (Y=package, Z=flash size letter).
Поддерживаются: F401, F405, F407, F411, F412, F429, F439, F446.
Chip database: `sdk/cmake/families/stm32f4.cmake`.

## Добавление нового семейства

1. Создать `sdk/cmake/families/stm32XX.cmake` с функцией `stm32XX_get_chip_info`
2. Добавить CMSIS device headers в `sdk/hal/stm32XX/include/cmsis/`
3. Создать vector tables в `sdk/hal/stm32XX/src/cmsis/`
4. Создать `sdk/hal/stm32XX/ldscripts/mem.ld.in`
