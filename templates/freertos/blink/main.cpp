#include "cmsis/stm32f4xx.h"
#include "rtos/rtos.hpp"

constexpr uint32_t LED_GREEN  = GPIO_ODR_OD12;
constexpr uint32_t LED_ORANGE = GPIO_ODR_OD13;
constexpr uint32_t LED_RED    = GPIO_ODR_OD14;
constexpr uint32_t LED_BLUE   = GPIO_ODR_OD15;

static void taskGreenOrange(void*) {
    while (true) {
        GPIOD->BSRR = LED_GREEN;
        rtos::Task::delay(pdMS_TO_TICKS(500));
        GPIOD->BSRR = LED_GREEN << 16;
        rtos::Task::delay(pdMS_TO_TICKS(500));
    }
}

static void taskRedBlue(void*) {
    while (true) {
        GPIOD->BSRR = LED_RED;
        rtos::Task::delay(pdMS_TO_TICKS(250));
        GPIOD->BSRR = LED_RED << 16;
        GPIOD->BSRR = LED_BLUE;
        rtos::Task::delay(pdMS_TO_TICKS(250));
        GPIOD->BSRR = LED_BLUE << 16;
    }
}

int main() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    __DSB();

    GPIOD->MODER |= GPIO_MODER_MODER12_0
                   | GPIO_MODER_MODER13_0
                   | GPIO_MODER_MODER14_0
                   | GPIO_MODER_MODER15_0;

    xTaskCreate(taskGreenOrange, "green", 128, nullptr, 1, nullptr);
    xTaskCreate(taskRedBlue, "red", 128, nullptr, 1, nullptr);

    vTaskStartScheduler();

    for (;;) {}
}
