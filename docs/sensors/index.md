# Sensors overview

The SDK ships drivers for three sensors used across multiple templates. Each
sensor implements an interface from `sdk/sensors/include/sensor/` and is built
as part of the `stm32_sensors` library.

| Sensor | Bus | Module | Interface |
|--------|-----|--------|-----------|
| [MPU6050](mpu6050.md) | I2C | `sensor.mpu6050` | `ISensor`-like (custom struct) |
| [SSD1306](ssd1306.md) | I2C | `sensor.ssd1306` | `IDisplay` |
| [W25Q32](w25q32.md) | SPI | `sensor.w25q32` | `IExternalFlash` |

## Importing

```cpp
import sensor.mpu6050;
import sensor.ssd1306;
import sensor.w25q32;
```

## Shared interfaces

Interfaces live in `sdk/sensors/include/sensor/`:

- `display.cppm` — `IDisplay` (init, clear, setPixel, drawChar, drawText, flush, width, height).
- `external_flash.cppm` — `IExternalFlash` (init, read, writePage, eraseSector, chipErase, capacity, sectorSize, pageSize, jedecId).

When writing custom sensors, prefer implementing one of these interfaces so
your code is interchangeable with existing drivers.

## Templates that exercise sensors

| Template | Sensors used |
|----------|--------------|
| `freertos/mpu6050-uart` | MPU6050 + UART DMA |
| `freertos/oled-display-test` | SSD1306 |
| `freertos/w25q32-flash-test` | W25Q32 |
| `freertos/imu-flash-oled-demo` | MPU6050 + SSD1306 + W25Q32 |
