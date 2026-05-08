module;
#include <cstdint>
export module driver.gpio;

import driver.types;

export namespace driver {

enum class PinMode : uint8_t {
    None = 0xFF,
    Input = 0,
    Output = 1,
    AlternateFunction = 2,
    Analog = 3
};

enum class PullMode : uint8_t {
    None = 0,
    PullUp = 1,
    PullDown = 2
};

enum class OutputSpeed : uint8_t {
    None = 0xFF,
    Low = 0,
    Medium = 1,
    High = 2,
    VeryHigh = 3
};

enum class OutputType : uint8_t {
    None = 0xFF,
    PushPull = 0,
    OpenDrain = 1
};

struct GpioConfig {
    uint8_t pin;
    PinMode mode;
    PullMode pull;
    OutputSpeed speed;
    OutputType type;
    uint8_t af = 0;
};

consteval GpioConfig gpio(GpioConfig c) {
    if (c.pin > 15) {
        throw "GpioConfig: pin must be in [0, 15]";
    }
    if (c.mode == PinMode::None) {
        throw "GpioConfig: mode must not be PinMode::None";
    }
    const bool needsDrive =
        (c.mode == PinMode::Output || c.mode == PinMode::AlternateFunction);
    if (needsDrive && c.speed == OutputSpeed::None) {
        throw "GpioConfig: OutputSpeed required for Output/AlternateFunction";
    }
    if (needsDrive && c.type == OutputType::None) {
        throw "GpioConfig: OutputType required for Output/AlternateFunction";
    }
    if (c.mode == PinMode::AlternateFunction && c.af > 15) {
        throw "GpioConfig: af must be in [0, 15]";
    }
    return c;
}

class IGpioPin {
public:
    virtual ~IGpioPin() = default;
    virtual void set() = 0;
    virtual void reset() = 0;
    virtual void toggle() = 0;
    virtual Status read() = 0;
};

}  // namespace driver
