#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace driver
{

enum class Status : uint8_t
{
    Ok,
    Timeout,
    Nack,
    BusError,
    Busy,
    InvalidArg,
    HardwareError
};

} // namespace driver
