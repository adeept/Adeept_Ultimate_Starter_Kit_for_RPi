#include <stdio.h>
#include <lgpio.h>
#include <unistd.h>

#define TIMEOUT 5  // Delay 5 seconds
#define RELAY_PIN  4  // BCM4 (wiringPi 7, physical pin 7)
#define ON  1
#define OFF 0

int main()
{
    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    lgGpioClaimOutput(h, 0, RELAY_PIN, 0);

    while (1) {
        lgGpioWrite(h, RELAY_PIN, ON);
        sleep(TIMEOUT);
        lgGpioWrite(h, RELAY_PIN, OFF);
        sleep(TIMEOUT);
    }

    lgGpiochipClose(h);
    return 0;
}
