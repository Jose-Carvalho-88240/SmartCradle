#ifndef DHT_H
#define DHT_H

#include <linux/types.h>

typedef struct DHT11
{
	u_int8_t CompleteSample[5];
	u_int8_t TemperatureI;
	u_int8_t TemperatureD;
	u_int8_t HumidityI;
	u_int8_t HumidityD;
    u_int8_t checksum;
} dht11_t ;

void initDHT11();
int readDHT11(dht11_t *data);

#endif