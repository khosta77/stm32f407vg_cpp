#pragma once

#include "driver/types.hpp"
#include <span>

namespace driver
{

class IFlash
{
public:
    virtual ~IFlash() = default;

    virtual Status read( uint32_t addr, std::span<uint8_t> data ) = 0;
    virtual Status write( uint32_t addr, std::span<const uint8_t> data ) = 0;
    virtual Status eraseSector( uint8_t sector ) = 0;
    virtual size_t sectorSize( uint8_t sector ) const = 0;
    virtual uint8_t sectorCount() const = 0;
};

} // namespace driver
