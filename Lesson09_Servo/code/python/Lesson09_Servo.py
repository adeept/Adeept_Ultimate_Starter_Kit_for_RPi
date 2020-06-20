import RPi.GPIO as GPIO
import time
import signal
import atexit

atexit.register(GPIO.cleanup)  

servopin = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(servopin, GPIO.OUT, initial=False)
p = GPIO.PWM(servopin,50)
p.start(0)
time.sleep(1)

while(True):
	for i in range(0,181,10):
		print('i=',i)
		p.ChangeDutyCycle(2.5 + 10 * i / 180)
		time.sleep(0.2)
		p.ChangeDutyCycle(0)
		time.sleep(0.5)

	for i in range(180,0,-10):
		print('........i=',i)
		p.ChangeDutyCycle(2.5 + 10 * i / 180)
		time.sleep(0.2)
		p.ChangeDutyCycle(0)
		time.sleep(0.5)