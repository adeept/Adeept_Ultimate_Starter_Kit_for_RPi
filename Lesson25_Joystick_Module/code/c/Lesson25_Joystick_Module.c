#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio.h>

#define PCF8591_ADDRESS 0x48
#define Y               0x41
#define X               0x42
#define Z               0x43


int read(int fd, int reg) {
  wiringPiI2CReadReg8(fd, reg);
  return wiringPiI2CReadReg8(fd, reg);
}

int main() {
  int fd = wiringPiI2CSetup(PCF8591_ADDRESS);

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
        printf("up\n");
        break;
      case 2:
        printf("down\n");
        break;
      case 3:
        printf("right\n");
        break;
      case 4:
        printf("left\n");
        break;
      default:
        break;
    }
    tmp = 0;
    
    delay(500);
  }
  return 0;
}
