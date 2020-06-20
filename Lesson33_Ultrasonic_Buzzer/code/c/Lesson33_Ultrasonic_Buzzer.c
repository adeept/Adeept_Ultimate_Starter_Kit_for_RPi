#include <wiringPi.h>  
#include <stdio.h>  
#include <sys/time.h>  

#define  Trig    4  
#define  Echo    5  
#define  BeepPin 1

void ultraInit(void)  
{  
	pinMode(Echo, INPUT);  
	pinMode(Trig, OUTPUT);  
}  

float disMeasure(void)  
{  
	struct timeval tv1;  
	struct timeval tv2;  
	long start, stop;  
	float dis;  

	digitalWrite(Trig, LOW);  
	delayMicroseconds(2);  

	digitalWrite(Trig, HIGH);  //produce a pluse
	delayMicroseconds(10); 
	digitalWrite(Trig, LOW);  

	while(!(digitalRead(Echo) == 1));  
	gettimeofday(&tv1, NULL);           //current time 

	while(!(digitalRead(Echo) == 0));  
	gettimeofday(&tv2, NULL);           //current time  

	start = tv1.tv_sec * 1000000 + tv1.tv_usec; 
	stop  = tv2.tv_sec * 1000000 + tv2.tv_usec;  

	dis = (float)(stop - start) / 1000000 * 34000 / 2;  //count the distance 

	return dis;  
}  

void beepOn() {
  
		//digitalWrite(BeepPin, LOW);  //beep on
		delay(1000);                  //delay
		digitalWrite(BeepPin, HIGH); //beep off
		delay(1000);                  //delay
	
}

void beepOff() {
  digitalWrite(BeepPin, LOW); //beep off
}

int main(void)  
{  
	float current_dis;  
  float safety_dis = 50;

	if(wiringPiSetup() == -1){ //when initialize wiring failed,print message to screen  
		printf("setup wiringPi failed !\n");  
		return -1;   
	}  

	ultraInit();  
  pinMode(BeepPin, OUTPUT);   //set GPIO0 output
  
	while(1){  
		current_dis = disMeasure();
		printf("Current distance = %0.2f cm\n",current_dis);  
   if (current_dis < safety_dis)
      beepOn();
    else
      beepOff();
  
		delay(1000); 
	}  

	return 0;  
}  
