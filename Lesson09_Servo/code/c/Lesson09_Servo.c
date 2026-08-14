#include <stdio.h>
#include <lgpio.h>

#define SERVO_PIN  17  // BCM17 (wiringPi 0, physical pin 11)

// Generate servo PWM pulse: pulse width 500~2500us, period 20ms
void servo(int h, int pulse_us)
{
    lgGpioWrite(h, SERVO_PIN, 1);
    lguSleep(pulse_us / 1000000.0);
    lgGpioWrite(h, SERVO_PIN, 0);
    lguSleep((20000 - pulse_us) / 1000000.0);
}

int main(void)
{
    int i;

    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    lgGpioClaimOutput(h, 0, SERVO_PIN, 0);

    while (1) {
        servo(h, 500);
        lguSleep(0.5);
        for (i = 500; i <= 2500; i += 500) {
            servo(h, i);
            printf("i = %d\n", i);
            lguSleep(0.5);
        }
        servo(h, 2500);
        lguSleep(0.5);
        for (i = 2500; i >= 500; i -= 500) {
            servo(h, i);
            printf("............i = %d\n", i);
            lguSleep(0.5);
        }
    }

    lgGpiochipClose(h);
    return 0;
}
