#pragma once

#include "driver/uart.hpp"
#include "driver/circular_buffer.hpp"
#include "cmsis/stm32f4xx.h"

#ifdef STM32_USE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

namespace driver {
namespace stm32f4 {

class Uart : public IUart {
    USART_TypeDef* const _periph;
    IRQn_Type const _irqn;
    CircularBuffer<uint8_t, 256> _rxBuf;
    CircularBuffer<uint8_t, 256> _txBuf;
    bool _initialized = false;

#ifdef STM32_USE_FREERTOS
    SemaphoreHandle_t _rxSem = nullptr;
    SemaphoreHandle_t _txSem = nullptr;
    SemaphoreHandle_t _mutex = nullptr;
#endif

public:
    Uart(USART_TypeDef* periph, IRQn_Type irqn)
        : _periph(periph), _irqn(irqn) {}

    Status init(const UartConfig& cfg) override {
        uint32_t pclk;
        if (_periph == USART1 || _periph == USART6) {
            pclk = SystemCoreClock / 2;
        } else {
            pclk = SystemCoreClock / 4;
        }

        _periph->CR1 = 0;
        _periph->CR2 = 0;
        _periph->CR3 = 0;

        _periph->BRR = (pclk + cfg.baudrate / 2) / cfg.baudrate;

        uint32_t cr1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

        if (cfg.dataBits == 9) cr1 |= USART_CR1_M;

        if (cfg.parity == UartConfig::Parity::Even) {
            cr1 |= USART_CR1_PCE;
        } else if (cfg.parity == UartConfig::Parity::Odd) {
            cr1 |= USART_CR1_PCE | USART_CR1_PS;
        }

        if (cfg.stopBits == 2) {
            _periph->CR2 |= USART_CR2_STOP_1;
        }

        cr1 |= USART_CR1_RXNEIE;
        _periph->CR1 = cr1;

        NVIC_SetPriority(_irqn, 6);
        NVIC_EnableIRQ(_irqn);

#ifdef STM32_USE_FREERTOS
        _rxSem = xSemaphoreCreateBinary();
        _txSem = xSemaphoreCreateBinary();
        _mutex = xSemaphoreCreateMutex();
        xSemaphoreGive(_txSem);
#endif

        _initialized = true;
        return Status::Ok;
    }

    void deinit() override {
        NVIC_DisableIRQ(_irqn);
        _periph->CR1 = 0;

#ifdef STM32_USE_FREERTOS
        if (_rxSem) vSemaphoreDelete(_rxSem);
        if (_txSem) vSemaphoreDelete(_txSem);
        if (_mutex) vSemaphoreDelete(_mutex);
        _rxSem = nullptr;
        _txSem = nullptr;
        _mutex = nullptr;
#endif

        _initialized = false;
    }

    size_t write(std::span<const uint8_t> data, uint32_t timeoutMs = 0xFFFFFFFF) override {
#ifdef STM32_USE_FREERTOS
        xSemaphoreTake(_mutex, portMAX_DELAY);
#endif
        size_t sent = 0;
        while (sent < data.size()) {
            if (_txBuf.push(data[sent])) {
                _periph->CR1 |= USART_CR1_TXEIE;
                ++sent;
            } else {
#ifdef STM32_USE_FREERTOS
                _periph->CR1 |= USART_CR1_TXEIE;
                if (!xSemaphoreTake(_txSem, pdMS_TO_TICKS(timeoutMs))) break;
#else
                (void)timeoutMs;
                while (_txBuf.full()) {}
#endif
            }
        }
#ifdef STM32_USE_FREERTOS
        xSemaphoreGive(_mutex);
#endif
        return sent;
    }

    size_t read(std::span<uint8_t> data, uint32_t timeoutMs = 0xFFFFFFFF) override {
        size_t received = 0;
        while (received < data.size()) {
            uint8_t byte;
            if (_rxBuf.pop(byte)) {
                data[received++] = byte;
            } else {
#ifdef STM32_USE_FREERTOS
                if (!xSemaphoreTake(_rxSem, pdMS_TO_TICKS(timeoutMs))) break;
#else
                (void)timeoutMs;
                if (received > 0) break;
                while (_rxBuf.empty()) {}
#endif
            }
        }
        return received;
    }

    size_t writeNonBlocking(std::span<const uint8_t> data) override {
        size_t written = _txBuf.write(data.data(), data.size());
        if (written > 0) _periph->CR1 |= USART_CR1_TXEIE;
        return written;
    }

    size_t readNonBlocking(std::span<uint8_t> data) override {
        return _rxBuf.read(data.data(), data.size());
    }

    size_t rxAvailable() const override { return _rxBuf.size(); }
    size_t txFree() const override { return _txBuf.free_space(); }

    void irqHandler() override {
        uint32_t sr = _periph->SR;

        if (sr & USART_SR_RXNE) {
            uint8_t byte = static_cast<uint8_t>(_periph->DR);
            _rxBuf.push(byte);
#ifdef STM32_USE_FREERTOS
            BaseType_t woken = pdFALSE;
            if (_rxSem) xSemaphoreGiveFromISR(_rxSem, &woken);
            portYIELD_FROM_ISR(woken);
#endif
        }

        if (sr & USART_SR_TXE) {
            uint8_t byte;
            if (_txBuf.pop(byte)) {
                _periph->DR = byte;
            } else {
                _periph->CR1 &= ~USART_CR1_TXEIE;
#ifdef STM32_USE_FREERTOS
                BaseType_t woken = pdFALSE;
                if (_txSem) xSemaphoreGiveFromISR(_txSem, &woken);
                portYIELD_FROM_ISR(woken);
#endif
            }
        }

        if (sr & (USART_SR_ORE | USART_SR_NE | USART_SR_FE | USART_SR_PE)) {
            volatile uint32_t dummy = _periph->DR;
            (void)dummy;
        }
    }
};

} // namespace stm32f4
} // namespace driver
