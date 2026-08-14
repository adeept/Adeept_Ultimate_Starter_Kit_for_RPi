#include <lgpio.h>
#include <stdio.h>

#define BEEP_PIN  18  // BCM18 (wiringPi 1, physical pin 12)

int main(void)
{
    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    lgGpioClaimOutput(h, 0, BEEP_PIN, 1);

    while (1) {
        lgGpioWrite(h, BEEP_PIN, 0);  // Beep on (active low)
        lguSleep(0.1);
        lgGpioWrite(h, BEEP_PIN, 1);  // Beep off
        lguSleep(0.1);
    }

    lgGpiochipClose(h);
    return 0;
}
