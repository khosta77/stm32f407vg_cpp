#include "driver/i2c.hpp"
#include "cmsis/stm32f4xx.h"

#ifdef STM32_USE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

namespace driver {
namespace stm32f4 {

class I2c : public II2c {
    I2C_TypeDef* const _periph;
    bool _initialized = false;

#ifdef STM32_USE_FREERTOS
    SemaphoreHandle_t _mutex = nullptr;
#endif

    static constexpr uint32_t TIMEOUT_LOOPS = 100000;

    bool waitFlag(volatile uint32_t& reg, uint32_t flag, bool set) const {
        for (uint32_t i = 0; i < TIMEOUT_LOOPS; ++i) {
            if (set) {
                if (reg & flag) return true;
            } else {
                if (!(reg & flag)) return true;
            }
        }
        return false;
    }

    bool waitBusy() const {
        return waitFlag(_periph->SR2, I2C_SR2_BUSY, false);
    }

    void generateStart() const {
        _periph->CR1 |= I2C_CR1_START;
        waitFlag(_periph->SR1, I2C_SR1_SB, true);
    }

    void generateStop() const {
        _periph->CR1 |= I2C_CR1_STOP;
    }

    bool sendAddress(uint8_t addr, bool read) const {
        _periph->DR = (addr << 1) | (read ? 1 : 0);
        if (!waitFlag(_periph->SR1, I2C_SR1_ADDR, true)) {
            if (_periph->SR1 & I2C_SR1_AF) {
                _periph->SR1 &= ~I2C_SR1_AF;
                generateStop();
                return false;
            }
            return false;
        }
        volatile uint32_t dummy = _periph->SR2;
        (void)dummy;
        return true;
    }

    void busRecovery() {
        _periph->CR1 |= I2C_CR1_SWRST;
        _periph->CR1 &= ~I2C_CR1_SWRST;
    }

public:
    explicit I2c(I2C_TypeDef* periph) : _periph(periph) {}

    bool init(const I2cConfig& cfg) override {
        uint32_t pclk = SystemCoreClock / 4;

        _periph->CR1 = I2C_CR1_SWRST;
        _periph->CR1 = 0;

        uint32_t freqMhz = pclk / 1000000;
        _periph->CR2 = freqMhz & I2C_CR2_FREQ;

        if (cfg.fastMode || cfg.clockSpeed > 100000) {
            _periph->CCR = I2C_CCR_FS | (pclk / (cfg.clockSpeed * 3));
            _periph->TRISE = (freqMhz * 300 / 1000) + 1;
        } else {
            _periph->CCR = pclk / (cfg.clockSpeed * 2);
            _periph->TRISE = freqMhz + 1;
        }

        _periph->CR1 = I2C_CR1_PE;

#ifdef STM32_USE_FREERTOS
        _mutex = xSemaphoreCreateMutex();
#endif

        _initialized = true;
        return true;
    }

    void deinit() override {
        _periph->CR1 = 0;

#ifdef STM32_USE_FREERTOS
        if (_mutex) vSemaphoreDelete(_mutex);
        _mutex = nullptr;
#endif

        _initialized = false;
    }

    bool write(uint8_t addr, const uint8_t* data, size_t len, uint32_t timeoutMs) override {
        (void)timeoutMs;

#ifdef STM32_USE_FREERTOS
        xSemaphoreTake(_mutex, portMAX_DELAY);
#endif

        bool ok = false;

        if (waitBusy()) {
            generateStart();
            if (sendAddress(addr, false)) {
                ok = true;
                for (size_t i = 0; i < len; ++i) {
                    if (!waitFlag(_periph->SR1, I2C_SR1_TXE, true)) {
                        ok = false;
                        break;
                    }
                    _periph->DR = data[i];
                }
                if (ok) waitFlag(_periph->SR1, I2C_SR1_BTF, true);
                generateStop();
            }
        }

        if (!ok) busRecovery();

#ifdef STM32_USE_FREERTOS
        xSemaphoreGive(_mutex);
#endif

        return ok;
    }

