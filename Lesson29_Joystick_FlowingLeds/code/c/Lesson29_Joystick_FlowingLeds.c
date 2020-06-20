#include <wiringPi.h>
#include <stdio.h>
#include <wiringPiI2C.h>

#define PCF8591_ADDRESS 0x48
#define Y               0x41
#define X               0x42
#define Z               0x43


int read(int fd, int reg) {
  wiringPiI2CReadReg8(fd, reg);
  return wiringPiI2CReadReg8(fd, reg);
}


//make led_n on
void led_on(int n)
{
digitalWrite(n, LOW);
}

//make led_n off
void led_off(int n)
{
digitalWrite(n, HIGH);
}

int main() {
  if(wiringPiSetup() == -1){ //when initialize wiring failed,print message to screen
printf("setup wiringPi failed !\n");
return -1; 
}
  int fd = wiringPiI2CSetup(PCF8591_ADDRESS);

  int tmp = 0;
  int x, y, z, i;

  for(i=0;i<8;i++){       //make 8 pins' mode is output
pinMode(i, OUTPUT);
}
  
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
        for(i=0;i<8;i++){   //make led on from left to right
          led_on(i);
          delay(300);
          led_off(i);
        }
        break;
      case 4:
        for(i=8;i>=0;i--){  //make led off from right to left
          led_on(i);
          delay(300);
          led_off(i);
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
