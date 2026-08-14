#! /usr/bin/python3
from gpiozero import DistanceSensor
import time

# BCM pins: trigger=23 (physical 16), echo=24 (physical 18)
sensor = DistanceSensor(echo=24, trigger=23)

time.sleep(2)

try:
    while True:
        print('Distance: %0.2f m' % sensor.distance)
        time.sleep(0.5)
except KeyboardInterrupt:
    sensor.close()
