#pragma once

#include "driver/types.hpp"
#include <span>

namespace driver {

struct I2cConfig {
    uint32_t clockSpeed = 100000;
    bool fastMode = false;
};

class II2c {
public:
    virtual ~II2c() = default;

    virtual Status init(const I2cConfig& cfg) = 0;
    virtual void deinit() = 0;

    virtual Status write(uint8_t addr, std::span<const uint8_t> data, uint32_t timeoutMs = 100) = 0;
    virtual Status read(uint8_t addr, std::span<uint8_t> data, uint32_t timeoutMs = 100) = 0;

    virtual Status writeReg(uint8_t addr, uint8_t reg, std::span<const uint8_t> data, uint32_t timeoutMs = 100) = 0;
    virtual Status readReg(uint8_t addr, uint8_t reg, std::span<uint8_t> data, uint32_t timeoutMs = 100) = 0;

    virtual Status probe(uint8_t addr, uint32_t timeoutMs = 10) = 0;
};

} // namespace driver
