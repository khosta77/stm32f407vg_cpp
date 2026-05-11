# Chips overview

The SDK currently targets the STM32F4 family. Chip selection happens at
configure time via `-DSTM32_CHIP=<name>` (set automatically by `stmtool` from
`stmproject.toml`).

| Family | RAM | CCM | Flash | Example part |
|--------|-----|-----|-------|--------------|
| STM32F401 | 64-96K | — | 128-512K | STM32F401CC, STM32F401RE |
| STM32F405 | 128K | 64K | 512-1024K | STM32F405RG |
| STM32F407 | 128K | 64K | 512-1024K | STM32F407VG |
| STM32F411 | 128K | — | 256-512K | STM32F411CE |
| STM32F412 | 256K | — | 512-1024K | STM32F412VG |
| STM32F429 | 192K | 64K | 512-2048K | STM32F429ZI |
| STM32F439 | 192K | 64K | 512-2048K | STM32F439ZI |
| STM32F446 | 128K | — | 256-512K | STM32F446RE |

The chip-name convention is `STM32F4xxYZ` where:

- `Y` — package letter (R/V/Z/…).
- `Z` — flash size letter:
    - `B` → 128 KiB
    - `C` → 256 KiB
    - `E` → 512 KiB
    - `G` → 1 MiB
    - `I` → 2 MiB

Flash size is decoded automatically by `sdk/cmake/families/stm32f4.cmake`.

## Adding a new family

1. Create `sdk/cmake/families/stm32XX.cmake` with a `stm32XX_get_chip_info`
   function that fills RAM, CCM, flash size, and CPU flags.
2. Add CMSIS device headers under `sdk/hal/stm32XX/include/cmsis/`.
3. Add vector tables under `sdk/hal/stm32XX/src/cmsis/`.
4. Create `sdk/hal/stm32XX/ldscripts/mem.ld.in`.

See [STM32F4](stm32f4.md) for a concrete implementation reference.

## Datasheets

See [Datasheets](datasheets.md) for direct links to ST's reference manuals,
datasheets, and programming manuals.
