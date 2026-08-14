#include <stdio.h>
#include <lgpio.h>

#define PCF8591_ADDR    0x48  // PCF8591 address
#define MIC_SENSOR_CH   0x42  // Mic sensor channel (A2)

int main()
{
    int fd = lgI2cOpen(1, PCF8591_ADDR, 0);
    if (fd < 0) {
        printf("Failed to open I2C device\n");
        return -1;
    }

    float micSensorData;

    while (1) {
        micSensorData = lgI2cReadByteData(fd, MIC_SENSOR_CH);
        printf("micSensorData: %.2f\n", micSensorData / 10);
        lguSleep(0.5);
    }

    lgI2cClose(fd);
    return 0;
}
