#! /usr/bin/python3
from gpiozero import DistanceSensor, Buzzer
import time

# BCM pins: trigger=23, echo=24 (physical 16,18)
# Buzzer: BCM18 (physical pin 12), active low
sensor = DistanceSensor(echo=24, trigger=23)
buzzer = Buzzer(18, active_high=False)

time.sleep(2)

# Turn on beep
def beep_on():
    buzzer.on()
    time.sleep(0.1)
    buzzer.off()
    time.sleep(0.1)

# Turn off beep
def beep_off():
    buzzer.on()  # Active low: high = off

def destroy():
    sensor.close()
    buzzer.close()

if __name__ == '__main__':
    safety_distance = 0.5  # Safety distance in meters

    try:
        while True:
            current_distance = sensor.distance
            time.sleep(1)
            print('Current distance: %0.2f m' % current_distance)

            if current_distance < safety_distance:
                beep_on()
            else:
                beep_off()

            time.sleep(1)
    except KeyboardInterrupt:
        destroy()
