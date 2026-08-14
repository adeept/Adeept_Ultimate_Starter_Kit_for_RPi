#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <lgpio.h>
#include <unistd.h>

#define SWPin_BCM 27
#define RoAPin_BCM 18
#define RoBPin_BCM 17

static int globalCounter = 0;

int main(void)
{
    int h;
    int lastSW;
    int currentSW;
    int lastA;
    int currentA;
    int currentB;

    h = lgGpiochipOpen(4);
    if(h < 0){
        printf("Open gpiochip4 failed, try gpiochip0\n");
        h = lgGpiochipOpen(0);
        if(h < 0){
            fprintf(stderr,"Unable to open gpio chip: %s\n",strerror(errno));
            return 1;
        }
    }

    printf("Rotary Encoder lgpio Test\n");
    printf("SW  = BCM%d\n",SWPin_BCM);
    printf("CLK = BCM%d\n",RoAPin_BCM);
    printf("DT  = BCM%d\n",RoBPin_BCM);

    if(lgGpioClaimInput(h,LG_SET_PULL_UP,SWPin_BCM) < 0 ||
       lgGpioClaimInput(h,LG_SET_PULL_UP,RoAPin_BCM) < 0 ||
       lgGpioClaimInput(h,LG_SET_PULL_UP,RoBPin_BCM) < 0){
        printf("Failed to claim GPIO pins\n");
        lgGpiochipClose(h);
        return 1;
    }

    lastSW = lgGpioRead(h,SWPin_BCM);
    lastA = lgGpioRead(h,RoAPin_BCM);

    while(1){
        currentSW = lgGpioRead(h,SWPin_BCM);

        if(lastSW == 1 && currentSW == 0){
            globalCounter = 0;
            printf("%d\n",globalCounter);
            usleep(30000);
        }

        lastSW = currentSW;

        currentA = lgGpioRead(h,RoAPin_BCM);

        if(lastA == 1 && currentA == 0){
            currentB = lgGpioRead(h,RoBPin_BCM);

            if(currentB == 1)
                globalCounter++;
            else
                globalCounter--;

            printf("%d\n",globalCounter);

            while(lgGpioRead(h,RoAPin_BCM) == 0)
                usleep(100);

            lastA = 1;
        }
        else{
            lastA = currentA;
        }

        usleep(100);
    }

    lgGpiochipClose(h);
    return 0;
}