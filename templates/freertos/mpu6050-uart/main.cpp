#include "cmsis/stm32f4xx.h"
#include "rtos/rtos.hpp"
#include "sensor/cached_sensor.hpp"
#include "mpu6050.hpp"
#include "driver/stm32f4/uart.hpp"
#include "driver/stm32f4/i2c.hpp"

#include <cstdio>
#include <cstring>
#include <span>

namespace {

driver::stm32f4::I2c g_i2c1(I2C1);
sensor::Mpu6050 g_mpu(g_i2c1);
driver::stm32f4::Uart g_uart2(USART2, USART2_IRQn);

bool readImu(sensor::ImuData& out, void* ctx) {
    auto* mpu = static_cast<sensor::Mpu6050*>(ctx);
    return mpu->read(out);
}

void taskView(void* param) {
    auto* cached = static_cast<sensor::CachedSensor<sensor::ImuData>*>(param);
    char buf[128];
    for (;;) {
        auto data = cached->get();
        int len = snprintf(buf, sizeof(buf),
            "A: %.2f %.2f %.2f G: %.1f %.1f %.1f T: %.1f\r\n",
            static_cast<double>(data.accel.x),
            static_cast<double>(data.accel.y),
            static_cast<double>(data.accel.z),
            static_cast<double>(data.gyro.x),
            static_cast<double>(data.gyro.y),
            static_cast<double>(data.gyro.z),
            static_cast<double>(data.temp));
        if (len > 0) {
            g_uart2.write(std::span<const uint8_t>(
                reinterpret_cast<const uint8_t*>(buf), static_cast<size_t>(len)));
        }
        rtos::Task::delay(pdMS_TO_TICKS(100));
    }
}

} // namespace

extern "C" void USART2_IRQHandler() {
    g_uart2.irqHandler();
}

int main() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN | RCC_APB1ENR_USART2EN;
    __DSB();

    GPIOA->MODER |= GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1;
    GPIOA->AFR[0] |= (7U << GPIO_AFRL_AFSEL2_Pos) | (7U << GPIO_AFRL_AFSEL3_Pos);

    GPIOB->MODER |= GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1;
    GPIOB->OTYPER |= GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7;
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0;
    GPIOB->AFR[0] |= (4U << GPIO_AFRL_AFSEL6_Pos) | (4U << GPIO_AFRL_AFSEL7_Pos);

    driver::UartConfig uartCfg;
    uartCfg.baudrate = 115200;
    g_uart2.init(uartCfg);

    driver::I2cConfig i2cCfg;
    i2cCfg.clockSpeed = 400000;
    i2cCfg.fastMode = true;
    g_i2c1.init(i2cCfg);

    g_mpu.init();

    sensor::CachedSensor<sensor::ImuData> cachedImu(
        10, readImu, &g_mpu, "imu", 256, 2
    );

    xTaskCreate(taskView, "view", 512, &cachedImu, 1, nullptr);
    vTaskStartScheduler();

    for (;;) {}
}
