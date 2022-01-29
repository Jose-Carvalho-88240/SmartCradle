/**
 * @file main.c
 * @author José Carvalho, João Carneiro
 * @brief Main process implementation
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h> 
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include "../inc/database.h"
#include "../inc/microphone.h"
#include "../inc/livestream.h"
#include "../inc/motor.h"
#include "../inc/dht.h"

#define MSGQOBJ_NAME "/mqLocalDaemon" /**<Message queue name */
#define SHMEMOBJ_NAME "/shLocalDaemon" /**<Shared memory name */
#define MAX_MSG_LEN     128

#define streamPrio 2 /**<tStartStopStream priority */
#define motorPrio 2 /**<tStartStopMotor priority */
#define updateDatabasePrio 3 /**<tUpdateDatabase priority */
#define sensorPrio 4 /**<tReadSensor priority */

#define motorTimeout 10 /**<Motor timeout in minutes */
#define sensorSample 10 /**<Sensor sampling time in minutes */

_Bool motorFlag = 0; /**<Motor flag */
_Bool streamFlag = 0; /**<Livestream flag */
_Bool sensorFlag = 0; /**<Sensor sample successful flag */

pthread_mutex_t motorFlag_mutex = PTHREAD_MUTEX_INITIALIZER; /**<shared variable (motorFlag) */
pthread_mutex_t streamFlag_mutex = PTHREAD_MUTEX_INITIALIZER; /**<shared variable (streamFlag) */
pthread_mutex_t sensorFlag_mutex = PTHREAD_MUTEX_INITIALIZER; /**<shared variable (sensorFlag) */

float databaseTemperature = 0; /**<Temperature to send to database */
float databaseHumidity = 0; /**<Humidity to send to database */

pid_t daemonPID; /**<Daemon PID */

mqd_t msgq_id; /**<Message queue ID */
struct mq_attr msgq_attr; /**<Message queue attributes */


/**
 * @brief Initializes the thread parameters with defined priority
 * 
 * @param priority Thread priority
 * @param pthread_attr Thread attributes structure
 * @param pthread_param Thread parameters structure
 */
void initThread(int priority, pthread_attr_t *pthread_attr, struct sched_param *pthread_param);

/**
 * @brief Check for errors upon creating threads
 * 
 * @param status : Value returned from functions
 */
void checkErrors(int status);

/**
 * @brief Proces signal handler
 * 
 * @param signo Signal received
 */
static void signalHandler(int signo) 
{
	switch (signo)
	{
	    case (SIGALRM): //Timer has ended
            pthread_mutex_lock(&motorFlag_mutex);
            motorFlag = 0;
            send_swing_flag(motorFlag); //Update the motor flag on the database
            pthread_mutex_unlock(&motorFlag_mutex);
            printf("Motor flag was updated in the database.\n");
		break;

        case (SIGUSR1): //Signal received from Daemon
            send_notification_flag(1); //Update notification flag on the database
            printf("Notification flag was updated in the database.\n");
		break;

        case (SIGINT):
            printf("Terminating program...\n");
            mq_unlink(MSGQOBJ_NAME); //unlink msgQ
            stopMotor(); //stop motor
            stopLivestream(); //stop livestream
            endServer(); //close server
            remMotor(); //remove motor device driver
            remDHT11(); //remove sensor device driver
            kill(daemonPID,SIGTERM); //kill daemon
            if(Py_IsInitialized())
                Py_FinalizeEx(); //close python interpreter
            exit(1);
        break;
	}
}

/**
 * @brief Reads from temperature and humidity sensor and stores values
 * 
 * If sampling fails, there will be another 2 consecutive samplings. If it
 * fails again, the thread will go into sleep until next sample time
 */
void *tReadSensor(void *arg)
{
    dht11_t sensorSampling; 
    float temperature = 0;
    float humidity = 0;

    u_int8_t samplingTries = 0;

    while(1)
    {
        do{
            if(readDHT11(&sensorSampling)) //Read from the sensor
            {
                //Sampling successful
                samplingTries = 0;
                printf("Sensor sampling successful:\n");
            }
            else
            {
                //Sampling failed
                samplingTries++;
                fprintf(stderr,"Failed to sample sensor...Trying again in 5 seconds.\n");
                sleep(5);   
            }
        }while(samplingTries > 0 && samplingTries < 3); //Upon failure tries a maximum of 3 times before going to sleep

        if(!samplingTries) //If sampling was successful
        {
            temperature = sensorSampling.TemperatureI + (float)(sensorSampling.TemperatureD)/100;
            humidity = sensorSampling.HumidityI + (float)(sensorSampling.HumidityD)/100;

            printf("\tTemperature: %.2f \ºC \n \tHumidity: %.2f \%\n", temperature, humidity);

            //If the temperature or humidity values are different from those on the database, update them
            if( (temperature != databaseTemperature) || (humidity != databaseHumidity) )
            {
                pthread_mutex_lock(&sensorFlag_mutex);
                databaseTemperature = temperature;
                databaseHumidity = humidity;
                sensorFlag = 1;
                pthread_mutex_unlock(&sensorFlag_mutex);
            }
        }
        else if(samplingTries == 3) //If sampling failed the 3 attempts
        {
            samplingTries = 0;
            fprintf(stderr,"Failed to sample sensor 3 times. Ignoring...\n");    
        }

        /* Sleep for sensorSample minutes */
        sleep(60 * sensorSample); 
    }   
}

