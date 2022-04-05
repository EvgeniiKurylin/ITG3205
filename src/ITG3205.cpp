#include "ITG3205.h"
#include <cstdint>
#include <cstdio>

uint8_t constexpr ITG3205::REGISTER_WHO_AM_I        ;
uint8_t constexpr ITG3205::REGISTER_SMPLRT_DIV      ;
uint8_t constexpr ITG3205::REGISTER_DLPF_FS         ;
uint8_t constexpr ITG3205::REGISTER_INT_CFG         ;
uint8_t constexpr ITG3205::REGISTER_INT_STATUS      ;
uint8_t constexpr ITG3205::REGISTER_TEMP_OUT_H      ;
uint8_t constexpr ITG3205::REGISTER_TEMP_OUT_L      ;
uint8_t constexpr ITG3205::REGISTER_GYRO_XOUT_H     ;
uint8_t constexpr ITG3205::REGISTER_GYRO_XOUT_L     ;
uint8_t constexpr ITG3205::REGISTER_GYRO_YOUT_H     ;
uint8_t constexpr ITG3205::REGISTER_GYRO_YOUT_L     ;
uint8_t constexpr ITG3205::REGISTER_GYRO_ZOUT_H     ;
uint8_t constexpr ITG3205::REGISTER_GYRO_ZOUT_L     ;
uint8_t constexpr ITG3205::REGISTER_PWR_MGM         ;

uint8_t constexpr ITG3205::WHO_AM_I_CODE            ;



/* Constructors */
ITG3205::ITG3205(I2C * i2c, uint8_t addr)
    : _i2c(i2c)
    , _address(addr)
{
    _resolutionGyro = 1.0/14.375;
    _resolutionTemp = 1.0/280;
}

/* Public functions */
bool ITG3205::init(){
    return init(WHO_AM_I_CODE);
}

void ITG3205::calibrate(){
    _gyroOffset = xyzFloat(0,0,0);
    int N = 220;
    int n = 0;

    xyzFloat mesSum(0,0,0);
    
    wait_us(1e6);
    for (int i=0; i<N; ++i) {
        if (i > 20) {
            // Measurement
            mesSum += getGyro();
            ++n;
        }
        wait_us(50e3);
    }

    xyzFloat result = mesSum / n;
    printf("offsetX: %.2f\toffsetY: %.2f\toffsetZ: %.2f\r\n", result.x, result.y, result.z);

    _gyroOffset = result;
}

xyzFloat ITG3205::getGyro(){
    return readGyro();
}

float ITG3205::getTemperature(){
    return readTemperature();
}

/* Protected functions */
bool ITG3205::init(uint8_t expectedID){
    // Reset device: Register 62 – Power Management -> 0x80
    writeITG3205Register(REGISTER_PWR_MGM, 0x80);
    // Wait 50 ms for device start-up
    wait_us(50e3);

    // Device ID verification
    if (whoAmI(REGISTER_WHO_AM_I) != expectedID){
        return false;
    }
    // Set Power management register:
    // No reset, no sleep           -> 0b00******
    // All axis into normal mode    -> 0b**000***
    // PLL with X Gyro reference    -> 0b*****001
    // Result -> 0x01
    writeITG3205Register(REGISTER_PWR_MGM, 0x01);
    // Set full scale range to +/-2000[°/sec]: FS_SEL -> 3 -> 0b***11***
    // Set digital low pass filter (BW=98Hz, SR=1kHz): DLPF_CFG -> 1 -> 0b*****010
    writeITG3205Register(REGISTER_DLPF_FS, 0x1A);
    // Set Sample Rate Divider
    // Fsample = Finternal / (divider + 1)
    // Finternal: 1kHz or 8kHz
    // devider = 4 -> 0x04
    // Fsample = 1kHz / (4 + 1) = 200Hz -> 5 ms/sample
    writeITG3205Register(REGISTER_SMPLRT_DIV, 0x04);
    

    return true;
}

uint8_t ITG3205::whoAmI(uint8_t ID){
    return readITG3205Register8(ID);
}

void ITG3205::writeITG3205Register(uint8_t reg, uint8_t val){
    char msg[2] = {reg, val};
    _i2c->write(ITG3205_WRITE_ADDRESS, msg, 2, false);
    wait_us(5e2);
}

uint8_t ITG3205::readITG3205Register8(uint8_t reg){
    char msg[1] = {reg};
    _i2c->write(ITG3205_WRITE_ADDRESS, msg, 1, false);

    char recievedData[1];
    _i2c->read(ITG3205_READ_ADDRESS, recievedData, 1, false);
    // printf("0x%2X\r\n", recievedData[0]);

    return recievedData[0];
}

/* Private functions */
xyzFloat ITG3205::readGyro(){
    xyzFloat gyro;

    char request[1] = {REGISTER_GYRO_XOUT_H};
    _i2c->write(ITG3205_WRITE_ADDRESS, request, 1, false);

    char recievedData[6];
    _i2c->read(ITG3205_READ_ADDRESS, recievedData, 6, false);

    gyro.x = (int16_t)((recievedData[0] << 8) | recievedData[1]) * _resolutionGyro;
    gyro.y = (int16_t)((recievedData[2] << 8) | recievedData[3]) * _resolutionGyro;
    gyro.z = (int16_t)((recievedData[4] << 8) | recievedData[5]) * _resolutionGyro;

    gyro -= _gyroOffset;

    return gyro;
}

float ITG3205::readTemperature(){
    float temp;

    char request[1] = {REGISTER_TEMP_OUT_H};
    _i2c->write(ITG3205_WRITE_ADDRESS, request, 1, false);

    char recievedData[2];
    _i2c->read(ITG3205_READ_ADDRESS, recievedData, 2, false);
    

    temp = 35 + ((int16_t)((recievedData[0] << 8) | recievedData[1]) + 13200) * _resolutionTemp;

    return temp;
}

/* Other functions */
void printByte(char byte) {
  printf("0b");
  for (int i = 7; i >= 0; --i)
    printf("%d", (byte >> i) & 1);
}