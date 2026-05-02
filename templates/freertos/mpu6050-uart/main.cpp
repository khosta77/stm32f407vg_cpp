#include "cmsis/stm32f4xx.h"
#include "driver/stm32f4/i2c.hpp"
#include "driver/stm32f4/uart.hpp"
#include "rtos/rtos.hpp"
#include "sensor/cached_sensor.hpp"

import sensor.imu;
import sensor.mpu6050;
import driver.types;

#include <cstddef>
#include <cstdint>
#include <span>

extern "C"
{
float atan2f( float y, float x );
int snprintf( char *str, size_t size, const char *format, ... );
}

constexpr uint32_t LED_GREEN = GPIO_ODR_OD12;
constexpr uint32_t LED_ORANGE = GPIO_ODR_OD13;
constexpr uint32_t LED_RED = GPIO_ODR_OD14;
constexpr uint32_t LED_BLUE = GPIO_ODR_OD15;
constexpr float TILT_THRESHOLD = 15.0f;
constexpr float RAD_TO_DEG = 180.0f / 3.14159265f;

namespace
{

driver::stm32f4::I2c::Config i2cCfg { .clockSpeed = 400000, .fastMode = true };
driver::stm32f4::Uart::Config uartCfg { .baudrate = 115200 };

driver::stm32f4::I2c g_i2c1( *I2C1, i2cCfg );
sensor::Mpu6050 g_mpu( g_i2c1 );
driver::stm32f4::Uart g_uart2( *USART2, USART2_IRQn, uartCfg );

driver::Status readImu( sensor::ImuData &out, void *ctx )
{
    auto *mpu = static_cast<sensor::Mpu6050 *>( ctx );
    return mpu->read( out );
}

void taskView( void *param )
{
    auto *cached = static_cast<sensor::CachedSensor<sensor::ImuData> *>( param );
    char buf[128];
    while ( true )
    {
        auto data = cached->get();
        int len = snprintf( buf, sizeof( buf ), "A: %.2f %.2f %.2f G: %.1f %.1f %.1f T: %.1f\r\n",
                            static_cast<double>( data.accel.x ), static_cast<double>( data.accel.y ),
                            static_cast<double>( data.accel.z ), static_cast<double>( data.gyro.x ),
                            static_cast<double>( data.gyro.y ), static_cast<double>( data.gyro.z ),
                            static_cast<double>( data.temp ) );
        if ( len > 0 )
        {
            g_uart2.write( std::span<const uint8_t>( reinterpret_cast<const uint8_t *>( buf ),
                                                     static_cast<size_t>( len ) ) );
        }
        rtos::Task::delay( pdMS_TO_TICKS( 100 ) );
    }
}

void taskLed( void *param )
{
    auto *cached = static_cast<sensor::CachedSensor<sensor::ImuData> *>( param );
    while ( true )
    {
        auto data = cached->get();
        float pitch = atan2f( data.accel.y, data.accel.z ) * RAD_TO_DEG;
        float roll = atan2f( data.accel.x, data.accel.z ) * RAD_TO_DEG;

        uint32_t resetMask = ( LED_GREEN | LED_ORANGE | LED_RED | LED_BLUE ) << 16;
        GPIOD->BSRR = resetMask;

        if ( pitch > TILT_THRESHOLD )
        {
            GPIOD->BSRR = LED_GREEN;
        }
        else if ( pitch < -TILT_THRESHOLD )
        {
            GPIOD->BSRR = LED_RED;
        }

        if ( roll > TILT_THRESHOLD )
        {
            GPIOD->BSRR = LED_ORANGE;
        }
        else if ( roll < -TILT_THRESHOLD )
        {
            GPIOD->BSRR = LED_BLUE;
        }

        rtos::Task::delay( pdMS_TO_TICKS( 50 ) );
    }
}

} // namespace

extern "C" void USART2_IRQHandler() { g_uart2.irqHandler(); }

int main()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN | RCC_APB1ENR_USART2EN;
    __DSB();

    GPIOA->MODER |= GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1;
    GPIOA->AFR[0] |= ( 7U << GPIO_AFRL_AFSEL2_Pos ) | ( 7U << GPIO_AFRL_AFSEL3_Pos );

    GPIOB->MODER |= GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1;
    GPIOB->OTYPER |= GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7;
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0;
    GPIOB->AFR[0] |= ( 4U << GPIO_AFRL_AFSEL6_Pos ) | ( 4U << GPIO_AFRL_AFSEL7_Pos );

    GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0;

    g_mpu.init();

    sensor::CachedSensor<sensor::ImuData> cachedImu( 10, readImu, &g_mpu, "imu", 256, 2 );

    rtos::Task view( "view", 512, 1, taskView, &cachedImu );
    rtos::Task led( "led", 256, 1, taskLed, &cachedImu );

    rtos::Task::startScheduler();

    while ( true )
    {
    }
}
