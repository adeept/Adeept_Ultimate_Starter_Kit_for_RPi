#include <lgpio.h>
#include <stdio.h>

// BCM pins: rows=17,18,27,22 (wiringPi 0-3), cols=23,24,25,4 (wiringPi 4-7)
const int ROW[]    = {17, 18, 27, 22};
const int COLUMN[] = {23, 24, 25, 4};

int getKey(int h)
{
    int i, tmpRead, rowVal = -1, colVal = -1;
    char keyVal;

    // Set all columns as output low
    for (i = 0; i < 4; i++) {
        lgGpioClaimOutput(h, 0, COLUMN[i], 0);
    }
    // Set all rows as input with pull-up
    for (i = 0; i < 4; i++) {
        lgGpioClaimInput(h, LG_SET_PULL_UP, ROW[i]);
    }

    // Detect which row is pressed
    for (i = 0; i < 4; i++) {
        tmpRead = lgGpioRead(h, ROW[i]);
        if (tmpRead == 0) rowVal = i;
    }
    if (rowVal < 0 || rowVal > 3) return -1;

    // Switch columns to input pull-up, pressed row to output low
    for (i = 0; i < 4; i++) {
        lgGpioClaimInput(h, LG_SET_PULL_UP, COLUMN[i]);
    }
    lgGpioClaimOutput(h, 0, ROW[rowVal], 0);

    // Detect which column is pressed
    for (i = 0; i < 4; i++) {
        tmpRead = lgGpioRead(h, COLUMN[i]);
        if (tmpRead == 0) colVal = i;
    }
    if (colVal < 0 || colVal > 3) return -1;

    // Map row/col to key value
    switch (rowVal) {
        case 0: switch (colVal) {
            case 0: keyVal = 0; break; case 1: keyVal = 1; break;
            case 2: keyVal = 2; break; case 3: keyVal = 3; break;
        } break;
        case 1: switch (colVal) {
            case 0: keyVal = 4; break; case 1: keyVal = 5; break;
            case 2: keyVal = 6; break; case 3: keyVal = 7; break;
        } break;
        case 2: switch (colVal) {
            case 0: keyVal = 8; break; case 1: keyVal = 9; break;
            case 2: keyVal = 10; break; case 3: keyVal = 11; break;
        } break;
        case 3: switch (colVal) {
            case 0: keyVal = 12; break; case 1: keyVal = 13; break;
            case 2: keyVal = 14; break; case 3: keyVal = 15; break;
        } break;
    }
    return keyVal;
}

int main(void)
{
    int key = -1;

    int h = lgGpiochipOpen(4);
    if (h < 0) {
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }

    while (1) {
        key = getKey(h);
        if (key != -1) {
            switch (key) {
                case 0: printf("1\n"); break; case 1: printf("2\n"); break;
                case 2: printf("3\n"); break; case 3: printf("A\n"); break;
                case 4: printf("4\n"); break; case 5: printf("5\n"); break;
                case 6: printf("6\n"); break; case 7: printf("B\n"); break;
                case 8: printf("7\n"); break; case 9: printf("8\n"); break;
                case 10: printf("9\n"); break; case 11: printf("C\n"); break;
                case 12: printf("*\n"); break; case 13: printf("0\n"); break;
                case 14: printf("#\n"); break; case 15: printf("D\n"); break;
            }
        }
        lguSleep(0.2);
    }

    lgGpiochipClose(h);
    return 0;
}
