import RPi.GPIO as GPIO
import dht11
import time
import datetime

# initialize GPIO
GPIO.setwarnings(True)
GPIO.setmode(GPIO.BOARD)

# read data using pin 7
instance = dht11.DHT11(pin=7)

try:
	while True:
	    result = instance.read()
	    if result.is_valid():

	        print("Temperature: %-3.1f C" % result.temperature)
	        print("Humidity: %-3.1f %%" % result.humidity)
	        print("=====================")
	    time.sleep(2)

except KeyboardInterrupt:
    print("Cleanup")
    GPIO.cleanup()