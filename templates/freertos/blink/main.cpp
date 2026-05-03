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

namespace
{

GpioPin *g_ledGreen;
GpioPin *g_ledRed;
GpioPin *g_ledBlue;

void taskGreen( void * )
{
    while ( true )
    {
        g_ledGreen->toggle();
        rtos::Task::delay( pdMS_TO_TICKS( 500 ) );
    }
}

void taskRedBlue( void * )
{
    while ( true )
    {
        g_ledRed->set();
        rtos::Task::delay( pdMS_TO_TICKS( 250 ) );
        g_ledRed->reset();
        g_ledBlue->set();
        rtos::Task::delay( pdMS_TO_TICKS( 250 ) );
        g_ledBlue->reset();
    }
}

} // namespace

int main()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    __DSB();

    static GpioPin ledGreen{ *GPIOD,
        { 12, PinMode::Output, PullMode::None, OutputSpeed::Low, OutputType::PushPull } };
    static GpioPin ledRed{ *GPIOD,
        { 14, PinMode::Output, PullMode::None, OutputSpeed::Low, OutputType::PushPull } };
    static GpioPin ledBlue{ *GPIOD,
        { 15, PinMode::Output, PullMode::None, OutputSpeed::Low, OutputType::PushPull } };

    g_ledGreen = &ledGreen;
    g_ledRed = &ledRed;
    g_ledBlue = &ledBlue;

    static rtos::Task green( "green", 128, 1, taskGreen );
    static rtos::Task red( "red", 128, 1, taskRedBlue );

    rtos::Task::startScheduler();

    while ( true )
    {
    }
}
