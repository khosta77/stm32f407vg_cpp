#pragma once

#include "driver/types.hpp"
#include <span>

namespace driver {

struct UartConfig {
    uint32_t baudrate = 115200;
    uint8_t dataBits = 8;
    uint8_t stopBits = 1;
    enum class Parity : uint8_t { None, Even, Odd } parity = Parity::None;
};

class IUart {
public:
    virtual ~IUart() = default;

    virtual Status init(const UartConfig& cfg) = 0;
    virtual void deinit() = 0;

    virtual size_t write(std::span<const uint8_t> data, uint32_t timeoutMs = 0xFFFFFFFF) = 0;
    virtual size_t read(std::span<uint8_t> data, uint32_t timeoutMs = 0xFFFFFFFF) = 0;

    virtual size_t writeNonBlocking(std::span<const uint8_t> data) = 0;
    virtual size_t readNonBlocking(std::span<uint8_t> data) = 0;

    virtual size_t rxAvailable() const = 0;
    virtual size_t txFree() const = 0;

    virtual void irqHandler() = 0;
};

} // namespace driver
