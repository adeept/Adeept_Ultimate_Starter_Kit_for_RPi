#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define PCF8591_ADDRESS   0x48  // PCF5891 address
#define A0                0x40  // Photoresistance
#define A1                0x41  // Thermistor
#define A2                0x42  // None(Can read analog data of other sensors)
#define A3                0x43  // Analog-to-digital conversion

int main() {
  int fd = wiringPiI2CSetup(PCF8591_ADDRESS);
  float photosensitive_data, temp_data;

  while(1) {
    photosensitive_data = wiringPiI2CReadReg8(fd, A0);
    temp_data = wiringPiI2CReadReg8(fd, A1);
    printf("Photoresistance: %.2f\n", photosensitive_data);
    printf("temp: %.2f\n", temp_data);
    delay(500);
  }
}