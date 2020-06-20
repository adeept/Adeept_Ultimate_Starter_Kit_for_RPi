#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <math.h>

#define PI acos(-1)

// Scale Modifiers
#define ACCEL_SCALE_MODIFIER_2G 16384.0
#define ACCEL_SCALE_MODIFIER_4G 8192.0
#define ACCEL_SCALE_MODIFIER_8G 4096.0
#define ACCEL_SCALE_MODIFIER_16G 2048.0

#define GYRO_SCALE_MODIFIER_250DEG 131.0
#define GYRO_SCALE_MODIFIER_500DEG 65.5
#define GYRO_SCALE_MODIFIER_1000DEG 32.8
#define GYRO_SCALE_MODIFIER_2000DEG 16.4

// Pre-defined ranges
#define ACCEL_RANGE_2G 0x00
#define ACCEL_RANGE_4G 0x08
#define ACCEL_RANGE_8G 0x10
#define ACCEL_RANGE_16G 0x18

#define GYRO_RANGE_250DEG 0x00
#define GYRO_RANGE_500DEG 0x08
#define GYRO_RANGE_1000DEG 0x10
#define GYRO_RANGE_2000DEG 0x18

// MPU-6050 Registers
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C

#define ACCEL_XOUT0 0x3B
#define ACCEL_YOUT0 0x3D
#define ACCEL_ZOUT0 0x3F

#define TEMP_OUT0 0x41

#define GYRO_XOUT0 0x43
#define GYRO_YOUT0 0x45
#define GYRO_ZOUT0 0x47

#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG 0x1B

#define MPU6050_ADDRESS 0x68

// Read two i2c registers and combine them.
// register -- the first register to read from.
// Returns the combined read results.
int readI2CWord(int fd, int reg) {
  // Read the data from the registers
  int high = wiringPiI2CReadReg8(fd, reg);
  int low = wiringPiI2CReadReg8(fd, reg + 1);
  int value = (high << 8) + low;

  if (value >= 0x8000)
    return -((65535 - value) + 1);
  else
    return value; 
}

// Reads the temperature from the onboard temperature sensor of the MPU-6050.
// Returns the temperature in degrees Celcius.
float getTemp(int fd) {
  float raw_temp = readI2CWord(fd, TEMP_OUT0);
  // Get the actual temperature using the formule given in the
  // MPU-6050 Register Map and Descriptions revision 4.2, page 30
  float actual_temp = (raw_temp / 340.0) + 36.53;
  return actual_temp;
}

// Sets the range of the accelerometer to range.
// accel_range -- the range to set the accelerometer to. Using a
// pre-defined range is advised.
void setAccelRange(int fd, int accel_range) {
  // First change it to 0x00 to make sure we write the correct value later
  wiringPiI2CWriteReg8(fd, ACCEL_CONFIG, 0x00);
  // Write the new range to the ACCEL_CONFIG register
  wiringPiI2CWriteReg8(fd, ACCEL_CONFIG, accel_range);
}

// Reads the range the accelerometer is set to.
int readAccelRange(int fd) {
  return wiringPiI2CReadReg8(fd, ACCEL_CONFIG);
}

// Gets the X, Y and Z values from the accelerometer.
void getAccelData(int fd, float data[]) {
  float x = (float)readI2CWord(fd, ACCEL_XOUT0);
  float y = (float)readI2CWord(fd, ACCEL_YOUT0);
  float z = (float)readI2CWord(fd, ACCEL_ZOUT0);
  
  int accel_scale_modifier;
  int accel_range = readAccelRange(fd);
  
  if (accel_range == ACCEL_RANGE_2G)
    accel_scale_modifier = ACCEL_SCALE_MODIFIER_2G;
  else if (accel_range == ACCEL_RANGE_4G)
    accel_scale_modifier = ACCEL_SCALE_MODIFIER_4G;
  else if (accel_range == ACCEL_RANGE_8G)
    accel_scale_modifier = ACCEL_SCALE_MODIFIER_8G;
  else if (accel_range == ACCEL_RANGE_16G)
    accel_scale_modifier = ACCEL_SCALE_MODIFIER_16G;
  else
    printf("Unkown range - accel_scale_modifier set to ACCEL_SCALE_MODIFIER_2G\n");
    accel_scale_modifier = ACCEL_SCALE_MODIFIER_2G;
  
  x /= accel_scale_modifier;
  y /= accel_scale_modifier;
  z /= accel_scale_modifier;

  data[0] = x;
  data[1] = y;
  data[2] = z;
}

