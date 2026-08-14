#include <lgpio.h>
#include <stdio.h>
#define PCF8591_ADDR 0x48
#define Y_CH 0x41
#define X_CH 0x42
#define Z_CH 0x43

int readADC(int fd, int channel)
{
    int value;
    if (lgI2cWriteByte(fd, channel) < 0)
        return -1;
    if (lgI2cReadByte(fd) < 0)
        return -1;
    value = lgI2cReadByte(fd);
    return value;
}

int main(void)
{
    int fd = lgI2cOpen(1, PCF8591_ADDR, 0);
    int x, y, z;
    if (fd < 0) {
        printf("Failed to open I2C device\n");
        return -1;
    }
    while (1) {
        x = readADC(fd, X_CH);
        y = readADC(fd, Y_CH);
        z = readADC(fd, Z_CH);
        if (x < 0 || y < 0 || z < 0) {
            printf("I2C read error\n");
            lguSleep(0.1);
            continue;
        }
        if (x < 50)
            printf("left\n");
        else if (x > 220)
            printf("right\n");
        else if (y < 100)
            printf("up\n");
        else if (y > 230)
            printf("down\n");
        lguSleep(0.1);
    }
    lgI2cClose(fd);
    return 0;
}