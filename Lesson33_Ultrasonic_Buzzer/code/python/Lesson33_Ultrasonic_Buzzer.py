#! /usr/bin/python3
import RPi.GPIO as GPIO
import time


def setup(trig_pin, echo_pin, beep_pin):
  GPIO.setmode(GPIO.BOARD)
  GPIO.setup(trig_pin,GPIO.OUT,initial=GPIO.LOW)
  GPIO.setup(echo_pin,GPIO.IN)
  time.sleep(2)
  GPIO.setup(beep_pin, GPIO.OUT)   # Set pin mode as output
  GPIO.output(beep_pin, GPIO.HIGH) # Set pin to high(+3.3V) to off the beep

# Turn on beep
def beep_on(beep_pin):
    #GPIO.output(beep_pin, GPIO.LOW)
    time.sleep(0.1)
    GPIO.output(beep_pin, GPIO.HIGH)
    time.sleep(0.1)

# Turn off beep
def beep_off(beep_pin):
  GPIO.output(beep_pin, GPIO.LOW)

def destroy():
  GPIO.cleanup()

def checkdist(trig_pin, echo_pin):
	GPIO.output(trig_pin, GPIO.HIGH)
	time.sleep(0.000015)
	GPIO.output(trig_pin, GPIO.LOW)
	while not GPIO.input(echo_pin):
		pass
	t1 = time.time()
	while GPIO.input(echo_pin):
		pass
	t2 = time.time()
	return (t2-t1)*340/2


if __name__ == '__main__':
  trig_pin = 16
  echo_pin = 18
  beep_pin = 12
  setup(trig_pin, echo_pin, beep_pin)
  safety_distance = 0.5     # Safety distance
  
  try:
    while True:
      current_distance = checkdist(trig_pin, echo_pin)
      time.sleep(1)
      print('Current distance: %0.2f m' % current_distance)
      
      if current_distance < safety_distance:
        beep_on(beep_pin)
      else:
        beep_off(beep_pin)

    time.sleep(1)
  except KeyboardInterrupt:
    destroy()



