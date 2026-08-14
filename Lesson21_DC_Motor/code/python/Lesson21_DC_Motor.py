#!/usr/bin/env python
from gpiozero import Motor
import time

# BCM pins: 17,18 (physical pins 11,12)
motor = Motor(forward=17, backward=18)

def setup():
    motor.stop()

def motor_run(status, direction):
    if status == 1:  # Run
        if direction == 1:
            motor.forward()
        else:
            motor.backward()
    else:  # Stop
        motor.stop()

def loop():
    while True:
        motor_run(1, 1)
        time.sleep(5)
        motor_run(0, 1)
        time.sleep(5)
        motor_run(1, 0)
        time.sleep(5)

def destroy():
    motor.stop()
    motor.close()

if __name__ == '__main__':
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
