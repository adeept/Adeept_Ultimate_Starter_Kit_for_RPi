#include <lgpio.h>
#include <stdio.h>
int h;


// BCM pins: 17,18,27,22,23,24,25,4 (wiringPi 0-7)
const int ledPins[] = {17, 18, 27, 22, 23, 24, 25, 4};

void led_on(int h, int n) { lgGpioWrite(h, ledPins[n], 0); }
void led_off(int h, int n) { lgGpioWrite(h, ledPins[n], 1); }

int main(void)
{
    int i;
    h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    for (i = 0; i < 8; i++)
        lgGpioClaimOutput(h, 0, ledPins[i], 1);  // All LEDs off initially

    while (1) {
        for (i = 0; i < 8; i++) {  // Left to right
            led_on(h, i);
            lguSleep(0.3);
            led_off(h, i);
        }
        for (i = 7; i >= 0; i--) {  // Right to left
            led_on(h, i);
            lguSleep(0.3);
            led_off(h, i);
        }
    }

    lgGpiochipClose(h);
    return 0;
}
