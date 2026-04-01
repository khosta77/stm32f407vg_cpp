# stm32-sdk

Bare-metal C++17 экосистема для STM32 (CMSIS only).

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
- Conventional Commits для всех коммитов

## Структура

- `src/` -- пользовательский код (пример blink)
- `sdk/core/include/cmsis/` -- ARM CMSIS core headers (core_cm*.h)
- `sdk/core/include/{cortexm,diag,arm}/` -- generic Cortex-M headers
- `sdk/core/src/{cortexm,diag,newlib}/` -- generic Cortex-M sources
- `sdk/core/ldscripts/` -- sections.ld, libs.ld (generic)
- `sdk/hal/stm32f4/include/cmsis/` -- STM32F4 device headers
- `sdk/hal/stm32f4/src/cmsis/` -- system init + vector tables
- `sdk/hal/stm32f4/ldscripts/` -- mem.ld.in (F4 memory layout)
- `sdk/cmake/` -- stm32_sdk.cmake (entry), stm32_toolchain.cmake, families/
- `templates/` -- project templates (blink, etc.)
- `tools/stmtool/` -- Python CLI tool
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
