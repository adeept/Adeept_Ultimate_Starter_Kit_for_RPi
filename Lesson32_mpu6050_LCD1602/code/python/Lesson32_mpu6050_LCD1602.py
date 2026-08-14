import math
import time
from gpiozero import Device
from gpiozero.pins.lgpio import LGPIOFactory
from smbus2 import SMBus
from mpu6050 import mpu6050

Device.pin_factory = LGPIOFactory()

PCF8574_ADDR = 0x27
MPU6050_ADDR = 0x68
RS_BIT = 0x01
RW_BIT = 0x02
EN_BIT = 0x04
BL_BIT = 0x08

backlight = BL_BIT
lcd_bus = None

def pcf8574_write(data):
    lcd_bus.write_byte(PCF8574_ADDR, data | backlight)

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

def vector_2_degrees(x, y):
    angle = math.degrees(math.atan2(y, x))
    if angle < 0:
        angle += 360
    return angle

def get_angle_xz(x, z):
    return vector_2_degrees(x, z)

def get_angle_yz(y, z):
    return vector_2_degrees(y, z)

def main():
    global lcd_bus
    print("MPU6050 + LCD1602 Step Counter")
    try:
        sensor = mpu6050(MPU6050_ADDR)
        print("MPU6050 initialized")
    except Exception as e:
        print(f"Initialize MPU6050 Error: {e}")
        return
    try:
        lcd_bus = SMBus(1)
        lcd_init()
        print("LCD1602 initialized")
    except Exception as e:
        print(f"LCD initialization failed: {e}")
        return
    tmp_xz = 0.0
    tmp_yz = 0.0
    step_count = 0
    try:
        while True:
            accel_data = sensor.get_accel_data()
            x = accel_data["x"]
            y = accel_data["y"]
            z = accel_data["z"]
            angle_xz = get_angle_xz(x, z)
            angle_yz = get_angle_yz(y, z)
            if abs(angle_xz - tmp_xz) > 2 and abs(angle_yz - tmp_yz) > 2:
                step_count += 1
                tmp_xz = angle_xz
                tmp_yz = angle_yz
            print(f"XZ: {angle_xz:.2f}  YZ: {angle_yz:.2f}  Step: {step_count}")
            lcd_position(1, 0)
            lcd_print(f"step count: {step_count:<10}")
            time.sleep(1.0)
    except KeyboardInterrupt:
        print("\nExit")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        if lcd_bus:
            lcd_bus.close()

if __name__ == "__main__":
    main()