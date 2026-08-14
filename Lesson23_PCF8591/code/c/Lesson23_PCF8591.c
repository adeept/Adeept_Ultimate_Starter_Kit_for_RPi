#include <stdio.h>
#include <lgpio.h>
#include <unistd.h>

#define PCF8591_ADDR 0x48
#define A0 0x40
#define A1 0x41

int readADC(int fd, int channel)
{
    int value;
    unsigned char data[1];

    data[0] = channel;

    if (lgI2cWriteDevice(fd, data, 1) < 0)
        return -1;

    // First byte after selecting the channel is a dummy conversion
    value = lgI2cReadByte(fd);
    if (value < 0)
        return -1;

    value = lgI2cReadByte(fd);
    if (value < 0)
        return -1;

    return value;
}

int main(void)
{
    int fd;
    int photosensitive_data;
    int temp_data;

    fd = lgI2cOpen(1, PCF8591_ADDR, 0);
    if (fd < 0) {
        printf("Failed to open PCF8591 at address 0x%02X\n", PCF8591_ADDR);
        return -1;
    }

    printf("PCF8591 lgpio Test\n");
    printf("I2C address: 0x%02X\n", PCF8591_ADDR);

    while (1) {
        photosensitive_data = readADC(fd, A0);
        temp_data = readADC(fd, A1);

        if (photosensitive_data < 0 || temp_data < 0) {
            printf("Failed to read PCF8591\n");
            break;
        }

        printf("Photoresistance: %d\n", photosensitive_data);
        printf("Temperature: %d\n", temp_data);

        lguSleep(0.5);
    }

    lgI2cClose(fd);
    return 0;
}