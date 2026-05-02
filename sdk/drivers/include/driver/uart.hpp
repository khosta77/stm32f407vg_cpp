#pragma once

#include "driver/types.hpp"
#include <span>

namespace driver
{

class IUart
{
public:
    virtual ~IUart() = default;

    virtual size_t write( std::span<const uint8_t> data ) = 0;
    virtual size_t read( std::span<uint8_t> data ) = 0;

    virtual size_t writeNonBlocking( std::span<const uint8_t> data ) = 0;
    virtual size_t readNonBlocking( std::span<uint8_t> data ) = 0;

    virtual size_t rxAvailable() const = 0;
    virtual size_t txFree() const = 0;

    virtual void irqHandler() = 0;
};

} // namespace driver
