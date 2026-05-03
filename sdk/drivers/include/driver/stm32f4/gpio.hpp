#pragma once

#include "cmsis/stm32f4xx.h"

import driver.gpio;
import driver.types;
import driver.reg;

namespace driver
{
namespace stm32f4
{

class GpioPin : public IGpioPin
{
    GPIO_TypeDef &_port;
    uint8_t _pin;

public:
    GpioPin( GPIO_TypeDef &port, const GpioConfig &cfg )
        : _port( port ), _pin( cfg.pin )
    {
        uint32_t pos2 = _pin * 2U;
        reg::modify( _port.MODER, 0x3U << pos2,
                     static_cast<uint32_t>( cfg.mode ) << pos2 );
        reg::modify( _port.PUPDR, 0x3U << pos2,
                     static_cast<uint32_t>( cfg.pull ) << pos2 );

        uint32_t speedVal = ( cfg.speed != OutputSpeed::None )
                                ? static_cast<uint32_t>( cfg.speed )
                                : 0U;
        reg::modify( _port.OSPEEDR, 0x3U << pos2, speedVal << pos2 );

        if ( cfg.type == OutputType::OpenDrain )
        {
            reg::set( _port.OTYPER, 1U << _pin );
        }
        else
        {
            reg::clear( _port.OTYPER, 1U << _pin );
        }

        if ( cfg.mode == PinMode::AlternateFunction )
        {
            uint32_t afIdx = ( _pin < 8 ) ? 0U : 1U;
            uint32_t afPos = ( _pin & 0x7U ) * 4U;
            reg::modify( _port.AFR[afIdx], 0xFU << afPos,
                         static_cast<uint32_t>( cfg.af ) << afPos );
        }
    }

    void set() override { _port.BSRR = ( 1U << _pin ); }

    void reset() override { _port.BSRR = ( 1U << ( _pin + 16U ) ); }

    void toggle() override
    {
        if ( reg::read( _port.ODR, 1U << _pin ) )
        {
            _port.BSRR = ( 1U << ( _pin + 16U ) );
        }
        else
        {
            _port.BSRR = ( 1U << _pin );
        }
    }

    Status read() override
    {
        return reg::read( _port.IDR, 1U << _pin ) ? Status::Ok : Status::None;
    }
};

} // namespace stm32f4
} // namespace driver
