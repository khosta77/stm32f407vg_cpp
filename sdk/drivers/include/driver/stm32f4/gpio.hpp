#pragma once

#include "driver/gpio.hpp"
#include "cmsis/stm32f4xx.h"

namespace driver {
namespace stm32f4 {

class Gpio : public IGpio {
    GPIO_TypeDef* const _port;

public:
    explicit Gpio(GPIO_TypeDef* port) : _port(port) {}

    void configure(uint8_t pin, const GpioConfig& cfg) override {
        uint32_t pos2 = pin * 2;

        _port->MODER &= ~(0x3U << pos2);
        _port->MODER |= (static_cast<uint32_t>(cfg.mode) << pos2);

        _port->OSPEEDR &= ~(0x3U << pos2);
        _port->OSPEEDR |= (static_cast<uint32_t>(cfg.speed) << pos2);

        _port->PUPDR &= ~(0x3U << pos2);
        _port->PUPDR |= (static_cast<uint32_t>(cfg.pull) << pos2);

        if (cfg.type == OutputType::OpenDrain) {
            _port->OTYPER |= (1U << pin);
        } else {
            _port->OTYPER &= ~(1U << pin);
        }

        if (cfg.mode == PinMode::AlternateFunction) {
            uint32_t afIdx = (pin < 8) ? 0 : 1;
            uint32_t afPos = (pin & 0x7) * 4;
            _port->AFR[afIdx] &= ~(0xFU << afPos);
            _port->AFR[afIdx] |= (static_cast<uint32_t>(cfg.af) << afPos);
        }
    }

    void set(uint8_t pin) override {
        _port->BSRR = (1U << pin);
    }

    void reset(uint8_t pin) override {
        _port->BSRR = (1U << (pin + 16));
    }

    void toggle(uint8_t pin) override {
        _port->ODR ^= (1U << pin);
    }

    bool read(uint8_t pin) override {
        return (_port->IDR & (1U << pin)) != 0;
    }
};

} // namespace stm32f4
} // namespace driver
