#include <lgpio.h>
#include <stdio.h>

#define LED_PIN 18  // BCM18 (physical pin 12)
int h;

int main(void)
{
    int i;
    h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    printf("Program is starting ...\n");
    lgGpioClaimOutput(h, 0, LED_PIN, 0);

    while (1) {
        for (i = 100; i >= 0; i--) {
            lgTxPwm(h, LED_PIN, 1000, i, 0, 0);  // 1kHz 
            lguSleep(0.02);
        }
        lguSleep(0.3);
        for (i = 0; i <= 100; i++) {
            lgTxPwm(h, LED_PIN, 1000, i, 0, 0);
            lguSleep(0.02);
        }
        lguSleep(0.3);
    }

    lgGpiochipClose(h);
    return 0;
}
