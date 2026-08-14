#include <lgpio.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/resource.h>

#define DHT_PIN 4
#define LED_RED 17
#define LED_GREEN 18
#define LED_BLUE 27
#define DHTLIB_OK 0
#define DHTLIB_ERROR_CHECKSUM -1
#define DHTLIB_ERROR_TIMEOUT -2
#define DHTLIB_INVALID_VALUE -999
#define DHTLIB_TIMEOUT 2000
#define DHTLIB_DHT11_WAKEUP 20

static int gpioHandle;
static uint8_t bits[5];
static double humidity, temperature;

void sigint_handler(int sig)
{
    (void)sig;
    lgGpiochipClose(gpioHandle);
    printf("\nExit, GPIO chip closed\n");
    _exit(0);
}

uint32_t micros(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)ts.tv_sec * 1000000 + (uint32_t)ts.tv_nsec / 1000;
}

void delay_ms(int ms)
{
    usleep((useconds_t)ms * 1000);
}

int gpioSetOutput(int pin, int level)
{
    lgGpioFree(gpioHandle, pin);
    return lgGpioClaimOutput(gpioHandle, 0, pin, level);
}

int gpioSetInputPullup(int pin)
{
    lgGpioFree(gpioHandle, pin);
    return lgGpioClaimInput(gpioHandle, LG_SET_PULL_UP, pin);
}

int readSensor(int pin, int wakeupDelay)
{
    int mask = 0x80;
    int idx = 0;
    int i;
    uint32_t t;

    for (i = 0; i < 5; i++)
        bits[i] = 0;

    gpioSetOutput(pin, 0);
    delay_ms(wakeupDelay);

    lgGpioWrite(gpioHandle, pin, 1);
    usleep(10);
    gpioSetInputPullup(pin);

    t = micros();
    while (lgGpioRead(gpioHandle, pin)) {
        if (micros() - t > DHTLIB_TIMEOUT)
            return DHTLIB_ERROR_TIMEOUT;
    }

    t = micros();
    while (!lgGpioRead(gpioHandle, pin)) {
        if (micros() - t > DHTLIB_TIMEOUT)
            return DHTLIB_ERROR_TIMEOUT;
    }

    t = micros();
    while (lgGpioRead(gpioHandle, pin)) {
        if (micros() - t > DHTLIB_TIMEOUT)
            return DHTLIB_ERROR_TIMEOUT;
    }

    for (i = 0; i < 40; i++) {
        t = micros();

        while (!lgGpioRead(gpioHandle, pin)) {
            if (micros() - t > DHTLIB_TIMEOUT)
                return DHTLIB_ERROR_TIMEOUT;
        }

        t = micros();

        while (lgGpioRead(gpioHandle, pin)) {
            if (micros() - t > DHTLIB_TIMEOUT)
                return DHTLIB_ERROR_TIMEOUT;
        }

        if (micros() - t > 60)
            bits[idx] |= mask;

        mask >>= 1;

        if (mask == 0) {
            mask = 0x80;
            idx++;
        }
    }

    gpioSetOutput(pin, 1);
    return DHTLIB_OK;
}

int readDHT11Once(int pin)
{
    int rv;
    uint8_t sum;

    rv = readSensor(pin, DHTLIB_DHT11_WAKEUP);

    if (rv != DHTLIB_OK) {
        humidity = DHTLIB_INVALID_VALUE;
        temperature = DHTLIB_INVALID_VALUE;
        return rv;
    }

    humidity = bits[0];
    temperature = bits[2] + bits[3] * 0.1;

    sum = bits[0] + bits[1] + bits[2] + bits[3];

    if (bits[4] != sum)
        return DHTLIB_ERROR_CHECKSUM;

    return DHTLIB_OK;
}

int readDHT11(int pin)
{
    int chk = DHTLIB_INVALID_VALUE;

    for (int i = 0; i < 15; i++) {
        chk = readDHT11Once(pin);

        if (chk == DHTLIB_OK)
            return DHTLIB_OK;

        delay_ms(100);
    }

    return chk;
}

int mapValue(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ledInit(int h)
{
    lgGpioClaimOutput(h, 0, LED_RED, 1);
    lgGpioClaimOutput(h, 0, LED_GREEN, 1);
    lgGpioClaimOutput(h, 0, LED_BLUE, 1);
}

void ledColorSet(int h, int color)
{
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;

    r = mapValue(r, 0, 255, 0, 100);
    g = mapValue(g, 0, 255, 0, 100);
    b = mapValue(b, 0, 255, 0, 100);

    r = 100 - r;
    g = 100 - g;
    b = 100 - b;

    lgTxPwm(h, LED_RED, 1000, r, 0, 0);
    lgTxPwm(h, LED_GREEN, 1000, g, 0, 0);
    lgTxPwm(h, LED_BLUE, 1000, b, 0, 0);
}

int main(void)
{
    int chk;
    int count = 0;
    int temperature_int;
    int humidity_int;
    int color;

    printf("Raspberry Pi lgpio DHT11 + RGB LED test program\n");
    printf("DHT11 = BCM%d\n", DHT_PIN);
    printf("RGB LED = R:%d G:%d B:%d\n", LED_RED, LED_GREEN, LED_BLUE);

    gpioHandle = lgGpiochipOpen(4);

    if (gpioHandle < 0) {
        printf("gpiochip4 not detected, fallback to gpiochip0\n");
        gpioHandle = lgGpiochipOpen(0);

        if (gpioHandle < 0) {
            printf("Failed to open GPIO chip\n");
            return -1;
        }
    }

    setpriority(PRIO_PROCESS, 0, -10);
    signal(SIGINT, sigint_handler);

    ledInit(gpioHandle);

    while (1) {
        count++;
        printf("Measurement counts: %d\n", count);

        chk = readDHT11(DHT_PIN);

        if (chk == DHTLIB_OK) {
            temperature_int = (int)temperature;
            humidity_int = (int)humidity;

            printf("Humidity: %.2f %%\tTemperature: %.2f *C\n", humidity, temperature);

            color = ((temperature_int + humidity_int) & 0xFF) << 16;
            ledColorSet(gpioHandle, color);
        } else if (chk == DHTLIB_ERROR_CHECKSUM) {
            printf("Checksum error\n");
        } else {
            printf("Data not good, skip\n");
        }

        delay_ms(2000);
    }

    lgGpiochipClose(gpioHandle);
    return 0;
}