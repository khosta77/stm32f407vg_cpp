#pragma once

#include <cstdint>
#include <cstddef>

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

    virtual bool init(const SpiConfig& cfg) = 0;
    virtual void deinit() = 0;

    virtual bool transfer(const uint8_t* txData, uint8_t* rxData, size_t len, uint32_t timeoutMs = 100) = 0;
    virtual bool write(const uint8_t* data, size_t len, uint32_t timeoutMs = 100) = 0;
    virtual bool read(uint8_t* data, size_t len, uint32_t timeoutMs = 100) = 0;
};

} // namespace driver
