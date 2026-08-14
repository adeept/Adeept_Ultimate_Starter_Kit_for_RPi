#include <lgpio.h>
#include <stdio.h>

#define LED_PIN    17  // BCM17 (wiringPi 0)
#define BTN_PIN    18  // BCM18 (wiringPi 1)
int h;

int main(void)
{
    h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }


    lgGpioClaimOutput(h, 0, LED_PIN, 1);
    lgGpioClaimInput(h, LG_SET_PULL_UP, BTN_PIN);

    while (1) {
        if (lgGpioRead(h, BTN_PIN) == 1)
            lgGpioWrite(h, LED_PIN, 1);
        else
            lgGpioWrite(h, LED_PIN, 0);
    }

    lgGpiochipClose(h);
    return 0;
}
