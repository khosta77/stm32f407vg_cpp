#pragma once

#include <cstdint>
#include <cstddef>

namespace driver {

struct I2cConfig {
    uint32_t clockSpeed = 100000;
    bool fastMode = false;
};

class II2c {
public:
    virtual ~II2c() = default;

    virtual bool init(const I2cConfig& cfg) = 0;
    virtual void deinit() = 0;

    virtual bool write(uint8_t addr, const uint8_t* data, size_t len, uint32_t timeoutMs = 100) = 0;
    virtual bool read(uint8_t addr, uint8_t* data, size_t len, uint32_t timeoutMs = 100) = 0;

    virtual bool writeReg(uint8_t addr, uint8_t reg, const uint8_t* data, size_t len, uint32_t timeoutMs = 100) = 0;
    virtual bool readReg(uint8_t addr, uint8_t reg, uint8_t* data, size_t len, uint32_t timeoutMs = 100) = 0;

    virtual bool probe(uint8_t addr, uint32_t timeoutMs = 10) = 0;
};

} // namespace driver
