#include <wiringPi.h>  
#include <softPwm.h>  
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>

#define MAXTIMINGS 85

#define DHTPIN 3

int dht11_dat[5] = {0,0,0,0,0};

void read_dht11_dat(int *temperature, int *humidity)
{
	uint8_t laststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i;
	float f; // fahrenheit

	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

	// pull pin down for 18 milliseconds
	pinMode(DHTPIN, OUTPUT);
	digitalWrite(DHTPIN, LOW);
	delay(18);
	// then pull it up for 40 microseconds
	digitalWrite(DHTPIN, HIGH);
	delayMicroseconds(40); 
	// prepare to read the pin
	pinMode(DHTPIN, INPUT);

	// detect change and read data
	for ( i=0; i< MAXTIMINGS; i++) {
		counter = 0;
		while (digitalRead(DHTPIN) == laststate) {
			counter++;
			delayMicroseconds(1);
			if (counter == 255) {
				break;
			}
		}
		laststate = digitalRead(DHTPIN);

		if (counter == 255) break;

		// ignore first 3 transitions
		if ((i >= 4) && (i%2 == 0)) {
			// shove each bit into the storage bytes
			dht11_dat[j/8] <<= 1;
			if (counter > 16)
				dht11_dat[j/8] |= 1;
			j++;
		}
	}

	// check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	// print it out if data is good
	if ((j >= 40) && 
			(dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)) ) {
		f = dht11_dat[2] * 9. / 5. + 32;
		*temperature = dht11_dat[1];
		*humidity = dht11_dat[0];
		printf("Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n", 
				dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f);
	}
	else
	{
		printf("Data not good, skip\n");
	}
}


#define LedPinRed    0  
#define LedPinGreen  1  
#define LedPinBlue   2  

int map(int x, int in_min, int in_max, int out_min, int out_max)     
{  
	return (x -in_min) * (out_max - out_min) / (in_max - in_min) + out_min;  
}  

void ledInit(void)  
{  
	softPwmCreate(LedPinRed,  0, 100);  //create a soft pwm, original duty cycle is 0Hz, range is 0~100   
	softPwmCreate(LedPinGreen,0, 100);  
	softPwmCreate(LedPinBlue, 0, 100);  
}  

void ledColorSet(int color)        //set color, for example: 0xde3f47  
{  
	int r_val, g_val, b_val;  

	r_val = (color & 0xFF0000) >> 16;  //get red value  
	g_val = (color & 0x00FF00) >> 8;   //get green value  
	b_val = (color & 0x0000FF) >> 0;   //get blue value  

	r_val = map(r_val, 0, 255, 0, 100);    //change a num(0~255) to 0~100  
	g_val = map(g_val, 0, 255, 0, 100);  
	b_val = map(b_val, 0, 255, 0, 100);  

	softPwmWrite(LedPinRed,   100 - r_val);  //change duty cycle  
	softPwmWrite(LedPinGreen, 100 - g_val);  
	softPwmWrite(LedPinBlue,  100 - b_val);  
}  


int main (void)
{

	printf ("Raspberry Pi wiringPi DHT11 Temperature test program\n") ;

	if (wiringPiSetup () == -1)
		exit (1) ;
	
	int temparture, humidity, color;

	ledInit();

	while (1) 
	{
		read_dht11_dat(&temparture, &humidity);
		color = temparture + humidity;
		ledColorSet(color);
		delay(500); // wait 0.5sec to refresh
	}

	return 0 ;
}