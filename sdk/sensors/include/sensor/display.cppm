module;
#include <cstddef>
#include <cstdint>
export module sensor.display;

import driver.types;

export namespace sensor {

class IDisplay {
public:
    virtual ~IDisplay() = default;

    virtual driver::Status init() = 0;
    virtual void clear() = 0;
    virtual void setPixel(uint16_t x, uint16_t y, bool on) = 0;
    virtual void drawChar(uint16_t x, uint16_t y, char c) = 0;
    virtual void drawText(uint16_t x, uint16_t y, const char *text) = 0;
    virtual driver::Status flush() = 0;

    virtual uint16_t width() const = 0;
    virtual uint16_t height() const = 0;
};

}  // namespace sensor
