#ifndef ITG3205_H
#define ITG3205_H

#include "mbed.h"
#include "utilities.h"
#include <cstdint>
#include <stdint.h>

#define ITG3205_DEFAULT_ADDRESS     0xD0
#define ITG3205_WRITE_ADDRESS       0xD0
#define ITG3205_READ_ADDRESS        0xD1

class ITG3205{
    public:
        /* Registers */
        static uint8_t constexpr REGISTER_WHO_AM_I      = 0x00;     // R/W
        static uint8_t constexpr REGISTER_SMPLRT_DIV    = 0x15;     // R/W
        static uint8_t constexpr REGISTER_DLPF_FS       = 0x16;     // R/W
        static uint8_t constexpr REGISTER_INT_CFG       = 0x17;     // R/W
        static uint8_t constexpr REGISTER_INT_STATUS    = 0x1A;     // R
        static uint8_t constexpr REGISTER_TEMP_OUT_H    = 0x1B;     // R
        static uint8_t constexpr REGISTER_TEMP_OUT_L    = 0x1C;     // R
        static uint8_t constexpr REGISTER_GYRO_XOUT_H   = 0x1D;     // R
        static uint8_t constexpr REGISTER_GYRO_XOUT_L   = 0x1E;     // R
        static uint8_t constexpr REGISTER_GYRO_YOUT_H   = 0x1F;     // R
        static uint8_t constexpr REGISTER_GYRO_YOUT_L   = 0x20;     // R
        static uint8_t constexpr REGISTER_GYRO_ZOUT_H   = 0x21;     // R
        static uint8_t constexpr REGISTER_GYRO_ZOUT_L   = 0x22;     // R
        static uint8_t constexpr REGISTER_PWR_MGM       = 0x3E;     // R/W

        /* Register values */
        static uint8_t constexpr WHO_AM_I_CODE          = 0x69;
        static uint8_t constexpr DEVICE_RESET_VALUE     = 0x80;

        /* Constructors */
        ITG3205(I2C * i2c, uint8_t addr = ITG3205_DEFAULT_ADDRESS);

        /* Methods */
        bool init();
        void calibrate();

        xyzFloat getGyro();
        float getTemperature();

        void setOffset(xyzFloat offset){ _gyroOffset = offset; }
        void setOffset(float x, float y, float z){ setOffset(xyzFloat(x,y,z)); }

    protected:
        bool init(uint8_t expectedID);
        uint8_t whoAmI(uint8_t ID);

        void writeITG3205Register(uint8_t reg, uint8_t val);
        uint8_t readITG3205Register8(uint8_t reg);

    private:
        I2C * _i2c;
        uint8_t _address;

        xyzFloat _gyroOffset;
        float   _resolutionGyro, _resolutionTemp;

        xyzFloat readGyro();
        float readTemperature();
};

#endif