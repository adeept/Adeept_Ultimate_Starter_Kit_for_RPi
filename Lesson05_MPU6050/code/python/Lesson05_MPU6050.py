import RPi.GPIO as GPIO
from mpu6050 import mpu6050
import time
from math import atan2, degrees


# Initialize mpu6050
mpu = mpu6050(0x68)


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

try:
  while True:
    # get temp
    print(mpu.get_temp())

    # get accelerometer data
    accel_data = mpu.get_accel_data()
    print(f"accelerometer x : {accel_data['x']}")
    print(f"accelerometer y : {accel_data['y']}")
    print(f"accelerometer z : {accel_data['z']}")

    # get gyroscope data
    gyro_data = mpu.get_gyro_data()
    print(f"gyroscope x : {gyro_data['x']}")
    print(f"gyroscope y : {gyro_data['y']}")
    print(f"gyroscope z : {gyro_data['z']}")

    angle_xz, angle_yz = get_inclination(accel_data['x'], accel_data['y'], accel_data['z'])
    print('XZ angle = {:6.2f}deg   YZ angle = {:6.2f}deg'.format(angle_xz, angle_yz))
    time.sleep(1)
except KeyboardInterrupt:
  GPIO.cleanup()
