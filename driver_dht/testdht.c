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

    putchar('\n');
    printf("Removing Device Driver.\n");
    system("rmmod dht11");

    return 0;
}
