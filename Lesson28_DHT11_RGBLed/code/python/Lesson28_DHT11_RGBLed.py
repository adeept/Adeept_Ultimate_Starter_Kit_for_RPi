import RPi.GPIO as GPIO
import time
import Adafruit_DHT


class RGBLed:

  def __init__(self, r_pin, g_pin, b_pin):
    self.pins = {
      'R': r_pin,
      'G': g_pin,
      'B': b_pin
    }               # Numbers GPIOs by physical location
    for _, pin in self.pins.items():
      GPIO.setup(pin, GPIO.OUT)                 # Set pins' mode is output
      GPIO.output(pin, GPIO.HIGH)             # Set pins to high(+3.3V) to off led
    
    self.p_R = GPIO.PWM(self.pins['R'], 2000) # set Frequece to 2KHz
    self.p_G = GPIO.PWM(self.pins['G'], 1999)
    self.p_B = GPIO.PWM(self.pins['B'], 5000)

    self.p_R.start(100)                       # Initial duty Cycle = 100(leds off)
    self.p_G.start(100)
    self.p_B.start(100)
  
  @staticmethod
  def map(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
  
  def off(self):
    for _, pin in self.pins.items():
      GPIO.output(pin, GPIO.HIGH)             # Turn off all leds

  def setColor(self, color):
    # For example : col = 0x112233
    R_val = (color & 0xff0000) >> 16
    G_val = (color & 0x00ff00) >> 8
    B_val = (color & 0x0000ff) >> 0

    R_val = RGBLed.map(R_val, 0, 255, 0, 100)
    G_val = RGBLed.map(G_val, 0, 255, 0, 100)
    B_val = RGBLed.map(B_val, 0, 255, 0, 100)
    
    self.p_R.ChangeDutyCycle(100 - R_val)     # Change duty cycle
    self.p_G.ChangeDutyCycle(100 - G_val)
    self.p_B.ChangeDutyCycle(100 - B_val)
  
  def loop(self):
    colors = [0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF, 0X6F00D2, 0xFF5809]
    while True:
      for color in colors:
        self.setColor(color)
        time.sleep(0.5)

  def destroy(self):
    self.p_R.stop()
    self.p_G.stop()
    self.p_B.stop()

    self.off()
    GPIO.cleanup()


def DHT11_read(channel):
  data = []
  j = 0
  time.sleep(1)

  GPIO.setup(channel, GPIO.OUT)

  GPIO.output(channel, GPIO.LOW)
  time.sleep(0.02)
  GPIO.output(channel, GPIO.HIGH)

  GPIO.setup(channel, GPIO.IN)

  while GPIO.input(channel) == GPIO.LOW:
    continue

  while GPIO.input(channel) == GPIO.HIGH:
    continue

  while j < 40:
    k = 0
    while GPIO.input(channel) == GPIO.LOW:
      continue

    while GPIO.input(channel) == GPIO.HIGH:
      k += 1
      if k > 100:
        break

    if k < 8:
      data.append(0)
    else:
      data.append(1)
    j += 1

  # print("sensor is working.")
  # print(data)

  humidity_bit = data[0:8]
  humidity_point_bit = data[8:16]
  temperature_bit = data[16:24]
  temperature_point_bit = data[24:32]
  check_bit = data[32:40]

  humidity = 0
  humidity_point = 0
  temperature = 0
  temperature_point = 0
  check = 0

  for i in range(8):
    humidity += humidity_bit[i] * 2 ** (7 - i)
    humidity_point += humidity_point_bit[i] * 2 ** (7 - i)
    temperature += temperature_bit[i] * 2 ** (7 - i)
    temperature_point += temperature_point_bit[i] * 2 ** (7 - i)
    check += check_bit[i] * 2 ** (7 - i)
    print(humidity)
  tmp = humidity + humidity_point + temperature + temperature_point

  if check:
    return {
      'temperature': float(str(temperature) + '.' + str(temperature_point)),
      'humidity': float(str(humidity) + '.' + str(humidity_point))
    }
  else:
    return {}


if __name__ == "__main__":
  R = 11
  G = 12
  B = 13
  channel = 15
  
  GPIO.setmode(GPIO.BOARD)
  rgbLed = RGBLed(R, G, B)

  
  try:
    while True:
      dht11_data = DHT11_read(15)
      
      if dht11_data:
        print(dht11_data['temperature'], dht11_data['humidity'])
        color = int(dht11_data['temperature'] + dht11_data['humidity'])
        rgbLed.setColor(color)
      else:
        print('wrong')

      time.sleep(1)
  except KeyboardInterrupt:
    rgbLed.destroy()
