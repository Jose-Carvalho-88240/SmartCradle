#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../inc/dht.h"

int main(void)
{
    printf("Devicer Drivers - DHT11\n");
    printf("\n\nInserting Device Driver...\n");
    
    initDHT11();

    sleep(2);

    dht11_t sensorData;

    readDHT11(&sensorData);

    float  temperature = sensorData.TemperatureI + (float)(sensorData.TemperatureD)/100;
    float humidity = sensorData.HumidityI + (float)(sensorData.HumidityD)/100;

    printf("\tTemperature: %.2f \ºC \n \tHumidity: %.2f \%\n", temperature, humidity);

    printf("Removing Device Driver.\n");
    system("rmmod dht11");

    return 0;
}
