#include "mbed.h"
#include "ITG3205.h"
#include <cstdint>
#include <cstdio>

/* 
ADXL345 -  0xA6 — Three axis acceleration 
ITG3205  - 0xD0 — Three axis gyroscope
HMC5883L - 0x1E — Three axis magnetic field
*/

Serial  pc(USBTX, USBRX, 115200);   // TX,  RX, baudrate
I2C     i2c(PA_10, PA_9);           // SDA, SCL

ITG3205 itg3205_dev(&i2c);

// main() runs in its own thread in the OS
int main()
{
    // printf("Start\r\n");
    if (itg3205_dev.init()) {
        printf("\nITG3205 is detected\r\n");
        // itg3205_dev.calibrate();
        itg3205_dev.setOffset(-0.77, 3.14, -1.73);
    }
    else {
        printf("\nITG3205 isn't found\r\n");
    }

    xyzFloat gyr;
    float temp;

    while (true) {
        gyr = itg3205_dev.getGyro();
        // temp = itg3205_dev.getTemperature();
        // printf("temp: %.2f\\r\n", temp);
        printf("gyrX: %.2f\tgyrY: %.2f\tgyrZ: %.2f\r\n", gyr.x, gyr.y, gyr.z);
        wait_us(50e3);
    }
}