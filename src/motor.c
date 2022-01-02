#include "../inc/motor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/types.h>

static _Bool motorRunning; /* Status of the motor */

void initMotor(){
    system("insmod motordriver.ko");
    motorRunning=0;
}

int startMotor(){
    int fd0 = open("/dev/motordriver0", O_WRONLY);   
    if(fd0 == -1)
    {
        printf("Failed to open motor driver device driver.\n");
        return 0;
    }

    motorRunning = 1;

    if(write(fd0, &motorRunning, 1) == -1)
    {
        printf("Failed to write on device driver.\n");
        motorRunning=0;
        close(fd0);
        return 0;
    }
    
    close(fd0);

    return 1;
}

int stopMotor(){
    int fd0 = open("/dev/motordriver0", O_WRONLY);   
    if(fd0 == -1)
    {
        printf("Failed to open motor driver device driver.\n");
        return 0;
    }

    motorRunning = 0;

    if(write(fd0, &motorRunning, 1) == -1)
    {
        printf("Failed to write on device driver.\n");
        motorRunning=1;
        close(fd0);
        return 0;
    }

    close(fd0);

    return 1;
}

_Bool getMotorStatus()
{
    return motorRunning;
}