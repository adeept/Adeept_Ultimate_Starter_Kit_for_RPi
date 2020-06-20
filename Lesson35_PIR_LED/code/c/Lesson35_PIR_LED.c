#include <wiringPi.h>
#include <stdio.h>

#define Pir_S  2
#define  LedPin           0



int main(void)
{
  int a,b;
	if(wiringPiSetup() == -1){
		printf("setup wiringPi failed !\n");
		return -1; 
	}
	
	pinMode(Pir_S, INPUT);
	pinMode(LedPin, OUTPUT);


	while(1){
     a = digitalRead(Pir_S);

     if(a == 0)
     {
       printf("...Movement not detected!\n");
       b = 1;
     }
     if(a == 1)
     {
       printf("Movement detected!...\n");
       b = 0;
     }
     digitalWrite(LedPin, b);
     delay(1000);
	}

	return 0;
}