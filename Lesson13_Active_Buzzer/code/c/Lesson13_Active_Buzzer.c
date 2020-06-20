#include <wiringPi.h>
#include <stdio.h>

#define BeepPin 1

int main(void)
{
	if(wiringPiSetup() == -1){ //when initialize wiringPi failed, print message to screen
		printf("setup wiringPi failed !");
		return -1; 
	}
	
	pinMode(BeepPin, OUTPUT);   //set GPIO0 output

	while(1){
		digitalWrite(BeepPin, LOW);  //beep on
		delay(100);                  //delay
		digitalWrite(BeepPin, HIGH); //beep off
		delay(100);                  //delay
	}

	return 0;
}

