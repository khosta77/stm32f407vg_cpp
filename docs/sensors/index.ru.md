# Обзор сенсоров

SDK поставляет драйверы трёх сенсоров, используемых в нескольких шаблонах.
Каждый драйвер реализует интерфейс из `sdk/sensors/include/sensor/` и собирается
в составе библиотеки `stm32_sensors`.

| Сенсор | Шина | Модуль | Интерфейс |
|--------|------|--------|-----------|
| [MPU6050](mpu6050.md) | I2C | `sensor.mpu6050` | `ISensor`-like (свой struct) |
| [SSD1306](ssd1306.md) | I2C | `sensor.ssd1306` | `IDisplay` |
| [W25Q32](w25q32.md) | SPI | `sensor.w25q32` | `IExternalFlash` |

## Импорт

```cpp
import sensor.mpu6050;
import sensor.ssd1306;
import sensor.w25q32;
```

## Общие интерфейсы

Интерфейсы лежат в `sdk/sensors/include/sensor/`:

- `display.cppm` — `IDisplay` (init, clear, setPixel, drawChar, drawText, flush, width, height).
- `external_flash.cppm` — `IExternalFlash` (init, read, writePage, eraseSector, chipErase, capacity, sectorSize, pageSize, jedecId).

При написании своих сенсоров предпочтительно реализовывать один из этих
интерфейсов — тогда код будет взаимозаменяем с существующими драйверами.

## Шаблоны, использующие сенсоры

| Шаблон | Используемые сенсоры |
|--------|----------------------|
| `freertos/mpu6050-uart` | MPU6050 + UART DMA |
| `freertos/oled-display-test` | SSD1306 |
| `freertos/w25q32-flash-test` | W25Q32 |
| `freertos/imu-flash-oled-demo` | MPU6050 + SSD1306 + W25Q32 |
