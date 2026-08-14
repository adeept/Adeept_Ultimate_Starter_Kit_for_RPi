#include <stdio.h>
#include <lgpio.h>

typedef unsigned char uchar;

// Segment codes for 0-9, A-F, g
const uchar SegCode[17] = {0xbf,0x86,0xdb,0xcf,0xE6,0xed,0xfd,0x87,0xff,0xef,0xdf,0xfc,0xd8,0xfb,0xf1,0xef,0xf4};

// BCM pins: 17,18,22,27,23,24,25,4,2 
const uchar segLedPins[] = {17, 18, 22, 27, 23, 24, 25, 4};

int readBit(uchar a, int b) { return (a >> b) & 0x01; }

int main(void)
{
    int i, j;

    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    for (i = 0; i < 8; i++)
        lgGpioClaimOutput(h, 0, segLedPins[i], 0);

    while (1) {
        for (j = 0; j < 17; j++) {
            for (i = 0; i < 8; i++)
                lgGpioWrite(h, segLedPins[i], readBit(SegCode[j], i));
            lguSleep(0.5);
        }
    }

    lgGpiochipClose(h);
    return 0;
}
