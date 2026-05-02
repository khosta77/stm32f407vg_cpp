#include "driver/spi.hpp"
#include "cmsis/stm32f4xx.h"

#ifdef STM32_USE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

namespace driver {
namespace stm32f4 {

class Spi : public ISpi {
    SPI_TypeDef* const _periph;
    bool _initialized = false;

#ifdef STM32_USE_FREERTOS
    SemaphoreHandle_t _mutex = nullptr;
#endif

    static constexpr uint32_t TIMEOUT_LOOPS = 100000;

    bool waitTxe() const {
        for (uint32_t i = 0; i < TIMEOUT_LOOPS; ++i) {
            if (_periph->SR & SPI_SR_TXE) return true;
        }
        return false;
    }

    bool waitRxne() const {
        for (uint32_t i = 0; i < TIMEOUT_LOOPS; ++i) {
            if (_periph->SR & SPI_SR_RXNE) return true;
        }
        return false;
    }

    bool waitNotBusy() const {
        for (uint32_t i = 0; i < TIMEOUT_LOOPS; ++i) {
            if (!(_periph->SR & SPI_SR_BSY)) return true;
        }
        return false;
    }

public:
    explicit Spi(SPI_TypeDef* periph) : _periph(periph) {}

    bool init(const SpiConfig& cfg) override {
        _periph->CR1 = 0;

        uint32_t cr1 = SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;

        uint32_t pclk = SystemCoreClock / 4;
        uint32_t div = pclk / cfg.clockHz;
        uint32_t br = 0;
        while ((1U << (br + 1)) < div && br < 7) ++br;
        cr1 |= (br << SPI_CR1_BR_Pos);

        if (cfg.mode & 0x01) cr1 |= SPI_CR1_CPHA;
        if (cfg.mode & 0x02) cr1 |= SPI_CR1_CPOL;
        if (cfg.lsbFirst) cr1 |= SPI_CR1_LSBFIRST;
        if (cfg.dataSize == 16) cr1 |= SPI_CR1_DFF;

        _periph->CR1 = cr1;
        _periph->CR1 |= SPI_CR1_SPE;

#ifdef STM32_USE_FREERTOS
        _mutex = xSemaphoreCreateMutex();
#endif

        _initialized = true;
        return true;
    }

    void deinit() override {
        waitNotBusy();
        _periph->CR1 = 0;

#ifdef STM32_USE_FREERTOS
        if (_mutex) vSemaphoreDelete(_mutex);
        _mutex = nullptr;
#endif

        _initialized = false;
    }

    bool transfer(const uint8_t* txData, uint8_t* rxData, size_t len, uint32_t timeoutMs) override {
        (void)timeoutMs;

#ifdef STM32_USE_FREERTOS
        xSemaphoreTake(_mutex, portMAX_DELAY);
#endif

        bool ok = true;
        for (size_t i = 0; i < len; ++i) {
            if (!waitTxe()) { ok = false; break; }
            _periph->DR = txData ? txData[i] : 0xFF;
            if (!waitRxne()) { ok = false; break; }
            uint8_t rx = static_cast<uint8_t>(_periph->DR);
            if (rxData) rxData[i] = rx;
        }
        waitNotBusy();

#ifdef STM32_USE_FREERTOS
        xSemaphoreGive(_mutex);
#endif

        return ok;
    }

    bool write(const uint8_t* data, size_t len, uint32_t timeoutMs) override {
        return transfer(data, nullptr, len, timeoutMs);
    }

    bool read(uint8_t* data, size_t len, uint32_t timeoutMs) override {
        return transfer(nullptr, data, len, timeoutMs);
    }
};

} // namespace stm32f4
} // namespace driver