/**
 * @brief Controls the livestream depending on livestream flag value
 */
void *tStartStopStream (void *arg)
{
    while(1)
    {
        if(streamFlag && !getStreamStatus()) //If streamFlag is on
        {
            if(startLivestream())
                fprintf(stderr, "Error when starting stream.\n");
            else
                printf("Livestream has started.\n");
        }
        else if(!streamFlag && getStreamStatus()) //If streamFlag is off
        {
            stopLivestream();
            printf("Livestream has stopped.\n");
        }
    }
}

/**
 * @brief Controls the motor depending on swing flag value
 */
void *tStartStopMotor (void *arg)
{
    struct itimerval itv;
    while(1)
    {   
        if(motorFlag && !getMotorStatus()) //If motorFlag is on
        {
            if(startMotor())
            {           
                itv.it_interval.tv_sec = 60 * motorTimeout;
                itv.it_interval.tv_usec = 0;
                itv.it_value.tv_sec = 60 * motorTimeout;
                itv.it_value.tv_usec = 0;
                setitimer (ITIMER_REAL, &itv, NULL); //Start timer
                printf("Motor has started.\n");
            }
            else
                fprintf(stderr, "Error when starting motor.\n");
        }
        else if(!motorFlag && getMotorStatus()) //If motorFlag is off
        {
            if(stopMotor())
            {
                itv.it_interval.tv_sec = 0;
                itv.it_interval.tv_usec = 0;
                itv.it_value.tv_sec = 0;
                itv.it_value.tv_usec = 0;
                setitimer (ITIMER_REAL, &itv, NULL); //Stop timer
                printf("Motor has stopped.\n");
            }
            else
                fprintf(stderr, "Error when stopping motor.\n");
        }
    }
}

/**
 * @brief Update values to/from database
 * 
 * Updates temperature and humidity in database, 
 * get livestream flag and swing flag values from
 * database
 */
void *tUpdateDatabase(void *arg)
{
    int ret;
    _Bool dMotorFlag, dStreamFlag;
    Py_Initialize(); //Initialize the Python Interpreter
    ret = initDatabase(); //Initialize the Database functions
    if(ret == -ERROR)
    {
        fprintf(stderr, "In initDatabase()\n");
        exit(1);
    }
    while(1)
    {
        /*  
        *   Update temperature and humidity 
        *   in the database
        */
       pthread_mutex_lock(&sensorFlag_mutex);
        if(sensorFlag)
        {
                sensorFlag = 0;
                send_temp_hum(databaseTemperature,databaseHumidity);
                printf("Temperature and Humidity were updated in the database.\n");
        }
        pthread_mutex_unlock(&sensorFlag_mutex);

        /*  
        *   Update motorFlag with the value 
        *   from the database
        */
        dMotorFlag = get_swing_flag();
        pthread_mutex_lock(&motorFlag_mutex);
        motorFlag = dMotorFlag;
        pthread_mutex_unlock(&motorFlag_mutex);
        
        /*  
        *   Update streamFlag with the value 
        *   from the database. If it changes,
        *   the new value is sent to the daemon
        *   via message queue
        */
        dStreamFlag = get_live_flag();
        pthread_mutex_lock(&streamFlag_mutex);
        if(streamFlag != dStreamFlag)
        {
            printf("Sending livestream flag update to daemon...\n");
            char *msg;
            msg = malloc(4);
            if(msg == NULL)
            {
                fprintf(stderr,"malloc() got error.\n");
            }
            else
            {
                //Set up the command LV-x
                if(dStreamFlag)
                    strcpy(msg,"LV-1");
                else
                    strcpy(msg,"LV-0");

                ret = mq_send(msgq_id, msg, strlen(msg)+1, 1); //Send to the message queue
                if(ret != 0)
                {
                    fprintf(stderr,"mq_send() got error %d\n",ret);
                    errno=ret;
                    perror("mq_send()");
                }
            }
            free(msg);
        }
        streamFlag = dStreamFlag;
        pthread_mutex_unlock(&streamFlag_mutex);
        
        /* Sleep for 5 seconds */
        sleep(5); 
    }
    mq_close(msgq_id);
    Py_FinalizeEx(); // for redundancy purposes
}

