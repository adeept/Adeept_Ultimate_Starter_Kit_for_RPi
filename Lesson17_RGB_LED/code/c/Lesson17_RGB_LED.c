#include <lgpio.h>
#include <stdio.h>

#define LED_RED 17
#define LED_GREEN 18
#define LED_BLUE 27

const int colors[]={0xFF0000,0x00FF00,0x0000FF,0xFFFF00,0x00FFFF,0xFF00FF,0xFFFFFF,0x9400D3};

int map(int x,int in_min,int in_max,int out_min,int out_max)
{
    return (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
}

void ledInit(int h)
{
    lgGpioClaimOutput(h,0,LED_RED,1);
    lgGpioClaimOutput(h,0,LED_GREEN,1);
    lgGpioClaimOutput(h,0,LED_BLUE,1);
}

void ledColorSet(int h,int color)
{
    int r=(color>>16)&0xFF;
    int g=(color>>8)&0xFF;
    int b=color&0xFF;
    r=map(r,0,255,0,100);
    g=map(g,0,255,0,100);
    b=map(b,0,255,0,100);
    lgTxPwm(h,LED_RED,1000,100-r,0,0);
    lgTxPwm(h,LED_GREEN,1000,100-g,0,0);
    lgTxPwm(h,LED_BLUE,1000,100-b,0,0);
}

int main(void)
{
    int h=lgGpiochipOpen(4);
    if(h<0){
        printf("Failed to open gpiochip4, try gpiochip0\n");
        h=lgGpiochipOpen(0);
        if(h<0)return -1;
    }
    ledInit(h);
    while(1){
        for(int i=0;i<sizeof(colors)/sizeof(int);i++){
            ledColorSet(h,colors[i]);
            lguSleep(0.5);
        }
    }
    lgGpiochipClose(h);
    return 0;
}