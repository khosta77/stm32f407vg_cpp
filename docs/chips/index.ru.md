# Поддерживаемые чипы

SDK на данный момент целится в семейство STM32F4. Выбор чипа происходит на
этапе configure через `-DSTM32_CHIP=<name>` (автоматически устанавливается
`stmtool` из `stmproject.toml`).

| Семейство | RAM | CCM | Flash | Пример |
|-----------|-----|-----|-------|--------|
| STM32F401 | 64-96К | — | 128-512К | STM32F401CC, STM32F401RE |
| STM32F405 | 128К | 64К | 512-1024К | STM32F405RG |
| STM32F407 | 128К | 64К | 512-1024К | STM32F407VG |
| STM32F411 | 128К | — | 256-512К | STM32F411CE |
| STM32F412 | 256К | — | 512-1024К | STM32F412VG |
| STM32F429 | 192К | 64К | 512-2048К | STM32F429ZI |
| STM32F439 | 192К | 64К | 512-2048К | STM32F439ZI |
| STM32F446 | 128К | — | 256-512К | STM32F446RE |

Формат имени чипа — `STM32F4xxYZ`, где:

- `Y` — буква корпуса (R/V/Z/…).
- `Z` — буква размера flash:
    - `B` → 128 КиБ
    - `C` → 256 КиБ
    - `E` → 512 КиБ
    - `G` → 1 МиБ
    - `I` → 2 МиБ

Размер flash декодируется автоматически в `sdk/cmake/families/stm32f4.cmake`.

## Добавление нового семейства

1. Создать `sdk/cmake/families/stm32XX.cmake` с функцией `stm32XX_get_chip_info`,
   заполняющей RAM, CCM, flash, флаги CPU.
2. Положить CMSIS device headers в `sdk/hal/stm32XX/include/cmsis/`.
3. Добавить vector tables в `sdk/hal/stm32XX/src/cmsis/`.
4. Создать `sdk/hal/stm32XX/ldscripts/mem.ld.in`.

Конкретный пример реализации — [STM32F4](stm32f4.md).

## Даташиты

См. [Даташиты](datasheets.md) — прямые ссылки на reference manuals, datasheets
и programming manuals от ST.
