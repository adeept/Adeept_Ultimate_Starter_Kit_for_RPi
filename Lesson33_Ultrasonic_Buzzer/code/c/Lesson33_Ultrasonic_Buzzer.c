#include <lgpio.h>
#include <stdio.h>
#include <sys/time.h>

#define TRIG_PIN   23  // BCM23 (wiringPi 4)
#define ECHO_PIN   24  // BCM24 (wiringPi 5)
#define BEEP_PIN   18  // BCM18 (wiringPi 1)

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

    lgGpioWrite(h, TRIG_PIN, 1);
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

void beepOn(int h)
{
    lgGpioWrite(h, BEEP_PIN, 1);  // Beep off
    lguSleep(1.0);
    lgGpioWrite(h, BEEP_PIN, 0);  // Beep on
    lguSleep(1.0);
}

void beepOff(int h)
{
    lgGpioWrite(h, BEEP_PIN, 0);  // Beep off
}

int main(void)
{
    float current_dis;
    float safety_dis = 50;

    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    ultraInit(h);
    lgGpioClaimOutput(h, 0, BEEP_PIN, 0);

    while (1) {
        current_dis = disMeasure(h);
        printf("Current distance = %0.2f cm\n", current_dis);

        if (current_dis < safety_dis)
            beepOn(h);
        else
            beepOff(h);

        lguSleep(1.0);
    }

    lgGpiochipClose(h);
    return 0;
}
