#include <lgpio.h>
#include <stdio.h>
#include <math.h>

#define PI acos(-1)

// Scale modifiers
#define ACCEL_SCALE_MODIFIER_2G   16384.0
#define ACCEL_SCALE_MODIFIER_4G   8192.0
#define ACCEL_SCALE_MODIFIER_8G   4096.0
#define ACCEL_SCALE_MODIFIER_16G  2048.0

#define GYRO_SCALE_MODIFIER_250DEG   131.0
#define GYRO_SCALE_MODIFIER_500DEG   65.5
#define GYRO_SCALE_MODIFIER_1000DEG  32.8
#define GYRO_SCALE_MODIFIER_2000DEG  16.4

// Pre-defined ranges
#define ACCEL_RANGE_2G   0x00
#define ACCEL_RANGE_4G   0x08
#define ACCEL_RANGE_8G   0x10
#define ACCEL_RANGE_16G  0x18

#define GYRO_RANGE_250DEG   0x00
#define GYRO_RANGE_500DEG   0x08
#define GYRO_RANGE_1000DEG  0x10
#define GYRO_RANGE_2000DEG  0x18

// MPU-6050 registers
#define PWR_MGMT_1    0x6B
#define PWR_MGMT_2    0x6C
#define ACCEL_XOUT0   0x3B
#define ACCEL_YOUT0   0x3D
#define ACCEL_ZOUT0   0x3F
#define TEMP_OUT0     0x41
#define GYRO_XOUT0    0x43
#define GYRO_YOUT0    0x45
#define GYRO_ZOUT0    0x47
#define ACCEL_CONFIG  0x1C
#define GYRO_CONFIG   0x1B

#define MPU6050_ADDR  0x68

// Read two I2C registers and combine them into a 16-bit value
int readI2CWord(int fd, int reg)
{
    int high = lgI2cReadByteData(fd, reg);
    int low = lgI2cReadByteData(fd, reg + 1);
    int value = (high << 8) + low;
    return (value >= 0x8000) ? -((65535 - value) + 1) : value;
}

// Read temperature from MPU-6050 onboard sensor
float getTemp(int fd)
{
    float raw_temp = readI2CWord(fd, TEMP_OUT0);
    return (raw_temp / 340.0) + 36.53;
}

// Set accelerometer range
void setAccelRange(int fd, int accel_range)
{
    lgI2cWriteByteData(fd, ACCEL_CONFIG, 0x00);
    lgI2cWriteByteData(fd, ACCEL_CONFIG, accel_range);
}

// Read current accelerometer range
int readAccelRange(int fd)
{
    return lgI2cReadByteData(fd, ACCEL_CONFIG);
}

// Get X, Y, Z accelerometer data
void getAccelData(int fd, float data[])
{
    float x = (float)readI2CWord(fd, ACCEL_XOUT0);
    float y = (float)readI2CWord(fd, ACCEL_YOUT0);
    float z = (float)readI2CWord(fd, ACCEL_ZOUT0);

    int range = readAccelRange(fd);
    float scale;

    if (range == ACCEL_RANGE_2G)        scale = ACCEL_SCALE_MODIFIER_2G;
    else if (range == ACCEL_RANGE_4G)   scale = ACCEL_SCALE_MODIFIER_4G;
    else if (range == ACCEL_RANGE_8G)   scale = ACCEL_SCALE_MODIFIER_8G;
    else if (range == ACCEL_RANGE_16G)  scale = ACCEL_SCALE_MODIFIER_16G;
    else { printf("Unknown accel range, defaulting to 2G\n"); scale = ACCEL_SCALE_MODIFIER_2G; }

    data[0] = x / scale;
    data[1] = y / scale;
    data[2] = z / scale;
}

// Set gyroscope range
void setGyroRange(int fd, int gyro_range)
{
    lgI2cWriteByteData(fd, GYRO_CONFIG, 0x00);
    lgI2cWriteByteData(fd, GYRO_CONFIG, gyro_range);
}

// Read current gyroscope range
int readGyroRange(int fd)
{
    return lgI2cReadByteData(fd, GYRO_CONFIG);
}

// Get X, Y, Z gyroscope data
void getGyroData(int fd, float data[])
{
    float x = (float)readI2CWord(fd, GYRO_XOUT0);
    float y = (float)readI2CWord(fd, GYRO_YOUT0);
    float z = (float)readI2CWord(fd, GYRO_ZOUT0);

    int range = readGyroRange(fd);
    float scale;

    if (range == GYRO_RANGE_250DEG)        scale = GYRO_SCALE_MODIFIER_250DEG;
    else if (range == GYRO_RANGE_500DEG)   scale = GYRO_SCALE_MODIFIER_500DEG;
    else if (range == GYRO_RANGE_1000DEG)  scale = GYRO_SCALE_MODIFIER_1000DEG;
    else if (range == GYRO_RANGE_2000DEG)  scale = GYRO_SCALE_MODIFIER_2000DEG;
    else { printf("Unknown gyro range, defaulting to 250deg\n"); scale = GYRO_SCALE_MODIFIER_250DEG; }

    data[0] = x / scale;
    data[1] = y / scale;
    data[2] = z / scale;
}

// Calculate angle in degrees from (x, y) vector relative to x axis
float vector_2_degrees(float x, float y)
{
    float angle = atan2(y, x) * 180.0 / PI;
    if (angle < 0) angle += 360;
    return angle;
}

float get_angle_xz(float x, float z) { return vector_2_degrees(x, z); }
float get_angle_yz(float y, float z) { return vector_2_degrees(y, z); }

int main()
{
    int fd = lgI2cOpen(1, MPU6050_ADDR, 0);
    if (fd >= 0) {
        printf("fd = %d\n", fd);
        lgI2cWriteByteData(fd, PWR_MGMT_1, 0x00);  // Wake up from sleep
    } else {
        printf("Initialize MPU6050 Error\n");
        return -1;
    }

    float temp;
    float accel_data[3];
    float gyro_data[3];

    while (1) {
        temp = getTemp(fd);
        getAccelData(fd, accel_data);
        getGyroData(fd, gyro_data);

        printf("temp: %f\n", temp);
        printf("accelerometer x: %f\n", accel_data[0]);
        printf("accelerometer y: %f\n", accel_data[1]);
        printf("accelerometer z: %f\n", accel_data[2]);
        printf("gyroscope x: %f\n", gyro_data[0]);
        printf("gyroscope y: %f\n", gyro_data[1]);
        printf("gyroscope z: %f\n", gyro_data[2]);
        printf("angle_xz: %6.2f\n", get_angle_xz(accel_data[0], accel_data[2]));
        printf("angle_yz: %6.2f\n", get_angle_yz(accel_data[1], accel_data[2]));
        lguSleep(1.0);
    }

    lgI2cClose(fd);
    return 0;
}
