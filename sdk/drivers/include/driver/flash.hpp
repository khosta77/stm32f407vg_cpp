#pragma once

#include <cstdint>
#include <cstddef>

namespace driver {

class IFlash {
public:
    virtual ~IFlash() = default;

    virtual bool read(uint32_t addr, uint8_t* data, size_t len) = 0;
    virtual bool write(uint32_t addr, const uint8_t* data, size_t len) = 0;
    virtual bool eraseSector(uint8_t sector) = 0;
    virtual size_t sectorSize(uint8_t sector) const = 0;
    virtual uint8_t sectorCount() const = 0;
};

} // namespace driver
