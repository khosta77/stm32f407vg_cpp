#include "cmsis/stm32f4xx.h"
#include "rtos/rtos.hpp"

import driver.gpio;
import driver.stm32f4.gpio;

using driver::GpioConfig;
using driver::OutputSpeed;
using driver::OutputType;
using driver::PinMode;
using driver::PullMode;
using driver::stm32f4::GpioPin;

namespace {

GpioPin g_ledGreen{*GPIOD, {12, PinMode::Output, PullMode::None, OutputSpeed::Low, OutputType::PushPull}};
GpioPin g_ledRed{*GPIOD, {14, PinMode::Output, PullMode::None, OutputSpeed::Low, OutputType::PushPull}};
GpioPin g_ledBlue{*GPIOD, {15, PinMode::Output, PullMode::None, OutputSpeed::Low, OutputType::PushPull}};

void taskGreen(void *) {
    while (true) {
        g_ledGreen.toggle();
        rtos::Task::delay(pdMS_TO_TICKS(500));
    }
}

void taskRedBlue(void *) {
    while (true) {
        g_ledRed.set();
        rtos::Task::delay(pdMS_TO_TICKS(250));
        g_ledRed.reset();
        g_ledBlue.set();
        rtos::Task::delay(pdMS_TO_TICKS(250));
        g_ledBlue.reset();
    }
}

}  // namespace

int main() {
    static rtos::Task green("green", 128, 1, taskGreen);
    static rtos::Task red("red", 128, 1, taskRedBlue);

    rtos::Task::startScheduler();

    while (true) {
    }
}
