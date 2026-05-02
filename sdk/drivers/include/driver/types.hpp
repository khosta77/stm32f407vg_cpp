#pragma once

#include <cstdint>
#include <cstddef>
#include <span>

namespace driver {

enum class Status : uint8_t {
    Ok,
    Timeout,
    Nack,
    BusError,
    Busy,
    InvalidArg,
    HardwareError
};

} // namespace driver
