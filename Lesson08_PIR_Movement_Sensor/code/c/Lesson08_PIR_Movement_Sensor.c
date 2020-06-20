#include <wiringPi.h>
#include <stdio.h>

#define Pir_S  0


int main(void)
{
        if(wiringPiSetup() == -1){
                printf("setup wiringPi failed !\n");
                return -1;
        }

        pinMode(Pir_S, INPUT);

        while(1){
                if(digitalRead(Pir_S)==LOW)
                        printf("...Movement not detected!\n");
                else
                        printf("Movement detected!...\n");

                delay(500);
        }

        return 0;
}

