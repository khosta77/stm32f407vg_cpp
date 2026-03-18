#include "cmsis/stm32f4xx.h"

static volatile uint32_t g_ticks;

extern "C" void SysTick_Handler() {
    ++g_ticks;
}

static void delay(uint32_t ms) {
    uint32_t start = g_ticks;
    while (g_ticks - start < ms) {}
}

constexpr uint16_t LED_GREEN  = 1U << 12;
constexpr uint16_t LED_ORANGE = 1U << 13;
constexpr uint16_t LED_RED    = 1U << 14;
constexpr uint16_t LED_BLUE   = 1U << 15;
constexpr uint16_t LED_ALL    = LED_GREEN | LED_ORANGE | LED_RED | LED_BLUE;

int main() {
    SysTick_Config(SystemCoreClock / 1000);

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    __DSB();

    GPIOD->MODER |= (1U << 24) | (1U << 26) | (1U << 28) | (1U << 30);

    while (true) {
        GPIOD->BSRR = LED_GREEN;
        delay(125);
        GPIOD->BSRR = LED_ORANGE;
        delay(125);
        GPIOD->BSRR = LED_RED;
        delay(125);
        GPIOD->BSRR = LED_BLUE;
        delay(125);

        GPIOD->BSRR = static_cast<uint32_t>(LED_ALL) << 16;
        delay(500);
    }
}
