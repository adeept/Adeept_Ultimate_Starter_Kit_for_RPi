#include <wiringPi.h>
#include <stdio.h>

#define   RCLK  0   //memory clock input(STCP)
#define   SRCLK 1   //shift register clock input(SHCP)
#define   SDI   2   //serial data input

const unsigned char data[] = {
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //NULL
		0x00,0x00,0x3C,0x42,0x42,0x3C,0x00,0x00, //0
		0x00,0x00,0x00,0x44,0x7E,0x40,0x00,0x00, //1
		0x00,0x00,0x44,0x62,0x52,0x4C,0x00,0x00, //2
		0x00,0x00,0x78,0x14,0x12,0x14,0x78,0x00, //A
		0x00,0x00,0x60,0x90,0x90,0xFE,0x00,0x00, //d
		0x00,0x00,0x1C,0x2A,0x2A,0x2A,0x24,0x00, //e
		0x00,0x00,0x1C,0x2A,0x2A,0x2A,0x24,0x00, //e
		0x00,0x00,0x7E,0x12,0x12,0x0C,0x00,0x00, //p
		0x00,0x00,0x08,0x7E,0x88,0x40,0x00,0x00, //t
		0x3C,0x42,0x95,0xB1,0xB1,0x95,0x42,0x3C, //:)
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //NULL
	};

const unsigned char tab[] = {0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f}; 

void init(void)
{
	pinMode(SDI, OUTPUT);
	pinMode(RCLK, OUTPUT);
	pinMode(SRCLK, OUTPUT);

	digitalWrite(SDI, 0);
	digitalWrite(RCLK, 0);
	digitalWrite(SRCLK, 0);
}

void hc595_in(unsigned char dat)
{
	int i;

	for(i=0;i<8;i++){
		digitalWrite(SDI, 0x80 & (dat << i));
		digitalWrite(SRCLK, 1);
//		delay(1);
		digitalWrite(SRCLK, 0);
	}
}

void hc595_out()
{
	digitalWrite(RCLK, 1);
//	delay(1);
	digitalWrite(RCLK, 0);
}

int main(void)
{
	int i, j, k;

	if(wiringPiSetup() == -1){
		printf("setup wiringPi failed !\n");
		return -1; 
	}

	init();

	while(1){
		for(i = 0; i < sizeof(data)-8; i++){
			for(k = 0; k < 15; k++){
				for(j = 0; j < 8; j++){
					hc595_in(data[i+j]);
					hc595_in(tab[j]);
					hc595_out();
					delay(2);
				}
			}
		}
	}
	
	return 0;
}
