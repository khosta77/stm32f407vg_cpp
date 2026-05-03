module;
#include <cstdint>
export module sensor.imu;

import driver.types;

export namespace sensor {

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
    virtual driver::Status init() = 0;
    virtual driver::Status read(ImuData &out) = 0;
    virtual driver::Status selfTest() = 0;
    virtual void setAccelRange(uint8_t g) = 0;
    virtual void setGyroRange(uint16_t dps) = 0;
};

}  // namespace sensor
