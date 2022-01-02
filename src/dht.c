#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../inc/dht.h"

void initDHT11(void){
    system("insmod dht11.ko");
}

int readDHT11(dht11_t *data){
    int fd0 = open("/dev/dht110", O_WRONLY);  
    
    if(fd0 == -1)
    {
        printf("Failed to open DHT11 device driver.\n");
        return 0;
    }

    if(read(fd0,&data->CompleteSample, strlen(data->CompleteSample)) == 0)
    {
        printf("Failed to read from device driver.\n");
        close(fd0);
        return 0;
    }
    
    close(fd0);

    data->TemperatureI=data->CompleteSample[0];
    data->TemperatureD=data->CompleteSample[1];
    data->HumidityI=data->CompleteSample[2];
    data->HumidityD=data->CompleteSample[3];
    data->checksum=data->CompleteSample[4];

    printf("Temperature: %d.%dºC\nHumidity: %d.%d\nChecksum: %d\n",data->TemperatureI,
    data->TemperatureD,data->HumidityI,data->HumidityD,data->checksum);
    
    return 1;
}
