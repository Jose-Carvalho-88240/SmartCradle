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
    printf("Devicer Drivers - Led Blinking\n");

    unsigned int count = 0;
    printf("\n");

    printf("\n\nInserting Device Driver...\n");
    system("insmod motordriver.ko");

    printf("\nCheck devicer driver:\n");
    system("lsmod");

    printf("\nIs the device driver in /dev:\n");
    system("ls -l /dev/motordriver0");
    sleep(3);

    int fd0 = open("/dev/led0", O_WRONLY);
    char ledOn = '1', ledOff = '0';
    char r;
    printf("\nStarting loop (5 times):");
    sleep(1);
    while(count != 5)
    {
        
        write(fd0, &ledOn, 1);
        printf("\nandar!");
        sleep(2);
       
        write(fd0, &ledOff, 1);
        printf("\nparar!");
        sleep(2);
        count++;
    }
    printf("Closing Device Driver.\n");
    close(fd0);
    putchar('\n');
    printf("Removing Device Driver.\n");
    system("rmmod motordriver");

    return 0;

}
