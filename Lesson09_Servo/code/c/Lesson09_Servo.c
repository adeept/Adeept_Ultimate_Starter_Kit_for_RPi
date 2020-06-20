#include <stdio.h>
#include <wiringPi.h>

#define  Servo  0

void servo(int angle) //500~2500
{
	digitalWrite(Servo, 1);
	delayMicroseconds(angle);
	digitalWrite(Servo, 0);
	delayMicroseconds(20000-angle);
}

int main(void)
{
	int i, j;

	if(wiringPiSetup() < 0){
		printf("wiringPi setup error!\n");
		return -1;
	}	

	pinMode(Servo, OUTPUT);

	while(1){
		servo(500);
		delay(500);
		for(i=500; i <=2500; i=i+500){
			servo(i);
			printf("i = %d\n", i);
			delay(500);
		}
		servo(2500);
		delay(500);
		for(i=2500; i >=500; i=i-500){
			servo(i);
			printf("............i = %d\n", i);
			delay(500);
		}
	}

	return 0;
}