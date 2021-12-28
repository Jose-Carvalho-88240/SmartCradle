#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void simpleDelay (void)
{
    volatile unsigned int i;
    for (i = 0; i != 100000000; i++);
}

int main(void)
{
    sleep(5);
    printf("Devicer Drivers - DHT11\n");

    printf("\n");

    printf("\n\nInserting Device Driver...\n");
    system("insmod dht11.ko");

    sleep(3);

    int fd0 = open("/dev/dht110", O_RDONLY);  
    
    if(fd0 == -1)
    {
        printf("Failed to open DHT11 device driver.\n");
        return NULL;
    }

    char buffer[5];
    int n=0;
    while(n<5)
    {
    printf("Will read\n");
    if(read(fd0,buffer, 5) != 5)
    {
        printf("Failed to read from device driver.\n");
        //close(fd0);
        //return NULL;
        
    }
    else
    {
        printf("Temperature: %d.%dºC\nHumidity: %d.%d\nChecksum: %d\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4]);
    }
    
    //close(fd0);
    
    n++;
    simpleDelay();
    }
    putchar('\n');
    printf("Removing Device Driver.\n");
    system("rmmod dht11");

    return 0;
}
