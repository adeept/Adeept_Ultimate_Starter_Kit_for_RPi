#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define PCF8591_ADDRESS   0x48  // PCF5891 address
#define MicSensor_ADDRESS 0x42  // MicSensor address

int main() {
  int fd = wiringPiI2CSetup(PCF8591_ADDRESS);
  float micSensorData;

  while(1) {
    micSensorData = wiringPiI2CReadReg8(fd, MicSensor_ADDRESS);
    printf("micSensorData: %.2f\n", micSensorData / 10);
    delay(500);
  }
}
