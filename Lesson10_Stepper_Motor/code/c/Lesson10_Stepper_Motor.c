#include <lgpio.h>
#include <stdio.h>

// BCM pins: 17,27,22,18
#define IN1  17
#define IN2  27
#define IN3  22
#define IN4  18

void setStep(int h, int a, int b, int c, int d)
{
    lgGpioWrite(h, IN1, a);
    lgGpioWrite(h, IN2, b);
    lgGpioWrite(h, IN3, c);
    lgGpioWrite(h, IN4, d);
}

void stop(int h)
{
    setStep(h, 0, 0, 0, 0);
}

void forward(int h, int t_ms, int steps)
{
    int i;
    double t = t_ms / 1000.0;
    for (i = 0; i < steps; i++) {
        setStep(h, 1, 0, 0, 0);
        lguSleep(t);
        setStep(h, 0, 1, 0, 0);
        lguSleep(t);
        setStep(h, 0, 0, 1, 0);
        lguSleep(t);
        setStep(h, 0, 0, 0, 1);
        lguSleep(t);
    }
}

void backward(int h, int t_ms, int steps)
{
    int i;
    double t = t_ms / 1000.0;
    for (i = 0; i < steps; i++) {
        setStep(h, 0, 0, 0, 1);
        lguSleep(t);
        setStep(h, 0, 0, 1, 0);
        lguSleep(t);
        setStep(h, 0, 1, 0, 0);
        lguSleep(t);
        setStep(h, 1, 0, 0, 0);
        lguSleep(t);
    }
}

int main(void)
{
    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    lgGpioClaimOutput(h, 0, IN1, 0);
    lgGpioClaimOutput(h, 0, IN2, 0);
    lgGpioClaimOutput(h, 0, IN3, 0);
    lgGpioClaimOutput(h, 0, IN4, 0);

    while (1) {
        printf("forward...\n");
        forward(h, 3, 512);
        printf("stop...\n");
        stop(h);
        lguSleep(2.0);
        printf("backward...\n");
        backward(h, 3, 256);
        printf("stop...\n");
        stop(h);
        lguSleep(2.0);
    }

    lgGpiochipClose(h);
    return 0;
}
