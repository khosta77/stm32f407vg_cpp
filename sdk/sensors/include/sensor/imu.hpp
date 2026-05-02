#pragma once

#include <cstdint>

namespace sensor {

struct Vec3f {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct ImuData {
    Vec3f accel;
    Vec3f gyro;
    float temp = 0.0f;
};

class IImu {
public:
    virtual ~IImu() = default;
    virtual bool init() = 0;
    virtual bool read(ImuData& out) = 0;
    virtual bool selfTest() = 0;
    virtual void setAccelRange(uint8_t g) = 0;
    virtual void setGyroRange(uint16_t dps) = 0;
};

} // namespace sensor
