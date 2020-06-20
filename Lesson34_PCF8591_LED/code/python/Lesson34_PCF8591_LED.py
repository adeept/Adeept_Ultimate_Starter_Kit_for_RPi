import smbus   
import time
import RPi.GPIO as GPIO


bus = smbus.SMBus(1)
channels = [0x40, 0x41, 0x42, 0x43]


def setup(Addr):
    global address
    address = Addr


def read(chn):
    bus.write_byte(address, channels[chn])
    bus.read_byte(address)
    return bus.read_byte(address)


def write(val):
    bus.write_byte_data(address, 0x40, int(val))

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


if __name__ == '__main__':
    setup(0x48)
    light_threshold = 100
    led = Led(11)
    led.setup()

    try:
      while True:
        light_intensity = read(0)
        print('Light intensity:', light_intensity)
        if light_intensity > light_threshold:
          led.on()
        else:
          led.off()
        time.sleep(1)
    except KeyboardInterrupt:
      led.destroy()



