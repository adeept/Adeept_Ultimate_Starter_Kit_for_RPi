#include <lgpio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>
#include "dht11.h"

#define DHTLIB_TIMEOUT 2000
#define DHTLIB_DHT11_WAKEUP 20

static int gpioHandle=-1;
static uint8_t bits[5];

static uint32_t micros(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    return (uint32_t)ts.tv_sec*1000000+(uint32_t)ts.tv_nsec/1000;
}

static void delay_ms(int ms)
{
    usleep((useconds_t)ms*1000);
}

static int gpioSetOutput(int pin,int level)
{
    lgGpioFree(gpioHandle,pin);
    return lgGpioClaimOutput(gpioHandle,0,pin,level);
}

static int gpioSetInputPullup(int pin)
{
    lgGpioFree(gpioHandle,pin);
    return lgGpioClaimInput(gpioHandle,LG_SET_PULL_UP,pin);
}

static int readSensor(int pin,int wakeupDelay)
{
    int mask=0x80;
    int idx=0;
    int i;
    uint32_t t;

    for(i=0;i<5;i++) bits[i]=0;

    gpioSetOutput(pin,0);
    delay_ms(wakeupDelay);

    lgGpioWrite(gpioHandle,pin,1);
    usleep(10);
    gpioSetInputPullup(pin);

    t=micros();
    while(lgGpioRead(gpioHandle,pin)){
        if(micros()-t>DHTLIB_TIMEOUT){
            lgGpioFree(gpioHandle,pin);
            return DHTLIB_ERROR_TIMEOUT;
        }
    }

    t=micros();
    while(!lgGpioRead(gpioHandle,pin)){
        if(micros()-t>DHTLIB_TIMEOUT){
            lgGpioFree(gpioHandle,pin);
            return DHTLIB_ERROR_TIMEOUT;
        }
    }

    t=micros();
    while(lgGpioRead(gpioHandle,pin)){
        if(micros()-t>DHTLIB_TIMEOUT){
            lgGpioFree(gpioHandle,pin);
            return DHTLIB_ERROR_TIMEOUT;
        }
    }

    for(i=0;i<40;i++){
        t=micros();
        while(!lgGpioRead(gpioHandle,pin)){
            if(micros()-t>DHTLIB_TIMEOUT){
                lgGpioFree(gpioHandle,pin);
                return DHTLIB_ERROR_TIMEOUT;
            }
        }

        t=micros();
        while(lgGpioRead(gpioHandle,pin)){
            if(micros()-t>DHTLIB_TIMEOUT){
                lgGpioFree(gpioHandle,pin);
                return DHTLIB_ERROR_TIMEOUT;
            }
        }

        if(micros()-t>60) bits[idx]|=mask;

        mask>>=1;
        if(mask==0){
            mask=0x80;
            idx++;
        }
    }

    gpioSetOutput(pin,1);
    return DHTLIB_OK;
}

static int readDHT11Once(int pin,float *humidity,float *temperature)
{
    int rv;
    uint8_t sum;

    rv=readSensor(pin,DHTLIB_DHT11_WAKEUP);

    if(rv!=DHTLIB_OK){
        *humidity=DHTLIB_INVALID_VALUE;
        *temperature=DHTLIB_INVALID_VALUE;
        return rv;
    }

    *humidity=bits[0];
    *temperature=bits[2]+bits[3]*0.1f;

    sum=bits[0]+bits[1]+bits[2]+bits[3];

    if(bits[4]!=sum)
        return DHTLIB_ERROR_CHECKSUM;

    return DHTLIB_OK;
}

int dht11_init(void)
{
    gpioHandle=lgGpiochipOpen(4);

    if(gpioHandle<0)
        gpioHandle=lgGpiochipOpen(0);

    if(gpioHandle<0)
        return gpioHandle;

    setpriority(PRIO_PROCESS,0,-10);

    return DHTLIB_OK;
}

int dht11_read(int pin,float *humidity,float *temperature)
{
    int chk=DHTLIB_INVALID_VALUE;
    int i;

    for(i=0;i<15;i++){
        chk=readDHT11Once(pin,humidity,temperature);

        if(chk==DHTLIB_OK)
            return DHTLIB_OK;

        delay_ms(100);
    }

    return chk;
}

void dht11_close(void)
{
    if(gpioHandle>=0){
        lgGpiochipClose(gpioHandle);
        gpioHandle=-1;
    }
}