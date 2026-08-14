#ifndef DHT11_H
#define DHT11_H

#define DHTLIB_OK 0
#define DHTLIB_ERROR_CHECKSUM -1
#define DHTLIB_ERROR_TIMEOUT -2
#define DHTLIB_INVALID_VALUE -999

int dht11_init(void);
int dht11_read(int pin,float *humidity,float *temperature);
void dht11_close(void);

#endif