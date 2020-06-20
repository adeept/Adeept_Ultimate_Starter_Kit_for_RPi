import RPi.GPIO as GPIO
import time

PIR_OUT_PIN = 13    # pin11

def setup():
	GPIO.setmode(GPIO.BOARD)       # Numbers GPIOs by physical location
	GPIO.setup(PIR_OUT_PIN, GPIO.IN)    # Set BtnPin's mode is input

def destroy():
	GPIO.cleanup()                     # Release resource


class Led:
  def __init__(self, pin):
    self.pin = pin  # LED GPIO pin

  def setup(self):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(self.pin, GPIO.OUT)
    GPIO.output(self.pin, GPIO.HIGH)

  def on(self):
    """turn on led"""
    GPIO.output(self.pin, GPIO.LOW)
  
  def off(self):
    """turn off led"""
    GPIO.output(self.pin, GPIO.HIGH)

  def destroy(self):
    """close GPIO pin"""
    GPIO.cleanup()

if __name__ == '__main__':     # Program start from here
	setup()
	led = Led(11)
	led.setup()
	try:
		while True:
		  if GPIO.input(PIR_OUT_PIN) == GPIO.LOW:
		    print('...Movement not detected!')
		    led.off()
		  else:
		    print('Movement detected!...')
		    led.on()
		  time.sleep(0.5)

	except KeyboardInterrupt:  # When 'Ctrl+C' is pressed, the child program destroy() will be  executed.
		destroy()
		led.destroy()


