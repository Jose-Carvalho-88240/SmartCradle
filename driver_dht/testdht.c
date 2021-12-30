#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "dht.h"

void simpleDelay (void)
{
    volatile unsigned int i;
    for (i = 0; i != 100000000; i++);
}

int main(void)
{
    printf("Devicer Drivers - DHT11\n");
    printf("\n\nInserting Device Driver...\n");
    system("insmod dht11.ko");

    sleep(2);

    dht11_t sensorData;

    int fd0 = open("/dev/dht110", O_RDWR);  
    
    if(fd0 == -1)
    {
        printf("Failed to open DHT11 device driver.\n");
        return NULL;
    }

    int n=0;
    while(n<2)
    {
        printf("Will read\n");
        if(!read(fd0,&sensorData, sizeof(sensorData)))
        {
            printf("Failed to read from device driver.\n");       
        }
        else
        {
            printf("Temperature: %d.%dC\nHumidity: %d.%d\nChecksum: %d\n\n",sensorData.TemperatureI,sensorData.TemperatureD, \
            sensorData.HumidityI, sensorData.HumidityD,sensorData.checksum);
        }
        n++;
        simpleDelay();
        simpleDelay();
        simpleDelay();
    }
    putchar('\n');
    printf("Removing Device Driver.\n");
    simpleDelay();
    system("rmmod dht11");

    return 0;
}
