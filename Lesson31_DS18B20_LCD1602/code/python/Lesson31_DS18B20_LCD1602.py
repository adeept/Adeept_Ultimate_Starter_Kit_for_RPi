import os
import glob
import time
from gpiozero import Device
from gpiozero.pins.lgpio import LGPIOFactory
from smbus2 import SMBus

Device.pin_factory = LGPIOFactory()

PCF8574_ADDR = 0x27
I2C_BUS = 1
BL_BIT = 0x08
RS_BIT = 0x01
RW_BIT = 0x02
EN_BIT = 0x04

DEVICE_ROOT = "/sys/bus/w1/devices/"
SENSOR_PREFIX = "28-"
TEMP_FILE = "w1_slave"

backlight = BL_BIT
bus = None

def pcf8574_write(data):
    bus.write_byte(PCF8574_ADDR, data | backlight)

def lcd_enable(data):
    pcf8574_write(data | EN_BIT)
    time.sleep(0.000001)
    pcf8574_write(data & ~EN_BIT)
    time.sleep(0.00005)

def lcd_write4(data, is_cmd):
    high = (data & 0xF0) | (0 if is_cmd else RS_BIT)
    low = ((data << 4) & 0xF0) | (0 if is_cmd else RS_BIT)
    lcd_enable(high)
    lcd_enable(low)

def lcd_cmd(cmd):
    lcd_write4(cmd, True)

def lcd_data(data):
    lcd_write4(data, False)

def lcd_position(row, col):
    offsets = [0x00, 0x40]
    lcd_cmd(0x80 | (offsets[row] + col))

def lcd_print(text):
    for char in text:
        lcd_data(ord(char))

def lcd_init():
    time.sleep(0.05)
    lcd_enable(0x30)
    time.sleep(0.0045)
    lcd_enable(0x30)
    time.sleep(0.0045)
    lcd_enable(0x30)
    time.sleep(0.00015)
    lcd_enable(0x20)
    time.sleep(0.0001)
    lcd_cmd(0x28)
    lcd_cmd(0x0C)
    lcd_cmd(0x01)
    time.sleep(0.002)
    lcd_cmd(0x06)

def get_ds18b20_path():
    devices = glob.glob(os.path.join(DEVICE_ROOT, SENSOR_PREFIX + "*"))
    for device in devices:
        sensor_path = os.path.join(device, TEMP_FILE)
        if os.path.isfile(sensor_path):
            return sensor_path
    return None

def read_ds18b20(sensor_path):
    try:
        with open(sensor_path, "r") as f:
            data = f.read()
    except OSError as e:
        print(f"Failed to read temperature data: {e}")
        return None

    index = data.find("t=")
    if index == -1:
        return None

    temp_str = ""
    for char in data[index + 2:]:
        if char.isdigit():
            temp_str += char
        else:
            break

    if not temp_str:
        return None

    return int(temp_str) / 1000.0

def main():
    global bus
    print("DS18B20 + I2C LCD Temperature Test")
    sensor_path = get_ds18b20_path()
    if sensor_path is None:
        print("No DS18B20 sensor detected")
        return
    print(f"Detected DS18B20 path: {sensor_path}")
    try:
        bus = SMBus(I2C_BUS)
        lcd_init()
        lcd_position(0, 0)
        lcd_print("DS18B20")
        lcd_position(1, 0)
        lcd_print("Starting...")
        while True:
            temperature = read_ds18b20(sensor_path)
            if temperature is not None:
                print(f"Temperature: {temperature:.3f} C")
                lcd_position(0, 0)
                lcd_print("Temp:           ")
                lcd_position(1, 0)
                lcd_print(f"{temperature:.3f} C")
            else:
                print("Failed to read temperature")
                lcd_position(0, 0)
                lcd_print("Temp Error      ")
                lcd_position(1, 0)
                lcd_print("                ")
            time.sleep(0.5)
    except KeyboardInterrupt:
        print("\nExit")
    except OSError as e:
        print(f"I2C error: {e}")
    finally:
        if bus is not None:
            bus.close()

if __name__ == "__main__":
    main()