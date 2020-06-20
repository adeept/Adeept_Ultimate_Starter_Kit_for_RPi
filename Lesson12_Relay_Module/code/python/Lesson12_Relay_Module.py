#encoding:utf-8

import RPi.GPIO as GPIO
import time

time_out=3  	# Delay time 5s
RELAY=4		

GPIO.setmode(GPIO.BCM)
GPIO.setup(RELAY,GPIO.OUT)

try:
   while True:
     GPIO.output(RELAY, GPIO.HIGH)
     time.sleep(time_out)
     GPIO.output(RELAY, GPIO.LOW)
     time.sleep(time_out)
     
except KeyboardInterrupt:
   GPIO.cleanup()
