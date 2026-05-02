#pragma once

#include "cmsis/stm32f4xx.h"
#include "driver/stm32f4/clock.hpp"
#include <span>

import driver.types;
import driver.i2c;
import driver.reg;

#ifdef STM32_USE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

namespace driver
{
namespace stm32f4
{

class I2c : public II2c
{
public:
    struct Config
    {
        uint32_t clockSpeed = 100000;
        bool fastMode = false;
    };

private:
    I2C_TypeDef &_periph;
    Config _cfg;

#ifdef STM32_USE_FREERTOS
    SemaphoreHandle_t _mutex = nullptr;
#endif

    static constexpr uint32_t TIMEOUT_LOOPS = 100000;

    bool waitFlag( volatile uint32_t &reg, uint32_t flag, bool set ) const
    {
        for ( uint32_t i = 0; i < TIMEOUT_LOOPS; ++i )
        {
            if ( set )
            {
                if ( reg & flag )
                {
                    return true;
                }
            }
            else
            {
                if ( !( reg & flag ) )
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool waitBusy() const { return waitFlag( _periph.SR2, I2C_SR2_BUSY, false ); }

    void generateStart() const
    {
        reg::set( _periph.CR1, I2C_CR1_START );
        waitFlag( _periph.SR1, I2C_SR1_SB, true );
    }

    void generateStop() const { reg::set( _periph.CR1, I2C_CR1_STOP ); }

    bool sendAddress( uint8_t addr, bool readOp ) const
    {
        _periph.DR = ( addr << 1 ) | ( readOp ? 1 : 0 );
        if ( !waitFlag( _periph.SR1, I2C_SR1_ADDR, true ) )
        {
            if ( reg::read( _periph.SR1, I2C_SR1_AF ) )
            {
                reg::clear( _periph.SR1, I2C_SR1_AF );
                generateStop();
            }
            return false;
        }
        volatile uint32_t dummy = _periph.SR2;
        (void) dummy;
        return true;
    }

    void busRecovery()
    {
        reg::set( _periph.CR1, I2C_CR1_SWRST );
        reg::clear( _periph.CR1, I2C_CR1_SWRST );
        reinit();
    }

    void reinit()
    {
        uint32_t pclk = getApb1Clock();
        uint32_t freqMhz = pclk / 1000000;
        _periph.CR2 = freqMhz & I2C_CR2_FREQ;

        if ( _cfg.fastMode || _cfg.clockSpeed > 100000 )
        {
            _periph.CCR = I2C_CCR_FS | ( pclk / ( _cfg.clockSpeed * 3 ) );
            _periph.TRISE = ( freqMhz * 300 / 1000 ) + 1;
        }
        else
        {
            _periph.CCR = pclk / ( _cfg.clockSpeed * 2 );
            _periph.TRISE = freqMhz + 1;
        }

        reg::write( _periph.CR1, I2C_CR1_PE );
    }

    void lockBus()
    {
#ifdef STM32_USE_FREERTOS
        xSemaphoreTake( _mutex, portMAX_DELAY );
#endif
    }

    void unlockBus()
    {
#ifdef STM32_USE_FREERTOS
        xSemaphoreGive( _mutex );
#endif
    }

public:
    I2c( I2C_TypeDef &periph, const Config &cfg ) : _periph( periph ), _cfg( cfg )
    {
        reg::write( _periph.CR1, I2C_CR1_SWRST );
        reg::write( _periph.CR1, 0 );
        reinit();

#ifdef STM32_USE_FREERTOS
        _mutex = xSemaphoreCreateMutex();
        configASSERT( _mutex );
#endif
    }

    ~I2c()
    {
        reg::write( _periph.CR1, 0 );

#ifdef STM32_USE_FREERTOS
        if ( _mutex )
        {
            vSemaphoreDelete( _mutex );
        }
#endif
    }

    I2c( const I2c & ) = delete;
    I2c &operator=( const I2c & ) = delete;

    Status write( uint8_t addr, std::span<const uint8_t> data ) override
    {
        lockBus();

        Status result = Status::BusError;
        if ( waitBusy() )
        {
            generateStart();
            if ( sendAddress( addr, false ) )
            {
                result = Status::Ok;
                for ( auto byte : data )
                {
                    if ( !waitFlag( _periph.SR1, I2C_SR1_TXE, true ) )
                    {
                        result = Status::Timeout;
                        break;
                    }
                    _periph.DR = byte;
                }
                if ( result == Status::Ok )
                {
                    waitFlag( _periph.SR1, I2C_SR1_BTF, true );
                }
                generateStop();
            }
            else
            {
                result = Status::Nack;
            }
        }

        if ( result != Status::Ok )
        {
            busRecovery();
        }
        unlockBus();
        return result;
    }

    Status read( uint8_t addr, std::span<uint8_t> data ) override
    {
        lockBus();

        Status result = Status::BusError;
        if ( waitBusy() )
        {
            if ( data.size() == 1 )
            {
                reg::clear( _periph.CR1, I2C_CR1_ACK );
            }
            else
            {
                reg::set( _periph.CR1, I2C_CR1_ACK );
            }

            generateStart();
            if ( sendAddress( addr, true ) )
            {
                result = Status::Ok;
                for ( size_t i = 0, I = data.size(); i < I; ++i )
                {
                    if ( i == I - 1 )
                    {
                        reg::clear( _periph.CR1, I2C_CR1_ACK );
                        generateStop();
                    }
                    if ( !waitFlag( _periph.SR1, I2C_SR1_RXNE, true ) )
                    {
                        result = Status::Timeout;
                        break;
                    }
                    data[i] = static_cast<uint8_t>( _periph.DR );
                }
            }
            else
            {
                result = Status::Nack;
            }
        }

        if ( result != Status::Ok )
        {
            generateStop();
            busRecovery();
        }
        unlockBus();
        return result;
    }

    Status writeReg( uint8_t addr, uint8_t reg, std::span<const uint8_t> data ) override
    {
        lockBus();

        Status result = Status::BusError;
        if ( waitBusy() )
        {
            generateStart();
            if ( sendAddress( addr, false ) )
            {
                if ( waitFlag( _periph.SR1, I2C_SR1_TXE, true ) )
                {
                    _periph.DR = reg;
                    result = Status::Ok;
                    for ( auto byte : data )
                    {
                        if ( !waitFlag( _periph.SR1, I2C_SR1_TXE, true ) )
                        {
                            result = Status::Timeout;
                            break;
                        }
                        _periph.DR = byte;
                    }
                    if ( result == Status::Ok )
                    {
                        waitFlag( _periph.SR1, I2C_SR1_BTF, true );
                    }
                }
                else
                {
                    result = Status::Timeout;
                }
                generateStop();
            }
            else
            {
                result = Status::Nack;
            }
        }

        if ( result != Status::Ok )
        {
            busRecovery();
        }
        unlockBus();
        return result;
    }

    Status readReg( uint8_t addr, uint8_t reg, std::span<uint8_t> data ) override
    {
        lockBus();

        Status result = Status::BusError;
        if ( waitBusy() )
        {
            generateStart();
            if ( sendAddress( addr, false ) )
            {
                if ( waitFlag( _periph.SR1, I2C_SR1_TXE, true ) )
                {
                    _periph.DR = reg;
                    waitFlag( _periph.SR1, I2C_SR1_BTF, true );

                    if ( data.size() == 1 )
                    {
                        reg::clear( _periph.CR1, I2C_CR1_ACK );
                    }
                    else
                    {
                        reg::set( _periph.CR1, I2C_CR1_ACK );
                    }

                    generateStart();
                    if ( sendAddress( addr, true ) )
                    {
                        result = Status::Ok;
                        for ( size_t i = 0, I = data.size(); i < I; ++i )
                        {
                            if ( i == I - 1 )
                            {
                                reg::clear( _periph.CR1, I2C_CR1_ACK );
                                generateStop();
                            }
                            if ( !waitFlag( _periph.SR1, I2C_SR1_RXNE, true ) )
                            {
                                result = Status::Timeout;
                                break;
                            }
                            data[i] = static_cast<uint8_t>( _periph.DR );
                        }
                    }
                    else
                    {
                        result = Status::Nack;
                    }
                }
                else
                {
                    result = Status::Timeout;
                }
            }
            else
            {
                result = Status::Nack;
            }
        }

        if ( result != Status::Ok )
        {
            generateStop();
            busRecovery();
        }
        unlockBus();
        return result;
    }

    Status probe( uint8_t addr ) override
    {
        lockBus();

        Status result = Status::Nack;
        if ( waitBusy() )
        {
            generateStart();
            if ( sendAddress( addr, false ) )
            {
                result = Status::Ok;
            }
            generateStop();
        }
        else
        {
            result = Status::Busy;
        }

        unlockBus();
        return result;
    }
};

} // namespace stm32f4
} // namespace driver
