#include <lgpio.h>
#include <stdio.h>
#include <sys/time.h>

#define TRIG_PIN  23  // BCM23 (wiringPi 4, physical pin 16)
#define ECHO_PIN  24  // BCM24 (wiringPi 5, physical pin 18)

void ultraInit(int h)
{
    lgGpioClaimInput(h, 0, ECHO_PIN);
    lgGpioClaimOutput(h, 0, TRIG_PIN, 0);
}

float disMeasure(int h)
{
    struct timeval tv1, tv2;
    long start, stop;
    float dis;

    lgGpioWrite(h, TRIG_PIN, 0);
    lguSleep(0.000002);

    lgGpioWrite(h, TRIG_PIN, 1);  // Trigger pulse
    lguSleep(0.00001);
    lgGpioWrite(h, TRIG_PIN, 0);

    while (lgGpioRead(h, ECHO_PIN) != 1);
    gettimeofday(&tv1, NULL);

    while (lgGpioRead(h, ECHO_PIN) != 0);
    gettimeofday(&tv2, NULL);

    start = tv1.tv_sec * 1000000 + tv1.tv_usec;
    stop = tv2.tv_sec * 1000000 + tv2.tv_usec;

    dis = (float)(stop - start) / 1000000 * 34000 / 2;
    return dis;
}

int main(void)
{
    float dis;

    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    ultraInit(h);

    while (1) {
        dis = disMeasure(h);
        printf("Distance = %0.2f cm\n", dis);
        lguSleep(1.0);
    }

    lgGpiochipClose(h);
    return 0;
}
