#pragma once

#include <cstdint>
#include <span>

import sensor.imu;
import driver.i2c;
import driver.types;

namespace sensor
{

class Mpu6050 : public IImu
{
    driver::II2c &_i2c;
    uint8_t _addr;
    float _accelScale = 16384.0f;
    float _gyroScale = 131.0f;

    static constexpr uint8_t REG_SMPLRT_DIV = 0x19;
    static constexpr uint8_t REG_CONFIG = 0x1A;
    static constexpr uint8_t REG_GYRO_CONFIG = 0x1B;
    static constexpr uint8_t REG_ACCEL_CONFIG = 0x1C;
    static constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;
    static constexpr uint8_t REG_GYRO_XOUT_H = 0x43;
    static constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;
    static constexpr uint8_t REG_WHO_AM_I = 0x75;

    driver::Status writeReg( uint8_t reg, uint8_t val )
    {
        return _i2c.writeReg( _addr, reg, std::span<const uint8_t>( &val, 1 ) );
    }

    driver::Status readRegs( uint8_t reg, uint8_t *buf, size_t len )
    {
        return _i2c.readReg( _addr, reg, std::span<uint8_t>( buf, len ) );
    }

    static int16_t combine( uint8_t hi, uint8_t lo )
    {
        return static_cast<int16_t>( ( static_cast<uint16_t>( hi ) << 8 ) | lo );
    }

public:
    explicit Mpu6050( driver::II2c &i2c, uint8_t addr = 0x68 ) : _i2c( i2c ), _addr( addr ) {}

    driver::Status init() override
    {
        driver::Status st;

        st = writeReg( REG_PWR_MGMT_1, 0x80 );
        if ( st != driver::Status::Ok )
        {
            return st;
        }

        for ( uint32_t i = 0; i < 100000; ++i )
        {
            __asm volatile( "nop" );
        }

        st = writeReg( REG_PWR_MGMT_1, 0x01 );
        if ( st != driver::Status::Ok )
        {
            return st;
        }

        st = writeReg( REG_SMPLRT_DIV, 0x07 );
        if ( st != driver::Status::Ok )
        {
            return st;
        }

        st = writeReg( REG_CONFIG, 0x06 );
        if ( st != driver::Status::Ok )
        {
            return st;
        }

        setAccelRange( 2 );
        setGyroRange( 250 );

        uint8_t whoami = 0;
        st = readRegs( REG_WHO_AM_I, &whoami, 1 );
        if ( st != driver::Status::Ok )
        {
            return st;
        }

        return ( whoami == 0x68 ) ? driver::Status::Ok : driver::Status::HardwareError;
    }

    driver::Status read( ImuData &out ) override
    {
        uint8_t buf[14];
        driver::Status st = readRegs( REG_ACCEL_XOUT_H, buf, 14 );
        if ( st != driver::Status::Ok )
        {
            return st;
        }

        out.accel.x = static_cast<float>( combine( buf[0], buf[1] ) ) / _accelScale * 9.80665f;
        out.accel.y = static_cast<float>( combine( buf[2], buf[3] ) ) / _accelScale * 9.80665f;
        out.accel.z = static_cast<float>( combine( buf[4], buf[5] ) ) / _accelScale * 9.80665f;

        int16_t rawTemp = combine( buf[6], buf[7] );
        out.temp = static_cast<float>( rawTemp ) / 340.0f + 36.53f;

        out.gyro.x = static_cast<float>( combine( buf[8], buf[9] ) ) / _gyroScale;
        out.gyro.y = static_cast<float>( combine( buf[10], buf[11] ) ) / _gyroScale;
        out.gyro.z = static_cast<float>( combine( buf[12], buf[13] ) ) / _gyroScale;

        return driver::Status::Ok;
    }

    driver::Status selfTest() override
    {
        uint8_t whoami = 0;
        driver::Status st = readRegs( REG_WHO_AM_I, &whoami, 1 );
        if ( st != driver::Status::Ok )
        {
            return st;
        }
        return ( whoami == 0x68 ) ? driver::Status::Ok : driver::Status::HardwareError;
    }

    void setAccelRange( uint8_t g ) override
    {
        uint8_t val;
        switch ( g )
        {
            case 4:
                val = 0x08;
                _accelScale = 8192.0f;
                break;
            case 8:
                val = 0x10;
                _accelScale = 4096.0f;
                break;
            case 16:
                val = 0x18;
                _accelScale = 2048.0f;
                break;
            default:
                val = 0x00;
                _accelScale = 16384.0f;
                break;
        }
        writeReg( REG_ACCEL_CONFIG, val );
    }

    void setGyroRange( uint16_t dps ) override
    {
        uint8_t val;
        switch ( dps )
        {
            case 500:
                val = 0x08;
                _gyroScale = 65.5f;
                break;
            case 1000:
                val = 0x10;
                _gyroScale = 32.8f;
                break;
            case 2000:
                val = 0x18;
                _gyroScale = 16.4f;
                break;
            default:
                val = 0x00;
                _gyroScale = 131.0f;
                break;
        }
        writeReg( REG_GYRO_CONFIG, val );
    }
};

} // namespace sensor
