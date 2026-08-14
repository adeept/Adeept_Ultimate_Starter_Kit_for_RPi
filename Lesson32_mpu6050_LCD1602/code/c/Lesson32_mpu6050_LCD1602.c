#include <lgpio.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PCF8574_ADDR  0x27
#define MPU6050_ADDR  0x68

#define RS_BIT  0x01
#define RW_BIT  0x02
#define EN_BIT  0x04
#define BL_BIT  0x08

#define PI acos(-1)

#define ACCEL_SCALE_2G   16384.0
#define ACCEL_SCALE_4G   8192.0
#define ACCEL_SCALE_8G   4096.0
#define ACCEL_SCALE_16G  2048.0

#define ACCEL_RANGE_2G   0x00
#define ACCEL_RANGE_4G   0x08
#define ACCEL_RANGE_8G   0x10
#define ACCEL_RANGE_16G  0x18

#define PWR_MGMT_1    0x6B
#define ACCEL_XOUT0   0x3B
#define ACCEL_YOUT0   0x3D
#define ACCEL_ZOUT0   0x3F
#define ACCEL_CONFIG  0x1C

static int lcd_fd = -1;
static unsigned char backlight = BL_BIT;

void pcf8574_write(unsigned char data)
{
    lgI2cWriteByte(lcd_fd, data | backlight);
}

void lcd_enable(unsigned char data)
{
    pcf8574_write(data | EN_BIT);
    lguSleep(0.000001);
    pcf8574_write(data & ~EN_BIT);
    lguSleep(0.00005);
}

void lcd_write4(unsigned char data, unsigned char is_cmd)
{
    unsigned char high = (data & 0xF0) | (is_cmd ? 0 : RS_BIT);
    unsigned char low = ((data << 4) & 0xF0) | (is_cmd ? 0 : RS_BIT);
    lcd_enable(high);
    lcd_enable(low);
}

void lcd_cmd(unsigned char cmd) { lcd_write4(cmd, 1); }
void lcd_data(unsigned char data) { lcd_write4(data, 0); }

void lcd_position(int row, int col)
{
    unsigned char offsets[] = {0x00, 0x40};
    lcd_cmd(0x80 | (offsets[row] + col));
}

void lcd_print(const char *str)
{
    while (*str) lcd_data(*str++);
}

void lcd_init(void)
{
    lguSleep(0.05);
    lcd_enable(0x30); lguSleep(0.0045);
    lcd_enable(0x30); lguSleep(0.0045);
    lcd_enable(0x30); lguSleep(0.00015);
    lcd_enable(0x20); lguSleep(0.0001);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x01); lguSleep(0.002);
    lcd_cmd(0x06);
}

int readI2CWord(int fd, int reg)
{
    int high = lgI2cReadByteData(fd, reg);
    int low = lgI2cReadByteData(fd, reg + 1);
    int value = (high << 8) + low;
    return (value >= 0x8000) ? -((65535 - value) + 1) : value;
}

void getAccelData(int fd, float data[])
{
    float x = (float)readI2CWord(fd, ACCEL_XOUT0);
    float y = (float)readI2CWord(fd, ACCEL_YOUT0);
    float z = (float)readI2CWord(fd, ACCEL_ZOUT0);
    data[0] = x / ACCEL_SCALE_2G;
    data[1] = y / ACCEL_SCALE_2G;
    data[2] = z / ACCEL_SCALE_2G;
}

float vector_2_degrees(float x, float y)
{
    float angle = (atan2(y, x) * 180.0) / PI;
    if (angle < 0) angle += 360;
    return angle;
}

float get_angle_xz(float x, float z) { return vector_2_degrees(x, z); }
float get_angle_yz(float y, float z) { return vector_2_degrees(y, z); }

int main()
{
    int mpu_fd = lgI2cOpen(1, MPU6050_ADDR, 0);
    if (mpu_fd >= 0) {
        lgI2cWriteByteData(mpu_fd, PWR_MGMT_1, 0x00);  // Wake up
    } else {
        printf("Initialize MPU6050 Error\n");
        return 1;
    }

    lcd_fd = lgI2cOpen(1, PCF8574_ADDR, 0);
    if (lcd_fd < 0) {
        printf("lcdInit failed!\n");
        return 1;
    }
    backlight = BL_BIT;
    lcd_init();

    float accel_data[3];
    float tmp_xz = 0, tmp_yz = 0;
    float angle_xz, angle_yz;
    int step_count = 0;
    char buf[32];

    while (1) {
        getAccelData(mpu_fd, accel_data);
        angle_xz = get_angle_xz(accel_data[0], accel_data[2]);
        angle_yz = get_angle_yz(accel_data[1], accel_data[2]);

        if ((fabs(angle_xz - tmp_xz) > 2) && (fabs(angle_yz - tmp_yz) > 2)) {
            step_count++;
            tmp_xz = angle_xz;
            tmp_yz = angle_yz;
        }

        lcd_position(1, 0);
        snprintf(buf, sizeof(buf), "step count: %d", step_count);
        lcd_print(buf);

        lguSleep(1.0);
    }

    lgI2cClose(mpu_fd);
    lgI2cClose(lcd_fd);
    return 0;
}
