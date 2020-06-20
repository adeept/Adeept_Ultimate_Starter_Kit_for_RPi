import PCF8591 as ADC
import time
import RPi.GPIO as GPIO
import signal
import atexit

atexit.register(GPIO.cleanup)  

servopin = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(servopin, GPIO.OUT, initial=False)
p = GPIO.PWM(servopin,50)
p.start(0)
time.sleep(2)

def loop():
  for i in range(0,181,10):
    p.ChangeDutyCycle(2.5 + 10 * i / 180)
    time.sleep(0.02)
    p.ChangeDutyCycle(0)
    time.sleep(0.2)

def reverse():
  for i in range(181,0,-10):
    p.ChangeDutyCycle(2.5 + 10 * i / 180)
    time.sleep(0.02)
    p.ChangeDutyCycle(0)
    time.sleep(0.2)


def setup():
    ADC.setup(0X48)
    global state


def direction():    #get joystick result
    state = ['home', 'up', 'down', 'left', 'right', 'pressed']
    i = 0   

    if ADC.read(1) <= 5:
        i = 1        #up
    if ADC.read(1) >= 250: 
        i = 2        #down

    if ADC.read(2) <= 5: 
        i = 3        #left
    if ADC.read(2) >= 250:
        i = 4        #right

    if ADC.read(0) == 0:
        i = 5        # Button pressed 

    if ADC.read(0) - 125 < 15 and ADC.read(0) - 125 > -15 and ADC.read(1) - 125 < 15 and ADC.read(1) - 125 > -15 and ADC.read(2) == 255:
        i = 0
    
    return state[i]

def destroy():
    pass    

if __name__ == '__main__':        # Program start from here
  setup() 
  status = ''
  try:
    while True:
      tmp = direction()
      print(tmp)
      if tmp != None and tmp != status:
        if tmp == 'up':
          loop()
        if tmp == 'down':
          reverse()
        status = tmp
      time.sleep(1)
  except KeyboardInterrupt:
    GPIO.cleanup()

