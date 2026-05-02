# stm32-sdk

Bare-metal C++17 экосистема для STM32 (CMSIS only).

## Использование

```bash
pip install ./tools/stmtool
stmtool project create my-project --chip STM32F407VG
cd my-project
stmtool build --native
stmtool flash
```

## Правила

- Только CMSIS, HAL/LL запрещены
- Прямой доступ к регистрам: `GPIOD->MODER`, `RCC->AHB1ENR`
- C++20 (gnu++20), C11 (gnu11)
- cxx.cpp компилируется с gnu++11 (совместимость с newlib)
- Conventional Commits для всех коммитов
- Битовые операции с регистрами через `driver::reg::set/clear/modify` (не прямые `|=`, `&= ~`)
- Не использовать сырые указатели на MMIO-периферию -- использовать ссылки (`GPIO_TypeDef&`)
- Allman brace style, пробелы внутри скобок: `f( a, b )`, однострочный if всегда в `{}`

## Структура

- `sdk/core/` -- ARM CMSIS core, Cortex-M runtime, newlib, linker scripts
- `sdk/hal/stm32f4/` -- STM32F4 device headers, vector tables, memory layout
- `sdk/cmake/` -- stm32_sdk.cmake, stm32_toolchain.cmake, families/
- `sdk/rtos/` -- FreeRTOS integration, FreeRTOSConfig.h, RAII wrappers (rtos.hpp)
- `sdk/drivers/` -- driver interfaces (.cppm modules) + stm32f4 implementations (.hpp)
- `sdk/sensors/` -- sensor interfaces (.cppm) + implementations (mpu6050)
- `templates/` -- project templates (bare-metal/blink, freertos/blink, freertos/mpu6050-uart)
- `tools/stmtool/` -- Python CLI tool (Typer + Rich)
- `docker/` -- Dockerfiles for build environment

## Выбор чипа

`-DSTM32_CHIP=<name>` где name = STM32F4xxYZ (Y=package, Z=flash size letter).
Поддерживаются: F401, F405, F407, F411, F412, F429, F439, F446.
Chip database: `sdk/cmake/families/stm32f4.cmake`.

## Добавление нового семейства

1. Создать `sdk/cmake/families/stm32XX.cmake` с функцией `stm32XX_get_chip_info`
2. Добавить CMSIS device headers в `sdk/hal/stm32XX/include/cmsis/`
3. Создать vector tables в `sdk/hal/stm32XX/src/cmsis/`
4. Создать `sdk/hal/stm32XX/ldscripts/mem.ld.in`
5. При необходимости добавить вариант sections.ld в `sdk/core/ldscripts/`

## Добавление нового чипа F4

1. Добавить CMSIS device header в `sdk/hal/stm32f4/include/cmsis/`
2. Создать vector table в `sdk/hal/stm32f4/src/cmsis/vectors_stm32fXXXxx.c`
3. Добавить запись в `sdk/cmake/families/stm32f4.cmake`
4. Убедиться, что `stm32f4xx.h` содержит `#include` для нового header
