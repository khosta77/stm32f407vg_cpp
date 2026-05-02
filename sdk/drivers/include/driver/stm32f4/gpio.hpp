#pragma once

#include "cmsis/stm32f4xx.h"
#include "driver/gpio.hpp"
#include "driver/reg.hpp"

namespace driver
{
namespace stm32f4
{

class Gpio : public IGpio
{
    GPIO_TypeDef &_port;

public:
    explicit Gpio( GPIO_TypeDef &port ) : _port( port ) {}

    void configure( uint8_t pin, const GpioConfig &cfg ) override
    {
        uint32_t pos2 = pin * 2U;
        reg::modify( _port.MODER, 0x3U << pos2, static_cast<uint32_t>( cfg.mode ) << pos2 );
        reg::modify( _port.OSPEEDR, 0x3U << pos2, static_cast<uint32_t>( cfg.speed ) << pos2 );
        reg::modify( _port.PUPDR, 0x3U << pos2, static_cast<uint32_t>( cfg.pull ) << pos2 );

        if ( cfg.type == OutputType::OpenDrain )
        {
            reg::set( _port.OTYPER, 1U << pin );
        }
        else
        {
            reg::clear( _port.OTYPER, 1U << pin );
        }

        if ( cfg.mode == PinMode::AlternateFunction )
        {
            uint32_t afIdx = ( pin < 8 ) ? 0U : 1U;
            uint32_t afPos = ( pin & 0x7U ) * 4U;
            reg::modify( _port.AFR[afIdx], 0xFU << afPos, static_cast<uint32_t>( cfg.af ) << afPos );
        }
    }

    void set( uint8_t pin ) override { _port.BSRR = ( 1U << pin ); }

    void reset( uint8_t pin ) override { _port.BSRR = ( 1U << ( pin + 16U ) ); }

    void toggle( uint8_t pin ) override
    {
        if ( reg::read( _port.ODR, 1U << pin ) )
        {
            _port.BSRR = ( 1U << ( pin + 16U ) );
        }
        else
        {
            _port.BSRR = ( 1U << pin );
        }
    }

    Status read( uint8_t pin, bool &value ) override
    {
        value = reg::read( _port.IDR, 1U << pin );
        return Status::Ok;
    }
};

} // namespace stm32f4
} // namespace driver
