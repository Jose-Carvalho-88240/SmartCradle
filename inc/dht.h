#ifndef DHT_H
#define DHT_H

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
	unsigned char CompleteSample[5];
	unsigned char TemperatureI;
	unsigned char TemperatureD;
	unsigned char HumidityI;
	unsigned char HumidityD;
    	unsigned char checksum;
} dht11_t ;

/**
 * @brief Loads the sensor device driver
 */
void initDHT11(void);

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
