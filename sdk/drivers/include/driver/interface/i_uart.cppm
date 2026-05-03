module;
#include <cstddef>
#include <cstdint>
#include <span>
export module driver.uart;

import driver.types;

export namespace driver {

enum class Parity : uint8_t {
    None,
    Even,
    Odd
};

class IUart {
public:
    virtual ~IUart() = default;

    virtual size_t write(std::span<const uint8_t> data) = 0;
    virtual size_t read(std::span<uint8_t> data) = 0;

    virtual size_t writeNonBlocking(std::span<const uint8_t> data) = 0;
    virtual size_t readNonBlocking(std::span<uint8_t> data) = 0;

    virtual size_t rxAvailable() const = 0;
    virtual size_t txFree() const = 0;

    virtual void irqHandler() = 0;
};

}  // namespace driver
