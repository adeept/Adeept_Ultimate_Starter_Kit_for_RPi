import RPi.GPIO as GPIO
import PCF8591 as ADC
import time

pins = [11, 12, 13, 15, 16]  # Leds pin


def joySetup():
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


def ledsSetup():
  GPIO.setmode(GPIO.BOARD)        # Numbers GPIOs by physical location
  for pin in pins:
    GPIO.setup(pin, GPIO.OUT)   # Set all pins' mode is output
    GPIO.output(pin, GPIO.HIGH)  # Set all pins to high(+3.3V) to off led


def loop():
  for pin in pins:
    GPIO.output(pin, GPIO.LOW)
    time.sleep(0.5)
    GPIO.output(pin, GPIO.HIGH)


def reverse():
  for pin in pins[::-1]:
    GPIO.output(pin, GPIO.LOW)
    time.sleep(0.5)
    GPIO.output(pin, GPIO.HIGH)


def off():
  for pin in pins:
    GPIO.output(pin, GPIO.HIGH)    # turn off all leds


def destroy():
  off()
  GPIO.cleanup()                     # Release resource


if __name__ == '__main__':     # Program start from here
  ledsSetup()
  joySetup()

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
      else:
        off()
      time.sleep(1)
  except KeyboardInterrupt:
    destroy()
