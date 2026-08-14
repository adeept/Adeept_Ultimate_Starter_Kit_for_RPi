#include <lgpio.h>
#include <stdio.h>

#define PIR_PIN  17  // BCM17 (wiringPi 0, physical pin 11)

int main(void)
{
    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    lgGpioClaimInput(h, 0, PIR_PIN);

    while (1) {
        if (lgGpioRead(h, PIR_PIN) == 0)
            printf("...Movement not detected!\n");
        else
            printf("Movement detected!...\n");
        lguSleep(0.5);
    }

    lgGpiochipClose(h);
    return 0;
}
