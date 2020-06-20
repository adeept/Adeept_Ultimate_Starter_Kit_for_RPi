#include <stdio.h>
#include <wiringPi.h>

typedef unsigned char uchar;
//			    0     1    2   3    4    5    6    7    8    9     a   b    c    d    e    f    g
const uchar SegCode[17] = {0xbf,0x86,0xdb,0xcf,0xE6,0xed,0xfd,0x87,0xff,0xef,0xdf,0xfc,0xd8,0xfb,0xf1,0xef,0xf4};

const uchar segLedPins[] = {0,1,3,2,4,5,6,7,8};
int redbade(uchar a,int b)
{
     return (a>>b)&0x01;
}
int main(void)
{
	int i,j;

	if(wiringPiSetup() < 0){ // setup wiringPi
		printf("wiringPi setup failed !\n");
		return -1;
	}


	for(i = 0; i < 8; i++){  // set pin mode as output(GPIO0~GPIO7)
		pinMode(segLedPins[i], OUTPUT);
	}


	while(1){
        for(j = 0; j <17; j++){
		for(i = 0; i  <8; i++){ // display 0~9,A~F
			digitalWrite(segLedPins[i],redbade(SegCode[j],i));
		}
   delay(500);
   }
	}

	return 0;
}

