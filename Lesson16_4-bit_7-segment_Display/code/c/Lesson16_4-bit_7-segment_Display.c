#include <lgpio.h>
#include <stdio.h>

// Segment data pins: BCM 17,18,27,22,23,24,25,4 (wiringPi 0-7)
const int segPins[] = {17, 18, 27, 22, 23, 24, 25, 4};

// Digit select pins: BCM 2,3,8,7 
#define BIT0  2   // wiringPi 8
#define BIT1  3   // wiringPi 9
#define BIT2  8   // wiringPi 10
#define BIT3  7   // wiringPi 11

const unsigned char SegCode[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char DatBuf[4] = {0, 0, 0, 0};
int cnt = 0;

// Write 8-bit segment data to pins
void writeByte(int h, unsigned char data)
{
    int i;
    for (i = 0; i < 8; i++)
        lgGpioWrite(h, segPins[i], (data >> i) & 0x01);
}

void sysInit(int h)
{
    int i;
    for (i = 0; i < 8; i++)
        lgGpioClaimOutput(h, 0, segPins[i], 1);
    lgGpioClaimOutput(h, 0, BIT0, 1);
    lgGpioClaimOutput(h, 0, BIT1, 1);
    lgGpioClaimOutput(h, 0, BIT2, 1);
    lgGpioClaimOutput(h, 0, BIT3, 1);
}

void do_cnt(void)
{
    DatBuf[0] = SegCode[cnt % 10];
    DatBuf[1] = SegCode[cnt % 100 / 10];
    DatBuf[2] = SegCode[cnt % 1000 / 100];
    DatBuf[3] = SegCode[cnt / 1000];
}

void display(int h)
{
    int i, j;
    for (i = 0; i < 100; i++) {
        lgGpioWrite(h, BIT0, 0); lgGpioWrite(h, BIT1, 1);
        lgGpioWrite(h, BIT2, 1); lgGpioWrite(h, BIT3, 1);
        writeByte(h, DatBuf[0]);
        lguSleep(0.001);

        lgGpioWrite(h, BIT0, 1); lgGpioWrite(h, BIT1, 0);
        lgGpioWrite(h, BIT2, 1); lgGpioWrite(h, BIT3, 1);
        writeByte(h, DatBuf[1]);
        lguSleep(0.001);

        lgGpioWrite(h, BIT0, 1); lgGpioWrite(h, BIT1, 1);
        lgGpioWrite(h, BIT2, 0); lgGpioWrite(h, BIT3, 1);
        writeByte(h, DatBuf[2]);
        lguSleep(0.001);

        lgGpioWrite(h, BIT0, 1); lgGpioWrite(h, BIT1, 1);
        lgGpioWrite(h, BIT2, 1); lgGpioWrite(h, BIT3, 0);
        writeByte(h, DatBuf[3]);
        lguSleep(0.001);
    }
    cnt++;
    if (cnt == 10000) cnt = 0;
}

int main(void)
{
    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    sysInit(h);

    while (1) {
        do_cnt();
        display(h);
    }

    lgGpiochipClose(h);
    return 0;
}
