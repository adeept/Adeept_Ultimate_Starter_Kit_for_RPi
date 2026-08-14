#include <lgpio.h>
#include <stdio.h>

#define PCF8591_ADDR 0x48
#define Y_CH 0x41
#define X_CH 0x42
#define Z_CH 0x43

const int ledPins[] = {17,18,27,22,23};
#define LED_COUNT 5

int readADC(int fd,int channel)
{
    int value;
    if(lgI2cWriteByte(fd,channel)<0)
        return -1;
    if(lgI2cReadByte(fd)<0)
        return -1;
    value=lgI2cReadByte(fd);
    return value;
}

void led_on(int h,int n)
{
    lgGpioWrite(h,ledPins[n],0);
}

void led_off(int h,int n)
{
    lgGpioWrite(h,ledPins[n],1);
}

void led_forward(int h)
{
    int i;
    for(i=0;i<LED_COUNT;i++){
        led_on(h,i);
        lguSleep(0.3);
        led_off(h,i);
    }
}

void led_backward(int h)
{
    int i;
    for(i=LED_COUNT-1;i>=0;i--){
        led_on(h,i);
        lguSleep(0.3);
        led_off(h,i);
    }
}

int main(void)
{
    int h=lgGpiochipOpen(4);
    if(h<0){
        printf("Failed to open gpiochip4, try gpiochip0\n");
        h=lgGpiochipOpen(0);
        if(h<0)
            return -1;
    }

    int fd=lgI2cOpen(1,PCF8591_ADDR,0);
    if(fd<0){
        printf("Failed to open I2C device\n");
        lgGpiochipClose(h);
        return -1;
    }

    int x,y,z;
    int i;

    for(i=0;i<LED_COUNT;i++)
        lgGpioClaimOutput(h,0,ledPins[i],1);

    while(1){
        x=readADC(fd,X_CH);
        y=readADC(fd,Y_CH);
        z=readADC(fd,Z_CH);

        if(x<0||y<0||z<0){
            printf("I2C read error\n");
            lguSleep(0.1);
            continue;
        }

        if(x<50){
            printf("left X=%d Y=%d Z=%d\n",x,y,z);
            led_backward(h);
        }
        else if(x>220){
            printf("right X=%d Y=%d Z=%d\n",x,y,z);
            led_forward(h);
        }
        else if(y<100){
            printf("up X=%d Y=%d Z=%d\n",x,y,z);
            led_forward(h);
        }
        else if(y>230){
            printf("down X=%d Y=%d Z=%d\n",x,y,z);
            led_backward(h);
        }

        lguSleep(0.1);
    }

    lgI2cClose(fd);
    lgGpiochipClose(h);
    return 0;
}