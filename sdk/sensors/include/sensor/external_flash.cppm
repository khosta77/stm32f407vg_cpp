module;
#include <cstddef>
#include <cstdint>
#include <span>
export module sensor.external_flash;

import driver.types;

export namespace sensor {

class IExternalFlash {
public:
    virtual ~IExternalFlash() = default;

    virtual driver::Status init() = 0;
    virtual driver::Status read(uint32_t addr, std::span<uint8_t> data) = 0;
    virtual driver::Status writePage(uint32_t addr, std::span<const uint8_t> data) = 0;
    virtual driver::Status eraseSector(uint32_t addr) = 0;
    virtual driver::Status chipErase() = 0;

    virtual uint32_t capacity() const = 0;
    virtual uint32_t sectorSize() const = 0;
    virtual uint32_t pageSize() const = 0;
    virtual uint32_t jedecId() const = 0;
};

}  // namespace sensor
