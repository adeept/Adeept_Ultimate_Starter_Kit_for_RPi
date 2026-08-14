#include <stdlib.h>
#include <stdio.h>
#include <lgpio.h>
#include <time.h>

#define PCF8574_ADDR  0x27  // Default I2C address of PCF8574

// PCF8574 pin mapping to LCD1602
#define RS_BIT   0x01  // P0 - Register Select
#define RW_BIT   0x02  // P1 - Read/Write
#define EN_BIT   0x04  // P2 - Enable
#define BL_BIT   0x08  // P3 - Backlight LED
#define D4_BIT   0x10  // P4 - Data bit 4
#define D5_BIT   0x20  // P5 - Data bit 5
#define D6_BIT   0x40  // P6 - Data bit 6
#define D7_BIT   0x80  // P7 - Data bit 7

static int i2c_fd = -1;
static unsigned char backlight = BL_BIT;

// Write a byte to PCF8574 via I2C
void pcf8574_write(unsigned char data)
{
    lgI2cWriteByte(i2c_fd, data | backlight);
}

// Pulse EN pin to latch data
void lcd_enable(unsigned char data)
{
    pcf8574_write(data | EN_BIT);
    lguSleep(0.000001);  // 1us enable pulse
    pcf8574_write(data & ~EN_BIT);
    lguSleep(0.00005);   // 50us for command execution
}

// Send 4-bit data to LCD
void lcd_write4(unsigned char data, unsigned char is_cmd)
{
    unsigned char high = (data & 0xF0) | (is_cmd ? 0 : RS_BIT);
    unsigned char low = ((data << 4) & 0xF0) | (is_cmd ? 0 : RS_BIT);
    lcd_enable(high);
    lcd_enable(low);
}

// Send command to LCD
void lcd_cmd(unsigned char cmd)
{
    lcd_write4(cmd, 1);
}

// Send data (character) to LCD
void lcd_data(unsigned char data)
{
    lcd_write4(data, 0);
}

// Set cursor position (row 0-1, col 0-15)
void lcd_position(int row, int col)
{
    unsigned char offsets[] = {0x00, 0x40};
    lcd_cmd(0x80 | (offsets[row] + col));
}

// Print a string at current cursor position
void lcd_print(const char *str)
{
    while (*str) {
        lcd_data(*str++);
    }
}

// Initialize LCD1602 in 4-bit mode
void lcd_init(void)
{
    lguSleep(0.05);  // Wait 50ms after power on

    // 8-bit mode initialization sequence (3 times)
    lcd_enable(0x30);
    lguSleep(0.0045);  // 4.5ms
    lcd_enable(0x30);
    lguSleep(0.0045);
    lcd_enable(0x30);
    lguSleep(0.00015);  // 150us

    // Switch to 4-bit mode
    lcd_enable(0x20);
    lguSleep(0.0001);

    // Function set: 4-bit, 2 lines, 5x8 dots
    lcd_cmd(0x28);
    // Display on, cursor off, blink off
    lcd_cmd(0x0C);
    // Clear display
    lcd_cmd(0x01);
    lguSleep(0.002);
    // Entry mode: increment, no shift
    lcd_cmd(0x06);
}

void printCPUTemperature()
{
    FILE *fp;
    char str_temp[15];
    float CPU_temp;
    char buf[32];

    fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (!fp) return;
    fgets(str_temp, 15, fp);
    CPU_temp = atof(str_temp) / 1000.0;
    fclose(fp);

    printf("CPU's temperature : %.2f \n", CPU_temp);
    lcd_position(0, 0);
    snprintf(buf, sizeof(buf), "CPU:%.2fC", CPU_temp);
    lcd_print(buf);
}

void printDataTime()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buf[32];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("%s \n", asctime(timeinfo));

    lcd_position(1, 0);
    snprintf(buf, sizeof(buf), "Time:%02d:%02d:%02d",
             timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    lcd_print(buf);
}

int main(void)
{
    printf("Program is starting ...\n");

    i2c_fd = lgI2cOpen(1, PCF8574_ADDR, 0);
    if (i2c_fd < 0) {
        printf("Failed to open I2C device\n");
        return 1;
    }

    backlight = BL_BIT;  // Turn on backlight
    lcd_init();

    while (1) {
        printCPUTemperature();
        printDataTime();
        lguSleep(1.0);
    }

    lgI2cClose(i2c_fd);
    return 0;
}
