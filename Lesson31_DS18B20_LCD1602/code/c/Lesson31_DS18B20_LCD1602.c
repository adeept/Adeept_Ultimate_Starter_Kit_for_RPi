#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <lgpio.h>

#define BUFSIZE 128
#define PCF8574_ADDR 0x27
#define DEVICE_ROOT "/sys/bus/w1/devices/"
#define SENSOR_PREFIX "28-"
#define TEMP_FILE "w1_slave"

#define RS_BIT 0x01
#define RW_BIT 0x02
#define EN_BIT 0x04
#define BL_BIT 0x08

static int i2c_fd = -1;
static unsigned char backlight = BL_BIT;

void pcf8574_write(unsigned char data)
{
    lgI2cWriteByte(i2c_fd, data | backlight);
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

void lcd_cmd(unsigned char cmd)
{
    lcd_write4(cmd, 1);
}

void lcd_data(unsigned char data)
{
    lcd_write4(data, 0);
}

void lcd_position(int row, int col)
{
    unsigned char offsets[] = {0x00, 0x40};
    lcd_cmd(0x80 | (offsets[row] + col));
}

void lcd_print(const char *str)
{
    while (*str)
        lcd_data(*str++);
}

void lcd_init(void)
{
    lguSleep(0.05);
    lcd_enable(0x30);
    lguSleep(0.0045);
    lcd_enable(0x30);
    lguSleep(0.0045);
    lcd_enable(0x30);
    lguSleep(0.00015);
    lcd_enable(0x20);
    lguSleep(0.0001);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x01);
    lguSleep(0.002);
    lcd_cmd(0x06);
}

int get_ds18b20_path(char *sensor_path)
{
    DIR *dir;
    struct dirent *dir_entry;

    dir = opendir(DEVICE_ROOT);
    if (dir == NULL) {
        perror("Failed to open directory " DEVICE_ROOT);
        return -2;
    }

    while ((dir_entry = readdir(dir)) != NULL) {
        if (strncmp(dir_entry->d_name, SENSOR_PREFIX, strlen(SENSOR_PREFIX)) == 0) {
            snprintf(sensor_path, BUFSIZE, "%s%s/%s", DEVICE_ROOT, dir_entry->d_name, TEMP_FILE);
            closedir(dir);
            return 0;
        }
    }

    closedir(dir);
    fprintf(stderr, "No DS18B20 sensor detected\n");
    return -1;
}

int read_ds18b20(const char *sensor_path, float *temp)
{
    int fd;
    int ret;
    int i, j;
    char buf[BUFSIZE];
    char tempBuf[16] = {0};

    memset(buf, 0, sizeof(buf));

    fd = open(sensor_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open temperature data file");
        return -1;
    }

    while (1) {
        ret = read(fd, buf, sizeof(buf) - 1);

        if (ret == 0)
            break;

        if (ret == -1) {
            if (errno == EINTR)
                continue;

            perror("read()");
            close(fd);
            return -1;
        }

        buf[ret] = '\0';
        break;
    }

    close(fd);

    for (i = 0; i < ret; i++) {
        if (buf[i] == 't' && i + 1 < ret && buf[i + 1] == '=') {
            for (j = 0; j < (int)sizeof(tempBuf) - 1 && i + 2 + j < ret; j++) {
                if (buf[i + 2 + j] < '0' || buf[i + 2 + j] > '9')
                    break;

                tempBuf[j] = buf[i + 2 + j];
            }
            break;
        }
    }

    if (tempBuf[0] == '\0')
        return -1;

    *temp = (float)atoi(tempBuf) / 1000.0;
    return 0;
}

int main(void)
{
    float temp;
    int ret;
    int i2c_fd_result;
    char sensor_path[BUFSIZE];
    char lcdBuf[32];

    printf("DS18B20 + LCD1602 Test\n");

    ret = get_ds18b20_path(sensor_path);
    if (ret != 0) {
        fprintf(stderr, "Failed to get DS18B20 path\n");
        return 1;
    }

    printf("Detected DS18B20 path: %s\n", sensor_path);

    i2c_fd_result = lgI2cOpen(1, PCF8574_ADDR, 0);
    if (i2c_fd_result < 0) {
        printf("Failed to open I2C device\n");
        return 1;
    }

    i2c_fd = i2c_fd_result;
    backlight = BL_BIT;
    lcd_init();

    lcd_cmd(0x01);
    lguSleep(0.002);
    lcd_position(0, 0);
    lcd_print("DS18B20");

    while (1) {
        if (read_ds18b20(sensor_path, &temp) == 0) {
            printf("Temperature: %.3f C\n", temp);

            lcd_position(1, 0);
            snprintf(lcdBuf, sizeof(lcdBuf), "Temp:%.3f C", temp);
            lcd_print(lcdBuf);
        } else {
            printf("Failed to read temperature\n");

            lcd_position(1, 0);
            lcd_print("Temp: Error     ");
        }

        lguSleep(0.5);
    }

    lgI2cClose(i2c_fd);
    return 0;
}