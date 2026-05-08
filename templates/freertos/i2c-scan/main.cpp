#include "cmsis/stm32f4xx.h"
#include "rtos/rtos.hpp"

import driver.types;
import driver.gpio;
import driver.reg;
import driver.uart;
import driver.stm32f4.gpio;
import driver.stm32f4.i2c;
import driver.stm32f4.uart;

extern "C" {
int snprintf(char *str, size_t size, const char *format, ...);
}

using driver::gpio;
using driver::GpioConfig;
using driver::OutputSpeed;
using driver::OutputType;
using driver::Parity;
using driver::PinMode;
using driver::PullMode;
using driver::stm32f4::GpioPin;
using driver::stm32f4::I2c;
using driver::stm32f4::Uart;

extern "C" void __initialize_hardware() {
    SystemCoreClockUpdate();
    driver::reg::set(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN);
    driver::reg::set(RCC->APB1ENR, RCC_APB1ENR_I2C1EN | RCC_APB1ENR_USART2EN);
    __DSB();
}

namespace {

GpioPin g_uartTx{
    *GPIOA,
    gpio({
        .pin = 2,
        .mode = PinMode::AlternateFunction,
        .pull = PullMode::None,
        .speed = OutputSpeed::VeryHigh,
        .type = OutputType::PushPull,
        .af = 7,
    }),
};
GpioPin g_uartRx{
    *GPIOA,
    gpio({
        .pin = 3,
        .mode = PinMode::AlternateFunction,
        .pull = PullMode::None,
        .speed = OutputSpeed::VeryHigh,
        .type = OutputType::PushPull,
        .af = 7,
    }),
};
GpioPin g_i2cScl{
    *GPIOB,
    gpio({
        .pin = 6,
        .mode = PinMode::AlternateFunction,
        .pull = PullMode::PullUp,
        .speed = OutputSpeed::VeryHigh,
        .type = OutputType::OpenDrain,
        .af = 4,
    }),
};
GpioPin g_i2cSda{
    *GPIOB,
    gpio({
        .pin = 7,
        .mode = PinMode::AlternateFunction,
        .pull = PullMode::PullUp,
        .speed = OutputSpeed::VeryHigh,
        .type = OutputType::OpenDrain,
        .af = 4,
    }),
};

I2c g_i2c1{
    *I2C1,
    {
        .clockSpeed = 100000,
        .fastMode = false,
    },
};
Uart<> g_uart2{
    *USART2,
    USART2_IRQn,
    {
        .baudrate = 115200,
        .dataBits = 8,
        .stopBits = 1,
        .parity = Parity::None,
    },
};

void writeStr(const char *s) {
    size_t n = 0;
    while (s[n] != '\0') {
        ++n;
    }
    g_uart2.write({reinterpret_cast<const uint8_t *>(s), n});
}

void taskScan(void *) {
    rtos::Task::delay(pdMS_TO_TICKS(200));
    while (true) {
        writeStr("\r\n=== I2C1 scan (PB6/PB7 @ 100 kHz) ===\r\n");
        char buf[80];
        size_t found = 0;
        for (uint8_t addr = 0x03; addr <= 0x77; ++addr) {
            auto st = g_i2c1.probe(addr);
            if (st == driver::Status::Ok) {
                int len = snprintf(
                    buf,
                    sizeof(buf),
                    "  found device at 7-bit 0x%02X (8-bit write 0x%02X / read 0x%02X)\r\n",
                    addr,
                    static_cast<unsigned>(addr << 1),
                    static_cast<unsigned>((addr << 1) | 1));
                if (len > 0) {
                    g_uart2.write({reinterpret_cast<const uint8_t *>(buf), static_cast<size_t>(len)});
                }
                ++found;
            }
        }
        int len = snprintf(buf, sizeof(buf), "scan complete: %u device(s)\r\n", static_cast<unsigned>(found));
        if (len > 0) {
            g_uart2.write({reinterpret_cast<const uint8_t *>(buf), static_cast<size_t>(len)});
        }
        rtos::Task::delay(pdMS_TO_TICKS(2000));
    }
}

}  // namespace

extern "C" void USART2_IRQHandler() {
    g_uart2.irqHandler();
}

int main() {
    static rtos::Task scan("scan", 384, 1, taskScan);

    rtos::Task::startScheduler();

    while (true) {
    }
}
