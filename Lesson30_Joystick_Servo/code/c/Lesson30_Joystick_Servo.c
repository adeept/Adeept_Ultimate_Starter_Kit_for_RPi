#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio.h>
#define  Servo  0

#define PCF8591_ADDRESS 0x48
#define Y               0x41
#define X               0x42
#define Z               0x43

void servo(int angle) //500~2500
{
	digitalWrite(Servo, 1);
	delayMicroseconds(angle);
	digitalWrite(Servo, 0);
	delayMicroseconds(20000-angle);
}

int read(int fd, int reg) {
  wiringPiI2CReadReg8(fd, reg);
  return wiringPiI2CReadReg8(fd, reg);
}

int main() {
  int fd = wiringPiI2CSetup(PCF8591_ADDRESS);

  int i, j;

	if(wiringPiSetup() < 0){
		printf("wiringPi setup error!\n");
		return -1;
	}	

	pinMode(Servo, OUTPUT);

  int tmp = 0;
  int x, y, z;
  
  while (1) {
    x = read(fd, X);
    y = read(fd, Y);
    z = read(fd, Z);

   if(x == 0) {
      tmp = 4;  // left
      x = 127;
    }
    if (x == 255) {
      tmp = 3;  // right
      x = 127;
    }

    if (y == 0) {
      tmp = 1;  // up
      y = 127;
    }  

    if (y == 255) {
      tmp = 2;  // down
      y = 127;
    }
    switch(tmp) {
      case 1:
        // printf("up\n");
        break;
      case 2:
        // printf("down\n");
        break;
      case 3:
        servo(500);
        delay(500);
        for(i=500; i <=2500; i=i+500){
          servo(i);
          printf("i = %d\n", i);
          delay(500);
        }
        break;
      case 4:
        servo(2500);
        delay(500);
        for(i=2500; i >=500; i=i-500){
          servo(i);
          printf("............i = %d\n", i);
          delay(500);
        }
        break;
      default:
        break;
    }
    tmp = 0;
    
    delay(500);
  }
  return 0;
}
