#include <lgpio.h>
#include <stdio.h>

#define BUZZER_PIN 18

void setTone(int h,int freq)
{
    if(freq<=0){
        lgTxPwm(h,BUZZER_PIN,0,0,0,0);
    }else{
        lgTxPwm(h,BUZZER_PIN,(float)freq,50.0,0,0);
    }
}

int main(void)
{
    int h=lgGpiochipOpen(4);
    if(h<0){
        printf("Failed to open gpiochip4, try open gpiochip0\n");
        h = lgGpiochipOpen(0);
        if (h < 0) return -1;
    }
    lgGpioClaimOutput(h,0,BUZZER_PIN,0);
    int frequencies[]={200,400,600,800,900,1000,1100};
    int count=sizeof(frequencies)/sizeof(frequencies[0]);
    while(1){
        for(int i=0;i<count;i++){
            setTone(h,frequencies[i]);
            lguSleep(0.2);
        }
        for(int i=count-1;i>=0;i--){
            setTone(h,frequencies[i]);
            lguSleep(0.2);
        }
    }
    lgGpiochipClose(h);
    return 0;
}
