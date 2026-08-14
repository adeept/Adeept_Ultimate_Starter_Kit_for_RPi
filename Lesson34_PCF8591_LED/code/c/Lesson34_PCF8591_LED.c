#include <stdio.h>
#include <lgpio.h>

#define PCF8591_ADDR  0x48
#define A0            0x40  // Photoresistor
#define LED_PIN       17    // BCM17 (wiringPi 0)

int main()
{
    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    int fd = lgI2cOpen(1, PCF8591_ADDR, 0);
    if (fd < 0) {
        printf("Failed to open I2C device\n");
        return -1;
    }

    float light_intensity;
    int light_threshold = 100;

    lgGpioClaimOutput(h, 0, LED_PIN, 1);

    while (1) {
        light_intensity = lgI2cReadByteData(fd, A0);
        printf("Light intensity: %.2f\n", light_intensity);

        if (light_intensity > light_threshold)
            lgGpioWrite(h, LED_PIN, 0);  // LED on
        else
            lgGpioWrite(h, LED_PIN, 1);  // LED off

        lguSleep(0.5);
    }

    lgI2cClose(fd);
    lgGpiochipClose(h);
    return 0;
}
