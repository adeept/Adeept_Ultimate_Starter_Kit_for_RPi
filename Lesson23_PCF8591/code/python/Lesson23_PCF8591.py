from gpiozero import Device
from gpiozero.pins.lgpio import LGPIOFactory
import smbus
import time

Device.pin_factory = LGPIOFactory()

PCF8591_ADDR = 0x48
A0 = 0x40
A1 = 0x41

bus = smbus.SMBus(1)

def readADC(channel):
    bus.write_byte(PCF8591_ADDR, channel)
    bus.read_byte(PCF8591_ADDR)
    return bus.read_byte(PCF8591_ADDR)

print("PCF8591 gpiozero Test")
print("I2C address: 0x%02X" % PCF8591_ADDR)

while True:
    photosensitive_data = readADC(A0)
    temp_data = readADC(A1)

    print("Photoresistance:", photosensitive_data)
    print("Temperature:", temp_data)

    time.sleep(0.5)