#include "driver/flash.hpp"
#include "cmsis/stm32f4xx.h"

namespace driver {
namespace stm32f4 {

class InternalFlash : public IFlash {
    static constexpr uint32_t FLASH_KEY1 = 0x45670123;
    static constexpr uint32_t FLASH_KEY2 = 0xCDEF89AB;

    void unlock() {
        if (FLASH->CR & FLASH_CR_LOCK) {
            FLASH->KEYR = FLASH_KEY1;
            FLASH->KEYR = FLASH_KEY2;
        }
    }

    void lock() {
        FLASH->CR |= FLASH_CR_LOCK;
    }

    bool waitComplete() const {
        while (FLASH->SR & FLASH_SR_BSY) {}
        return !(FLASH->SR & (FLASH_SR_PGSERR | FLASH_SR_PGPERR |
                              FLASH_SR_PGAERR | FLASH_SR_WRPERR));
    }

    void clearErrors() {
        FLASH->SR = FLASH_SR_PGSERR | FLASH_SR_PGPERR |
                    FLASH_SR_PGAERR | FLASH_SR_WRPERR | FLASH_SR_EOP;
    }

public:
    bool read(uint32_t addr, uint8_t* data, size_t len) override {
        const auto* src = reinterpret_cast<const uint8_t*>(addr);
        for (size_t i = 0; i < len; ++i) {
            data[i] = src[i];
        }
        return true;
    }

    bool write(uint32_t addr, const uint8_t* data, size_t len) override {
        unlock();
        clearErrors();

        FLASH->CR &= ~(FLASH_CR_PSIZE_Msk);
        FLASH->CR |= FLASH_CR_PG;

        auto* dst = reinterpret_cast<volatile uint8_t*>(addr);
        for (size_t i = 0; i < len; ++i) {
            dst[i] = data[i];
            if (!waitComplete()) {
                FLASH->CR &= ~FLASH_CR_PG;
                lock();
                return false;
            }
        }

        FLASH->CR &= ~FLASH_CR_PG;
        lock();
        return true;
    }

    bool eraseSector(uint8_t sector) override {
        unlock();
        clearErrors();

        FLASH->CR &= ~(FLASH_CR_PSIZE_Msk | FLASH_CR_SNB_Msk);
        FLASH->CR |= FLASH_CR_SER
                    | (static_cast<uint32_t>(sector) << FLASH_CR_SNB_Pos)
                    | (2U << FLASH_CR_PSIZE_Pos);
        FLASH->CR |= FLASH_CR_STRT;

        bool ok = waitComplete();

        FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB_Msk);
        lock();
        return ok;
    }

    size_t sectorSize(uint8_t sector) const override {
        if (sector < 4) return 16 * 1024;
        if (sector == 4) return 64 * 1024;
        return 128 * 1024;
    }

    uint8_t sectorCount() const override {
        return 12;
    }
};

} // namespace stm32f4
} // namespace driver
