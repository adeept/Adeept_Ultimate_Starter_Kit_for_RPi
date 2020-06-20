#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>

#define time_out 5// Delay time 5s
#define RELAY 7
#define ON 1
#define OFF 0

int main()
{
    wiringPiSetup();			
    pinMode(RELAY,OUTPUT);
    while(1)
    {
        digitalWrite(RELAY,ON);
        sleep(time_out);
        digitalWrite(RELAY,OFF);
        sleep(time_out);
    }
    return 0;
}