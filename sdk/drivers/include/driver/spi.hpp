#pragma once

#include "driver/types.hpp"
#include <span>

namespace driver
{

class ISpi
{
public:
    virtual ~ISpi() = default;

    virtual Status transfer( std::span<const uint8_t> txData, std::span<uint8_t> rxData ) = 0;
    virtual Status write( std::span<const uint8_t> data ) = 0;
    virtual Status read( std::span<uint8_t> data ) = 0;
};

} // namespace driver
