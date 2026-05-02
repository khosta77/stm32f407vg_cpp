#pragma once

#include "driver/types.hpp"
#include <span>

namespace driver
{

class II2c
{
public:
    virtual ~II2c() = default;

    virtual Status write( uint8_t addr, std::span<const uint8_t> data ) = 0;
    virtual Status read( uint8_t addr, std::span<uint8_t> data ) = 0;

    virtual Status writeReg( uint8_t addr, uint8_t reg, std::span<const uint8_t> data ) = 0;
    virtual Status readReg( uint8_t addr, uint8_t reg, std::span<uint8_t> data ) = 0;

    virtual Status probe( uint8_t addr ) = 0;
};

} // namespace driver
