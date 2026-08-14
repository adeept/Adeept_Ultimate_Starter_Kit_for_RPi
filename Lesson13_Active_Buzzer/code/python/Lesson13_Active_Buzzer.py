#!/usr/bin/env python
from gpiozero import Buzzer
import time

# BCM18 (physical pin 12), active low buzzer
beep = Buzzer(18, active_high=False)

def setup():
    beep.off()  # Set pin high to turn off buzzer

def loop():
    while True:
        beep.on()
        time.sleep(0.1)
        beep.off()
        time.sleep(0.1)

def destroy():
    beep.off()
    beep.close()

if __name__ == '__main__':
    print('Press Ctrl+C to end the program...')
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
