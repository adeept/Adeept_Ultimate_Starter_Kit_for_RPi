#include <stdio.h>
#include <lgpio.h>

#define SW_PIN 27
#define CLK_PIN 18
#define DT_PIN 17
#define LED_PIN 22

static int globalCounter = 0;

void rotaryDeal(int h)
{
    static int lastCLK = 1;
    int currentCLK = lgGpioRead(h, CLK_PIN);
    int currentDT = lgGpioRead(h, DT_PIN);
    if (currentCLK != lastCLK) {
        if (currentCLK == 0) {
            if (currentDT != currentCLK)
                globalCounter++;
            else
                globalCounter--;
        }
        lastCLK = currentCLK;
    }
    if (globalCounter < 0)
        globalCounter = 0;
    if (globalCounter > 100)
        globalCounter = 100;
}

int main(void)
{
    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0)
            return -1;
    }

    lgGpioClaimInput(h, LG_SET_PULL_UP, SW_PIN);
    lgGpioClaimInput(h, LG_SET_PULL_UP, CLK_PIN);
    lgGpioClaimInput(h, LG_SET_PULL_UP, DT_PIN);
    lgGpioClaimOutput(h, 0, LED_PIN, 0);

    while (1) {
        rotaryDeal(h);
        if (lgGpioRead(h, SW_PIN) == 0) {
            globalCounter = 0;
            while (lgGpioRead(h, SW_PIN) == 0)
                lguSleep(0.01);
        }
        lgTxPwm(h, LED_PIN, 1000, globalCounter, 0, 0);
        printf("%d\n", globalCounter);
        lguSleep(0.005);
    }

    lgGpiochipClose(h);
    return 0;
}