/**
 * @file motor.c
 * @author José Carvalho, João Carneiro
 * @brief Motor module implementation
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/types.h>
#include "../inc/motor.h"

const char RUN = '1';
const char STOP = '0';

static _Bool motorRunning; /* Status of the motor */

void initMotor(){
    system("insmod motordriver.ko");
    motorRunning = 0;
}

void remMotor(){
    system("rmmod motordriver");
}

int startMotor(){

    if(!motorRunning)
    {
        int fd0 = open("/dev/motordriver0", O_WRONLY);   
        if(fd0 == -1)
        {
            printf("Failed to open motor driver device driver.\n");
            return 0;
        }

        if(write(fd0, &RUN, 1) <= 0)
        {
            printf("Failed to write on device driver.\n");
            close(fd0);
            return 0;
        }

        motorRunning = 1;
        close(fd0);
    }
    return 1;
}

int stopMotor(){

    if(motorRunning)
    {
        int fd0 = open("/dev/motordriver0", O_WRONLY);   
        if(fd0 == -1)
        {
            printf("Failed to open motor driver device driver.\n");
            return 0;
        }   

        if(write(fd0, &STOP, 1) <= 0)
        {
            printf("Failed to write on device driver.\n");
            close(fd0);
            return 0;
        }

        motorRunning = 0;
        close(fd0);
    }
    return 1;
}

_Bool getMotorStatus()
{
    return motorRunning;
}