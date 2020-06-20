#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define PCF8591_ADDRESS   0x48  // PCF5891 address
#define A0                0x40  // Photoresistance
#define A1                0x41  // Thermistor
#define A2                0x42  // None(Can read analog data of other sensors)
#define A3                0x43  // Analog-to-digital conversion
#define  LedPin           0

int main() {
  int fd = wiringPiI2CSetup(PCF8591_ADDRESS);
  float light_intensity;
  int light_threshold = 100;

  if(wiringPiSetup() == -1){ //when initialize wiringPi failed, print message to screen
		printf("setup wiringPi failed !\n");
		return -1; 
	}
	
	pinMode(LedPin, OUTPUT);

  while(1) {
    light_intensity = wiringPiI2CReadReg8(fd, A0);
    printf("Light intensity: %.2f\n", light_intensity);

    if (light_intensity > light_threshold)
      digitalWrite(LedPin, LOW);   //led on
    else
      digitalWrite(LedPin, HIGH);  //led off
    delay(500);
  }
}