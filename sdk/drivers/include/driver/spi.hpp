#pragma once

#include "driver/types.hpp"
#include <span>

namespace driver {

struct SpiConfig {
    uint32_t clockHz = 1000000;
    uint8_t mode = 0;
    bool lsbFirst = false;
    uint8_t dataSize = 8;
};

class ISpi {
public:
    virtual ~ISpi() = default;

    virtual Status init(const SpiConfig& cfg) = 0;
    virtual void deinit() = 0;

    virtual Status transfer(std::span<const uint8_t> txData, std::span<uint8_t> rxData) = 0;
    virtual Status write(std::span<const uint8_t> data) = 0;
    virtual Status read(std::span<uint8_t> data) = 0;
};

} // namespace driver
