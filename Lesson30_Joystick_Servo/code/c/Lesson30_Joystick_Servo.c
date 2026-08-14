#include <lgpio.h>
#include <stdio.h>

#define SERVO_PIN 17
#define PCF8591_ADDR 0x48
#define Y_CH 0x41
#define X_CH 0x42
#define Z_CH 0x43

#define SERVO_MIN 500
#define SERVO_MAX 2500
#define SERVO_CENTER 1500
#define SERVO_STEP 100

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

void servo(int h,int pulse_us)
{
    float duty;

    duty=pulse_us/200.0;

    lgTxPwm(h,SERVO_PIN,50,duty,0,0);
}

int main(void)
{
    int h;
    int fd;
    int x,y,z;
    int servoPos=SERVO_CENTER;

    h=lgGpiochipOpen(4);
    if(h<0){
        printf("Failed to open gpiochip4, try gpiochip0\n");
        h=lgGpiochipOpen(0);
        if(h<0) return -1;
    }

    fd=lgI2cOpen(1,PCF8591_ADDR,0);

    if(fd<0){
        printf("Failed to open I2C device\n");
        lgGpiochipClose(h);
        return -1;
    }

    lgGpioClaimOutput(h,0,SERVO_PIN,0);

    servo(h,servoPos);

    printf("Joystick servo control start\n");
    printf("Servo center = %d us\n",servoPos);

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
            servoPos-=SERVO_STEP;

            if(servoPos<SERVO_MIN)
                servoPos=SERVO_MIN;

            printf("left  X=%d servo=%d\n",x,servoPos);
        }
        else if(x>220){
            servoPos+=SERVO_STEP;

            if(servoPos>SERVO_MAX)
                servoPos=SERVO_MAX;

            printf("right X=%d servo=%d\n",x,servoPos);
        }
        else if(y<100){
            servoPos+=SERVO_STEP;

            if(servoPos>SERVO_MAX)
                servoPos=SERVO_MAX;

            printf("up    Y=%d servo=%d\n",y,servoPos);
        }
        else if(y>230){
            servoPos-=SERVO_STEP;

            if(servoPos<SERVO_MIN)
                servoPos=SERVO_MIN;

            printf("down  Y=%d servo=%d\n",y,servoPos);
        }

        servo(h,servoPos);

        lguSleep(0.02);
    }

    lgTxPwm(h,SERVO_PIN,0,0,0,0);
    lgI2cClose(fd);
    lgGpiochipClose(h);

    return 0;
}