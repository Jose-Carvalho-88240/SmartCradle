#ifndef DHT_H
#define DHT_H

#include <linux/types.h>

typedef struct DHT11
{
	int CompleteSample[5];
	uint8_t TemperatureI;
	uint8_t TemperatureD;
	uint8_t HumidityI;
	uint8_t HumidityD;
    uint8_t checksum;
} dht11_t ;

void initDHT11();
dht11_t readDHT11();

#endif