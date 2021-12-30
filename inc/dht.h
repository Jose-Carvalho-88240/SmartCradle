#ifndef DHT_H
#define DHT_H

#include <linux/types.h>

/**
 * @brief DHT11 Data structure
 * 		@ CompleteSample: receives all the
 * 		data from the sensor
 *
 * 		@ TemperatureI: Integer part of
 *		the temperature
 * 		
 *		@ TemperatureD: Decimal part of
 *		the temperature
 *
 * 		@ HumidityI: Integer part of
 *		the humidity
 * 		
 *		@ HumidityD: Decimal part of
 *		the humidity
 */
typedef struct DHT11
{
	u_int8_t CompleteSample[5];
	u_int8_t TemperatureI;
	u_int8_t TemperatureD;
	u_int8_t HumidityI;
	u_int8_t HumidityD;
    u_int8_t checksum;
} dht11_t ;

/**
 * @brief Loads the sensor device driver
 * 
 */
void initDHT11();

/**
 * @brief Sample from the sensor
 * 
 * Reads from the device driver all of the
 * sensor data (temperature, humidity and checksum)
 * 
 * @param data: Pointer to dht11_t structure
 * @return int: Returns 0 on error, or 1 if sucessful 
 */
int readDHT11(dht11_t *data);

#endif