module;
#include <cstddef>
#include <cstdint>
export module driver.types;

export namespace driver {

enum class Status : uint8_t { Ok, None, Timeout, Nack, BusError, Busy, InvalidArg, HardwareError };

}  // namespace driver
