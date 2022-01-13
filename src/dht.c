/**
 * @file database.c
 * @author José Carvalho, João Carneiro
 * @brief Temperature and humidity sensor module implementation
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../inc/dht.h"

void initDHT11(void){
    system("insmod dht11.ko");
}

void remDHT11(void){
    system("rmmod dht11");
}

int readDHT11(dht11_t *data){
    int fd0 = open("/dev/dht110", O_RDONLY);  
    
    if(fd0 == -1)
    {
        printf("Failed to open DHT11 device driver.\n");
        return 0;
    }

    if(read(fd0,data, sizeof(*data)) <= 0)
    {
        printf("Failed to read from device driver.\n");
        close(fd0);
        return 0;
    }
    
    close(fd0);

    return 1;
}
