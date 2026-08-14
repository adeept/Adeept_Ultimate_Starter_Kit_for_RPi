#include <lgpio.h>
#include <stdio.h>

#define LED_PIN 17  // BCM17 (physical pin 11)
int h;

int main(void)
{
    h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    lgGpioClaimOutput(h, 0, LED_PIN, 0);

    while (1) {
        lgGpioWrite(h, LED_PIN, 0);  // LED on (active low)
        printf("led on...\n");
        lguSleep(0.5);
        lgGpioWrite(h, LED_PIN, 1);  // LED off
        printf("...led off\n");
        lguSleep(0.5);
    }

    lgGpiochipClose(h);
    return 0;
}
