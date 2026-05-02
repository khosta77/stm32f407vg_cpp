#pragma once

#include <cstdint>

namespace driver {

enum class PinMode : uint8_t {
    Input,
    Output,
    AlternateFunction,
    Analog
};

enum class PullMode : uint8_t {
    None,
    PullUp,
    PullDown
};

enum class OutputSpeed : uint8_t {
    Low,
    Medium,
    High,
    VeryHigh
};

enum class OutputType : uint8_t {
    PushPull,
    OpenDrain
};

struct GpioConfig {
    PinMode mode = PinMode::Input;
    PullMode pull = PullMode::None;
    OutputSpeed speed = OutputSpeed::Low;
    OutputType type = OutputType::PushPull;
    uint8_t af = 0;
};

class IGpio {
public:
    virtual ~IGpio() = default;
    virtual void configure(uint8_t pin, const GpioConfig& cfg) = 0;
    virtual void set(uint8_t pin) = 0;
    virtual void reset(uint8_t pin) = 0;
    virtual void toggle(uint8_t pin) = 0;
    virtual bool read(uint8_t pin) = 0;
};

} // namespace driver