int main (int argc, char *argv[])
{
    int ret;

    msgq_attr.mq_flags = 0;  
    msgq_attr.mq_maxmsg = 1;  
    msgq_attr.mq_msgsize = 5;  
    msgq_attr.mq_curmsgs = 0; 
    msgq_id = mq_open(MSGQOBJ_NAME, O_WRONLY | O_CREAT | O_EXCL | O_NONBLOCK, S_IRWXU | S_IRWXG | S_IRWXO, &msgq_attr);
    if(msgq_id == EEXIST) //If the message queue already exists
    {
        mq_unlink(MSGQOBJ_NAME); //Unlink the message queue
        msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO, NULL);
    }
    if (msgq_id == (mqd_t)-1) {
        perror("In mq_open()");
        exit(1);
    }
  
    pid_t my_pid = getpid();
    printf("PID: %d\n",my_pid);

    /*
    *   Open shared memory to drop PID for Daemon
    *   and read Daemon's PID
    */
    int fd;
    fd = shm_open(SHMEMOBJ_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if(fd < 0)
    {
        mq_unlink(MSGQOBJ_NAME);
        fprintf(stderr, "Error %s in shm_open()\n",errno);
        exit(1);
    }
    ftruncate(fd, sizeof(pid_t));
    void* ptr;
    ptr = mmap(0, sizeof(pid_t),PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if(ptr == (void *)-1)
    {
        mq_unlink(MSGQOBJ_NAME);
        shm_unlink(SHMEMOBJ_NAME);
        fprintf(stderr, "Error %s in mmap()\n",errno);
        exit(1);
    }
    sprintf(ptr, "%d", my_pid);

    /*
    *   Set Python Path to current Working Path
    */
    setenv("PYTHONPATH",".",1);

    /*
    *   Call the Daemon
    */
    system("./daemon.elf");

    /*
    *   Read Daemon's PID
    */
    daemonPID = atoi((char*)ptr);
    ret = munmap(0, sizeof(pid_t));
    if(ret < 0)
    {
        mq_unlink(MSGQOBJ_NAME);
        kill(daemonPID, SIGTERM);
        fprintf(stderr, "Error %s in munmap()\n",errno);
        exit(1);
    }
    ret = shm_unlink(SHMEMOBJ_NAME);
    if(ret < 0)
    {
        mq_unlink(MSGQOBJ_NAME);
        kill(daemonPID, SIGTERM);
        fprintf(stderr, "Error %s in shm_unlink()\n",errno);
        exit(1);
    }

    /*
    *   Define the signal handler
    *       @ SIGALRM for the motor timeout
    *       @ SIGUSR for the notification generation request
    *       from the Daemon
    *       @ SIGINT to kill the program upon receiving CTRL + C
    */
    signal(SIGALRM, signalHandler);
    signal(SIGUSR1, signalHandler);
    signal(SIGINT, signalHandler);

    /*
    *   Call the initialization functions for the modules
    *       @ Streaming
    *       @ DHT11 Sensor
    *       @ Motor Driver
    */
    initServer();
    initDHT11();
    initMotor();

    /*
    *   Threads initialization with predefined priorities:
    *
    *       @ tReadSensor with priority sensorPrio
    *       @ tStartStopStream with priority streamPrio
    *       @ tStartStopMotor with priority motorPrio
    *       @ tUpdateDatabase with priority updateDatabasePrio
    *   
    *   Create threads and detach them
    */
    int anyError=0;
    pthread_attr_t thread_attr;
	struct sched_param thread_param;

	pthread_t readSensorID, StartStopStreamID, updateDatabaseID, StartStopMotorID;

	pthread_attr_init (&thread_attr);
	pthread_attr_getschedparam (&thread_attr, &thread_param);

	initThread(sensorPrio,&thread_attr,&thread_param);
	anyError = pthread_create (&readSensorID, &thread_attr, tReadSensor, NULL);
    checkErrors(anyError); //Check for errors when creating thread

	initThread(streamPrio,&thread_attr,&thread_param);
    anyError = pthread_create (&StartStopStreamID, &thread_attr, tStartStopStream, NULL);
    checkErrors(anyError); //Check for errors when creating thread

    initThread(motorPrio,&thread_attr,&thread_param);
    anyError = pthread_create (&StartStopMotorID, &thread_attr, tStartStopMotor, NULL);
    checkErrors(anyError); //Check for errors when creating thread

    initThread(updateDatabasePrio,&thread_attr,&thread_param);
    anyError = pthread_create (&updateDatabaseID, &thread_attr, tUpdateDatabase, NULL);
    checkErrors(anyError); //Check for errors when creating thread
	
    pthread_detach (updateDatabaseID);
	pthread_detach(StartStopStreamID);
    pthread_detach (StartStopMotorID);
    pthread_detach (readSensorID);

	while(1){}
}

void initThread(int priority, pthread_attr_t *pthread_attr, struct sched_param *pthread_param)
{
	int min = sched_get_priority_min (SCHED_RR);
	int max = sched_get_priority_max (SCHED_RR);

    if(priority < min || priority > max) //If priority exceeds the limits
    {
        fprintf(stderr,"Thread priorities not valid.\n");
        perror("initThread()");
        exit(1);
    }
    pthread_param->sched_priority = priority;
    pthread_attr_setschedparam(&pthread_attr, &pthread_param); //Set the thread's priority
}

void checkErrors(int status)
{
	if(status)
	{     
        fprintf(stderr,"pthread_create() got error %d\n",status);
        errno=status;
        perror("pthread_create()");
        exit(1);    		
  	}
}
