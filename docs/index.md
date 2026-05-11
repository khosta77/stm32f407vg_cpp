# stm32-sdk

Bare-metal C++20 SDK for STM32 microcontrollers with optional FreeRTOS support.
CMSIS only, no HAL or LL.

## What you get

- **Drivers**: GPIO, I2C, UART (interrupt and DMA TX), SPI, DMA, internal flash.
- **Sensors**: MPU6050 IMU, SSD1306 OLED display, W25Q32 SPI flash.
- **Project templates**: 7 ready-to-build templates covering bare-metal and FreeRTOS scenarios.
- **`stmtool` CLI**: project scaffolding, build, flash, SDK update — all in one Python tool.
- **C++20 modules** throughout — no header bloat, deterministic compilation order.

## Where to go next

- [Quick start](quickstart.md) — install, create a project, flash in under five minutes.
- [stmtool reference](stmtool.md) — every command and flag.
- [Modules](modules/drivers.md) — API reference for drivers, sensors, FreeRTOS helpers.
- [Sensors](sensors/index.md) — pinout, datasheet links, integration examples.
- [Chips](chips/index.md) — supported STM32 families.
- [Upgrade notes](migration.md) — what to change when bumping the SDK version.
- [Release](release.md) — versioning policy, release history.

## Languages

This documentation is published in **English** (default) and **Русский**.
Use the language selector in the header to switch.

## Project status

Pre-1.0. Public APIs are still being refined. Pin a specific SDK tag in your
project's `stmproject.toml` (instead of `develop`) for reproducibility.
