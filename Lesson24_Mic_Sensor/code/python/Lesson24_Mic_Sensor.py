import smbus
import time


PCF8591_address = 0x48
MicSensor_address = 0x42

bus = smbus.SMBus(1)

while True:
  bus.write_byte(PCF8591_address, MicSensor_address)
  micSensorData = bus.read_byte(PCF8591_address)
  print("mic sensor data: ", micSensorData / 10)

  time.sleep(1)