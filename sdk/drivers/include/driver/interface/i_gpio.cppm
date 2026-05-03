module;
#include <cstdint>
export module driver.gpio;

import driver.types;

export namespace driver
{

enum class PinMode : uint8_t
{
    None = 0xFF,
    Input = 0,
    Output = 1,
    AlternateFunction = 2,
    Analog = 3
};

enum class PullMode : uint8_t
{
    None = 0,
    PullUp = 1,
    PullDown = 2
};

enum class OutputSpeed : uint8_t
{
    None = 0xFF,
    Low = 0,
    Medium = 1,
    High = 2,
    VeryHigh = 3
};

enum class OutputType : uint8_t
{
    None = 0xFF,
    PushPull = 0,
    OpenDrain = 1
};

struct GpioConfig
{
    uint8_t pin;
    PinMode mode;
    PullMode pull;
    OutputSpeed speed;
    OutputType type;
    uint8_t af;

    consteval GpioConfig( uint8_t p, PinMode m, PullMode pu,
                          OutputSpeed s, OutputType t, uint8_t a = 0 )
        : pin( p ), mode( m ), pull( pu ), speed( s ), type( t ), af( a )
    {
        if ( p > 15 )
        {
            throw "pin must be 0-15";
        }
        if ( m == PinMode::None )
        {
            throw "PinMode must not be None";
        }
        if ( ( m == PinMode::Output || m == PinMode::AlternateFunction )
             && s == OutputSpeed::None )
        {
            throw "OutputSpeed required for Output/AF";
        }
        if ( ( m == PinMode::Output || m == PinMode::AlternateFunction )
             && t == OutputType::None )
        {
            throw "OutputType required for Output/AF";
        }
    }
};

class IGpioPin
{
public:
    virtual ~IGpioPin() = default;
    virtual void set() = 0;
    virtual void reset() = 0;
    virtual void toggle() = 0;
    virtual Status read() = 0;
};

} // namespace driver