    bool read(uint8_t addr, uint8_t* data, size_t len, uint32_t timeoutMs) override {
        (void)timeoutMs;

#ifdef STM32_USE_FREERTOS
        xSemaphoreTake(_mutex, portMAX_DELAY);
#endif

        bool ok = false;

        if (waitBusy()) {
            if (len == 1) _periph->CR1 &= ~I2C_CR1_ACK;
            else _periph->CR1 |= I2C_CR1_ACK;

            generateStart();
            if (sendAddress(addr, true)) {
                ok = true;
                for (size_t i = 0; i < len; ++i) {
                    if (i == len - 1) {
                        _periph->CR1 &= ~I2C_CR1_ACK;
                        generateStop();
                    }
                    if (!waitFlag(_periph->SR1, I2C_SR1_RXNE, true)) {
                        ok = false;
                        break;
                    }
                    data[i] = static_cast<uint8_t>(_periph->DR);
                }
            }
        }

        if (!ok) {
            generateStop();
            busRecovery();
        }

#ifdef STM32_USE_FREERTOS
        xSemaphoreGive(_mutex);
#endif

        return ok;
    }

    bool writeReg(uint8_t addr, uint8_t reg, const uint8_t* data, size_t len, uint32_t timeoutMs) override {
        (void)timeoutMs;

#ifdef STM32_USE_FREERTOS
        xSemaphoreTake(_mutex, portMAX_DELAY);
#endif

        bool ok = false;

        if (waitBusy()) {
            generateStart();
            if (sendAddress(addr, false)) {
                if (waitFlag(_periph->SR1, I2C_SR1_TXE, true)) {
                    _periph->DR = reg;
                    ok = true;
                    for (size_t i = 0; i < len; ++i) {
                        if (!waitFlag(_periph->SR1, I2C_SR1_TXE, true)) {
                            ok = false;
                            break;
                        }
                        _periph->DR = data[i];
                    }
                    if (ok) waitFlag(_periph->SR1, I2C_SR1_BTF, true);
                }
                generateStop();
            }
        }

        if (!ok) busRecovery();

#ifdef STM32_USE_FREERTOS
        xSemaphoreGive(_mutex);
#endif

        return ok;
    }

    bool readReg(uint8_t addr, uint8_t reg, uint8_t* data, size_t len, uint32_t timeoutMs) override {
        (void)timeoutMs;

#ifdef STM32_USE_FREERTOS
        xSemaphoreTake(_mutex, portMAX_DELAY);
#endif

        bool ok = false;

        if (waitBusy()) {
            generateStart();
            if (sendAddress(addr, false)) {
                if (waitFlag(_periph->SR1, I2C_SR1_TXE, true)) {
                    _periph->DR = reg;
                    waitFlag(_periph->SR1, I2C_SR1_BTF, true);

                    if (len == 1) _periph->CR1 &= ~I2C_CR1_ACK;
                    else _periph->CR1 |= I2C_CR1_ACK;

                    generateStart();
                    if (sendAddress(addr, true)) {
                        ok = true;
                        for (size_t i = 0; i < len; ++i) {
                            if (i == len - 1) {
                                _periph->CR1 &= ~I2C_CR1_ACK;
                                generateStop();
                            }
                            if (!waitFlag(_periph->SR1, I2C_SR1_RXNE, true)) {
                                ok = false;
                                break;
                            }
                            data[i] = static_cast<uint8_t>(_periph->DR);
                        }
                    }
                }
            }
        }

        if (!ok) {
            generateStop();
            busRecovery();
        }

#ifdef STM32_USE_FREERTOS
        xSemaphoreGive(_mutex);
#endif

        return ok;
    }

    bool probe(uint8_t addr, uint32_t timeoutMs) override {
        (void)timeoutMs;

#ifdef STM32_USE_FREERTOS
        xSemaphoreTake(_mutex, portMAX_DELAY);
#endif

        bool found = false;
        if (waitBusy()) {
            generateStart();
            found = sendAddress(addr, false);
            generateStop();
        }

#ifdef STM32_USE_FREERTOS
        xSemaphoreGive(_mutex);
#endif

        return found;
    }
};

} // namespace stm32f4
} // namespace driver
