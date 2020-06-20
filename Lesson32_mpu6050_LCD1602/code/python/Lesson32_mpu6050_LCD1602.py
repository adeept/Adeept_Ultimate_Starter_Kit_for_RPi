import RPi.GPIO as GPIO
from mpu6050 import mpu6050
from time import sleep
from math import atan2, degrees
import smbus

def delay(time):
    sleep(time/1000.0)

def delayMicroseconds(time):
    sleep(time/1000000.0)

# Given a point (x, y) return the angle of that point relative to x axis.
# Returns: angle in degrees
def vector_2_degrees(x, y):
    angle = degrees(atan2(y, x))
    if angle < 0:
        angle += 360
    return angle


# Returns: tuple containing the two angles in degrees
def get_inclination(x, y, z):
    return vector_2_degrees(x, z), vector_2_degrees(y, z)

class Screen():

    enable_mask = 1<<2
    rw_mask = 1<<1
    rs_mask = 1<<0
    backlight_mask = 1<<3

    data_mask = 0x00

    def __init__(self, cols = 16, rows = 2, addr=0x27, bus=1):
        self.cols = cols
        self.rows = rows        
        self.bus_num = bus
        self.bus = smbus.SMBus(self.bus_num)
        self.addr = addr
        self.display_init()
        
    def enable_backlight(self):
        self.data_mask = self.data_mask|self.backlight_mask
        
    def disable_backlight(self):
        self.data_mask = self.data_mask& ~self.backlight_mask
       
    def display_data(self, *args):
        self.clear()
        for line, arg in enumerate(args):
            self.cursorTo(line, 0)
            self.println(arg[:self.cols].ljust(self.cols))
           
    def cursorTo(self, row, col):
        offsets = [0x00, 0x40, 0x14, 0x54]
        self.command(0x80|(offsets[row]+col))
    
    def clear(self):
        self.command(0x10)

    def println(self, line):
        for char in line:
            self.print_char(char)     

    def print_char(self, char):
        char_code = ord(char)
        self.send(char_code, self.rs_mask)

    def display_init(self):
        delay(1.0)
        self.write4bits(0x30)
        delay(4.5)
        self.write4bits(0x30)
        delay(4.5)
        self.write4bits(0x30)
        delay(0.15)
        self.write4bits(0x20)
        self.command(0x20|0x08)
        self.command(0x04|0x08, delay=80.0)
        self.clear()
        self.command(0x04|0x02)
        delay(3)

    def command(self, value, delay = 50.0):
        self.send(value, 0)
        delayMicroseconds(delay)
        
    def send(self, data, mode):
        self.write4bits((data & 0xF0)|mode)
        self.write4bits((data << 4)|mode)

    def write4bits(self, value):
        value = value & ~self.enable_mask
        self.expanderWrite(value)
        self.expanderWrite(value | self.enable_mask)
        self.expanderWrite(value)        

    def expanderWrite(self, data):
        self.bus.write_byte_data(self.addr, 0, data|self.data_mask)
       

if __name__ == "__main__":
    # Initialize mpu6050
    mpu = mpu6050(0x68)
    
    screen = Screen(bus=1, addr=0x27, cols=16, rows=2)
    screen.enable_backlight()

    tmp_xz, tmp_yz = 0, 0

    # Step count
    step_count = 0

    try:
      while True:
        accel_data = mpu.get_accel_data()
        angle_xz, angle_yz = get_inclination(accel_data['x'], accel_data['y'], accel_data['z'])
        if abs(angle_xz - tmp_xz) > 2 and abs(angle_yz - tmp_yz) > 2:
          step_count += 1
          tmp_xz = angle_xz
          tmp_yz = angle_yz
        
        line = f'Step Count: {step_count}'
        screen.display_data(line, '')

        sleep(1)
    except KeyboardInterrupt:
      GPIO.cleanup()




