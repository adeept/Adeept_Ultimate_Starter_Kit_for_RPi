import time
import RPi.GPIO as GPIO
from rpi_ws281x import *
import random

pin = 18  # BCM pin
num = 3

pixels = PixelStrip(num, pin)
pixels.begin()

colors = [
  Color(0, 0, 50),
  Color(50, 0, 0),
  Color(0, 255, 0),
  Color(22, 123, 52),
  Color(2, 88, 45),
  Color(127, 127, 127)
]

while True:
  for i in range(num):
    pixels.setPixelColor(i, random.choice(colors))
    pixels.show()
  time.sleep(1)

