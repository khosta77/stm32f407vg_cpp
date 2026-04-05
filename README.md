# stm32-sdk

Bare-metal C++17 SDK for STM32 microcontrollers. CMSIS only, no HAL/LL.

## Quick Start

```bash
pip install ./tools/stmtool
stmtool project create my-blink --chip STM32F407VG
cd my-blink
stmtool build --native
stmtool flash
```

## Requirements

- `arm-none-eabi-gcc` >= 12
- `cmake` >= 3.20
- `python` >= 3.10
- `st-flash` (optional, for flashing via ST-Link)

## stmtool CLI

| Command | Description |
|---------|-------------|
| `stmtool project create <name> --chip <chip>` | Create new project from template |
| `stmtool build` | Build project (Docker by default) |
| `stmtool build --native` | Build locally without Docker |
| `stmtool build --clean` | Clean build directory before building |
| `stmtool flash` | Flash firmware via st-link |
| `stmtool doctor` | Check development environment |
| `stmtool version` | Show stmtool version |

### Environment variables

| Variable | Description |
|----------|-------------|
| `STMSDK_PATH` | Path to SDK repository (auto-detected if stmtool installed from repo) |
| `STMSDK_REPO` | Override git URL for SDK submodule |
| `STMTOOL_LANG` | UI language: `en` (default), `ru` |

## Supported chips

| Family | RAM | CCM | Flash | Example |
|--------|-----|-----|-------|---------|
| STM32F401 | 64-96K | - | 128-512K | STM32F401CC, STM32F401RE |
| STM32F405 | 128K | 64K | 512-1024K | STM32F405RG |
| STM32F407 | 128K | 64K | 512-1024K | STM32F407VG |
| STM32F411 | 128K | - | 256-512K | STM32F411CE |
| STM32F412 | 256K | - | 512-1024K | STM32F412VG |
| STM32F429 | 192K | 64K | 512-2048K | STM32F429ZI |
| STM32F439 | 192K | 64K | 512-2048K | STM32F439ZI |
| STM32F446 | 128K | - | 256-512K | STM32F446RE |

Flash size is determined from chip name (letter at position 11: B=128K, C=256K, E=512K, G=1024K, I=2048K).

## Project structure

```
stm32-sdk/
  sdk/
    cmake/            CMake toolchain and family configs
    core/             Cortex-M runtime, newlib, linker scripts
    hal/stm32f4/      STM32F4 CMSIS headers, vectors, memory layout
  templates/          Project templates (blink, etc.)
  tools/stmtool/      Python CLI tool
  docker/             Build environment Dockerfiles
```

Generated project:
```
my-project/
  src/main.cpp        Application code
  stm32-sdk/          Git submodule -> stm32-sdk repo
  CMakeLists.txt      Build configuration
  stmproject.toml     Project config (chip, flash tool, etc.)
```

## Adding a new chip family

1. Create `sdk/cmake/families/stm32XX.cmake` with `stm32XX_get_chip_info` function
2. Add CMSIS device headers to `sdk/hal/stm32XX/include/cmsis/`
3. Create vector tables in `sdk/hal/stm32XX/src/cmsis/`
4. Create `sdk/hal/stm32XX/ldscripts/mem.ld.in`

## License

MIT
