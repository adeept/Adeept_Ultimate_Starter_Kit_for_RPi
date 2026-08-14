#include <lgpio.h>
#include <stdio.h>

#define PIR_PIN   27  // BCM27 (wiringPi 2)
#define LED_PIN   17  // BCM17 (wiringPi 0)

int main(void)
{
    int a, b;

    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    lgGpioClaimInput(h, 0, PIR_PIN);
    lgGpioClaimOutput(h, 0, LED_PIN, 1);

    while (1) {
        a = lgGpioRead(h, PIR_PIN);

        if (a == 0) { printf("...Movement not detected!\n"); b = 1; }
        if (a == 1) { printf("Movement detected!...\n"); b = 0; }

        lgGpioWrite(h, LED_PIN, b);
        lguSleep(1.0);
    }

    lgGpiochipClose(h);
    return 0;
}
