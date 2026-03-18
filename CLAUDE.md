# stm32f4_template

Bare-metal C++17 шаблон для STM32F4 (CMSIS only).

## Сборка

```bash
cmake -B build -DSTM32_CHIP=STM32F407VG
cmake --build build
cmake --build build --target flash
```

## Правила

- Только CMSIS, HAL/LL запрещены
- Прямой доступ к регистрам: `GPIOD->MODER`, `RCC->AHB1ENR`
- C++17 (gnu++17), C11 (gnu11)
- cxx.cpp компилируется с gnu++11 (совместимость с newlib)

## Структура

- `src/` -- пользовательский код
- `system/include/cmsis/` -- CMSIS device headers для STM32F4
- `system/src/cmsis/` -- system init + vector tables (по одному на семейство)
- `system/src/cortexm/` -- exception handlers, hardware init
- `system/src/newlib/` -- startup, syscalls, sbrk
- `ldscripts/` -- mem.ld.in (шаблон), sections.ld, libs.ld
- `cmake/` -- toolchain + chip database

## Выбор чипа

`-DSTM32_CHIP=<name>` где name = STM32F4xxYZ (Y=package, Z=flash size letter).
Поддерживаются: F401, F405, F407, F411, F412, F429, F439, F446.
Chip database: `cmake/stm32f4_chips.cmake`.

## Добавление нового чипа

1. Добавить CMSIS device header в `system/include/cmsis/`
2. Создать vector table в `system/src/cmsis/vectors_stm32fXXXxx.c`
3. Добавить запись в `cmake/stm32f4_chips.cmake`
4. Убедиться, что `stm32f4xx.h` содержит `#include` для нового header
