#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    sleep(5);
    printf("Devicer Drivers - Motor\n");

    unsigned int count = 0;
    printf("\n");

    printf("\n\nInserting Device Driver...\n");
    system("insmod motordriver.ko");

    sleep(3);

    int fd0 = open("/dev/motordriver0", O_WRONLY);
    char motorOn = '1', motorOff = '0';
    printf("\nStarting loop (5 times):");
    sleep(1);
    while(count != 5)
    {
        write(fd0, &motorOn, 1);
        printf("\nandar!");
        sleep(2);
       
        write(fd0, &motorOff, 1);
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