// Sets the range of the gyroscope to range.
// gyro_range -- the range to set the gyroscope to. Using a pre-defined
// range is advised.
void setGyroRange(int fd, int gyro_range) {
  // First change it to 0x00 to make sure we write the correct value later
  wiringPiI2CWriteReg8(fd, ACCEL_CONFIG, 0x00);
  // Write the new range to the GYRO_CONFIG register
  wiringPiI2CWriteReg8(fd, GYRO_CONFIG, gyro_range);
}

// Reads the range the gyroscope is set to.
int readGyroRange(int fd) {
  return wiringPiI2CReadReg8(fd, GYRO_CONFIG);
}

// Gets the X, Y and Z values from the gyroscope.
void getGyroData(int fd, float data[]) {
  float x = (float)readI2CWord(fd, GYRO_XOUT0);
  float y = (float)readI2CWord(fd, GYRO_YOUT0);
  float z = (float)readI2CWord(fd, GYRO_ZOUT0);

  int gyro_scale_modifier;
  int gyro_range = readGyroRange(fd);

  if (gyro_range == GYRO_RANGE_250DEG)
    gyro_scale_modifier = GYRO_SCALE_MODIFIER_250DEG;
  else if (gyro_range == GYRO_RANGE_500DEG)
    gyro_scale_modifier = GYRO_SCALE_MODIFIER_500DEG;
  else if (gyro_range == GYRO_RANGE_1000DEG)
    gyro_scale_modifier = GYRO_SCALE_MODIFIER_1000DEG;
  else if (gyro_range == GYRO_RANGE_2000DEG)
    gyro_scale_modifier = GYRO_SCALE_MODIFIER_2000DEG;
  else
    printf("Unkown range - gyro_scale_modifier set to GYRO_SCALE_MODIFIER_250DEG\n");
    gyro_scale_modifier = GYRO_SCALE_MODIFIER_250DEG;
  
  x /= gyro_scale_modifier;
  y /= gyro_scale_modifier;
  z /= gyro_scale_modifier;

  data[0] = x;
  data[1] = y;
  data[2] = z;
}

// Given a point (x, y) return the angle of that point relative to x axis.
// Returns: angle in degrees
float vector_2_degrees(float x, float y) {
  float angle = ((atan2(y, x) / 180) * PI);
  if (angle < 0)
    angle += 360;
  return angle;
}

float get_angle_xz(float x, float z) {
  return vector_2_degrees(x, z);
}

float get_angle_yz(float y, float z) {
  return vector_2_degrees(y, z);
}


int main() {
  // Initialize MPU6050
  wiringPiSetup();
  int fd = wiringPiI2CSetup(MPU6050_ADDRESS);
  if (fd >= 0) {
    printf("fd = %d\n", fd);
    // Turn off sleep mode
    wiringPiI2CWriteReg8(fd, PWR_MGMT_1, 0x00);
  } else {
    printf("Initialize MPU6050 Error");
  }

  float temp = getTemp(fd);   // temperature
  float accel_data[3];        // accelerometer
  float gyro_data[3];         // gyroscope

  getAccelData(fd, accel_data);
  getGyroData(fd, gyro_data);
  
  while(1) {
    printf("temp: %f\n", temp);
    printf("accelerometer x: %f\n", accel_data[0]);
    printf("accelerometer y: %f\n", accel_data[1]);
    printf("accelerometer z: %f\n", accel_data[2]);
    printf("gyroscope x: %f\n", gyro_data[0]);
    printf("gyroscope y: %f\n", gyro_data[1]);
    printf("gyroscope z: %f\n", gyro_data[2]);
    printf("angle_xz: %6.2f\n", get_angle_xz(accel_data[0], accel_data[2]));
    printf("angle_yz: %6.2f\n", get_angle_xz(accel_data[1], accel_data[2]));
    delay(1000);
  }

  return 0;
}