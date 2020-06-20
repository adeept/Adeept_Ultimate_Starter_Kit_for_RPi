#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define  BUFSIZE  128


#define pcf8574_address 0x27        // default I2C address of Pcf8574
// #define pcf8574_address 0x3F        // default I2C address of Pcf8574A
#define BASE 64         // BASE any number above 64
//Define the output pins of the PCF8574, which are directly connected to the LCD1602 pin.
#define RS      BASE+0
#define RW      BASE+1
#define EN      BASE+2
#define LED     BASE+3
#define D4      BASE+4
#define D5      BASE+5
#define D6      BASE+6
#define D7      BASE+7

int lcdhd;// used to handle LCD

int main(void){
  wiringPiSetup();  
  float temp;
	int i, j;
  int fd;
	int ret;

	char buf[BUFSIZE];
	char tempBuf[5];

  system("cat /sys/bus/w1/devices/28-*/w1_slave > result");

  pcf8574Setup(BASE,pcf8574_address);//initialize PCF8574
  for(i=0;i<8;i++){
      pinMode(BASE+i,OUTPUT);     //set PCF8574 port to output mode
  } 
  digitalWrite(LED,HIGH);     //turn on LCD backlight
  digitalWrite(RW,LOW);       //allow writing to LCD
  lcdhd = lcdInit(2,16,4,RS,EN,D4,D5,D6,D7,0,0,0,0);// initialize LCD and return “handle” used to handle LCD
  if(lcdhd == -1){
      printf("lcdInit failed !");
      return 1;
  }

	while(1){	
		fd = open("result", O_RDONLY);

		if(-1 == fd){
			perror("open device file error");
			return 1;
		}

		while(1){
			ret = read(fd, buf, BUFSIZE);
			if(0 == ret){
				break;	
			}
			if(-1 == ret){
				if(errno == EINTR){
					continue;	
				}
				perror("read()");
				close(fd);
				return 1;
			}
		}

		for(i=0;i<sizeof(buf);i++){
			if(buf[i] == 't' && buf[i+1] == '='){
				for(j=0;j<sizeof(tempBuf);j++){
					tempBuf[j] = buf[i+2+j];
				}
			}	
		}
		temp = (float)atoi(tempBuf) / 1000;

		printf("%.3f C\n",temp);

		close(fd);
    lcdPosition(lcdhd,0,1);// set the LCD cursor position to (0,1) 
    lcdPrintf(lcdhd,"Temp:%.3f", temp); //Display temp on LCD

		delay(500);

	}
  return 0;
}
