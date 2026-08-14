#include <lgpio.h>
#include <stdio.h>

// BCM pins: 17,18 (wiringPi 0,1)
#define MOTOR_A  17
#define MOTOR_B  18

void motor(int h, int status, int dir)
{
    if (status == 0) {  // Stop
        lgGpioWrite(h, MOTOR_A, 1);
        lgGpioWrite(h, MOTOR_B, 1);
    } else {  // Run
        if (dir == 0) {  // Clockwise
            lgGpioWrite(h, MOTOR_A, 1);
            lgGpioWrite(h, MOTOR_B, 0);
        } else {  // Counter-clockwise
            lgGpioWrite(h, MOTOR_A, 0);
            lgGpioWrite(h, MOTOR_B, 1);
        }
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

    lgGpioClaimOutput(h, 0, MOTOR_A, 0);
    lgGpioClaimOutput(h, 0, MOTOR_B, 0);

    while (1) {
        motor(h, 1, 1);
        lguSleep(5.0);
        motor(h, 0, 1);  // Stop
        lguSleep(5.0);
        motor(h, 1, 0);
        lguSleep(5.0);
    }

    lgGpiochipClose(h);
    return 0;
}
