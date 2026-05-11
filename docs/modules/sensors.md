# Sensor interfaces

Sensor drivers live under `sdk/sensors/`. The intent is that every sensor of a
given kind exposes the same abstract interface — your application code stays
the same when you swap an SSD1306 for a different I2C display.

## `IDisplay` — `sensor.display`

```cpp
import sensor.display;
using sensor::IDisplay;

class MyDisplay : public IDisplay {
public:
    driver::Status init() override;
    void clear() override;
    void setPixel(uint16_t x, uint16_t y, bool on) override;
    void drawChar(uint16_t x, uint16_t y, char c) override;
    void drawText(uint16_t x, uint16_t y, const char *text) override;
    driver::Status flush() override;
    uint16_t width() const override;
    uint16_t height() const override;
};
```

`drawText` is intentionally `const char *` (not `std::string_view`) so call
sites don't need `#include <string_view>`. That's a C++20 modules constraint
on this SDK: header includes in `main.cpp` can collide with `import` of a
module that uses the same header in its global module fragment.

Existing implementation: `sensor.ssd1306`.

## `IExternalFlash` — `sensor.external_flash`

```cpp
import sensor.external_flash;
using sensor::IExternalFlash;

class MyFlash : public IExternalFlash {
public:
    driver::Status init() override;
    driver::Status read(uint32_t addr, std::span<uint8_t> data) override;
    driver::Status writePage(uint32_t addr, std::span<const uint8_t> data) override;
    driver::Status eraseSector(uint32_t addr) override;
    driver::Status chipErase() override;
    uint32_t capacity() const override;
    uint32_t sectorSize() const override;
    uint32_t pageSize() const override;
    uint32_t jedecId() override;
};
```

Page-aligned writes, sector-aligned erases. `chipErase()` is slow (seconds);
prefer per-sector erase when possible.

Existing implementation: `sensor.w25q32`.

## MPU6050

MPU6050 doesn't have a generic IMU interface yet (it's the only IMU shipped).
The driver exposes a plain `Sample` struct:

```cpp
struct Sample {
    int16_t ax, ay, az;
    int16_t temp;
    int16_t gx, gy, gz;
};
```

A future IMU interface will follow the same pattern as `IDisplay` /
`IExternalFlash` once a second IMU driver lands.

## Adding a new sensor

1. Decide the interface — if one exists for your sensor kind, implement it;
   otherwise define `sensor/<kind>.cppm` with the virtual interface.
2. Create `sdk/sensors/<category>/<name>/<name>.cppm` (e.g.
   `sdk/sensors/displays/ssd1306/ssd1306.cppm`).
3. Wire the module into `sdk/cmake/stm32_sensors.cmake`.
4. Add datasheet links and pinout in `docs/sensors/<name>.md` (EN and RU).
5. Bump `docs/modules/sensors.md` if the interface changed.
