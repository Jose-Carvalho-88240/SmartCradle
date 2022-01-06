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
#include "../inc/wifi.h"
#include "../inc/microphone.h"
#include "../inc/livestream.h"
#include "../inc/motor.h"
#include "../inc/dht.h"

#define MSGQOBJ_NAME    "/mqLocalDaemon"
#define SHMEMOBJ_NAME "/shLocalDaemon"
#define MAX_MSG_LEN     128

/* priorites of each thread */
#define streamPrio 2
#define motorPrio 2
#define updateDatabasePrio 3
#define sensorPrio 4

#define motorTimeout 10 /* Minutes */
#define sensorSample 10 /* Minutes */

_Bool motorFlag = 0;
_Bool streamFlag = 0;
_Bool sensorFlag = 0;

pthread_mutex_t motorFlag_mutex = PTHREAD_MUTEX_INITIALIZER; /* shared variable (motorFlag) */
pthread_mutex_t streamFlag_mutex = PTHREAD_MUTEX_INITIALIZER; /* shared variable (streamFlag) */
pthread_mutex_t sensorFlag_mutex = PTHREAD_MUTEX_INITIALIZER; /* shared variable (sensorFlag) */

float databaseTemperature = 0;
float databaseHumidity = 0;

pid_t daemonPID;

mqd_t msgq_id;
struct mq_attr msgq_attr;

static void signalHandler(int signo) 
{
	switch (signo)
	{
	    case (SIGALRM):
            pthread_mutex_lock(&motorFlag_mutex);
            motorFlag = 0;
            send_swing_flag(motorFlag);
            pthread_mutex_unlock(&motorFlag_mutex);
            printf("Motor flag was updated in the database.\n");
		break;

        case (SIGUSR1):
            send_notification_flag(1);
            printf("Notification flag was updated in the database.\n");
		break;

        case (SIGINT):
            printf("Terminating program...\n");
            mq_unlink(MSGQOBJ_NAME); //unlink msg q
            stopMotor(); //stop motor
            stopLivestream(); //stop livestream
            endServer(); //close server
            remMotor(); //remove motor device driver
            remDHT11(); //remove sensor device driver
            const char cmd[30];
            sprintf(cmd, "killall daemon.elf"); 
            system(cmd); //kill Daemon
            Py_FinalizeEx(); //close python interpreter
            exit(1);
        break;
	}
}

void *tReadSensor(void *arg)
{
    dht11_t sensorSampling; 
    float temperature = 0;
    float humidity = 0;

    u_int8_t samplingTries = 0;

    while(1)
    {
        if(readDHT11(&sensorSampling))
        {
            samplingTries = 0;
             printf("Sensor sampling successful:\n");
        }
        else
        {
            samplingTries++;
            fprintf(stderr,"Failed to sample sensor...Trying again.\n");
            perror("readDHT11()");       
        }

        if(!samplingTries)
        {
            temperature = sensorSampling.TemperatureI + (float)(sensorSampling.TemperatureD)/100;
            humidity = sensorSampling.HumidityI + (float)(sensorSampling.HumidityD)/100;

            printf("\tTemperature: %.2f \ºC \n \tHumidity: %.2f \%\n", temperature, humidity);
            if( (temperature != databaseTemperature) || (humidity != databaseHumidity) )
            {
                pthread_mutex_lock(&sensorFlag_mutex);
                databaseTemperature = temperature;
                databaseHumidity = humidity;
                sensorFlag = 1;
                pthread_mutex_unlock(&sensorFlag_mutex);
            }
        }
        else if(samplingTries == 3)
        {
            samplingTries = 0;
            fprintf(stderr,"Failed to sample sensor 3 times. Ignoring...\n");
            perror("tReadSensor()");     
        }

        /* Pause for sensorSample minutes */
        sleep(60 * sensorSample); 
    }   

}

void *tStartStopStream (void *arg)
{
    while(1)
    {
        if(streamFlag && !getStreamStatus())
        {
            if(startLivestream())
                fprintf(stderr, "Error when starting stream.\n");
            else
                printf("Livestream has started.\n");
        }
        else if(!streamFlag && getStreamStatus())
        {
            stopLivestream();
            printf("Livestream has stopped.\n");
        }
    }
}

void *tStartStopMotor (void *arg)
{
    struct itimerval itv;
    while(1)
    {   
        if(motorFlag && !getMotorStatus())
        {
            if(startMotor())
            {           
                itv.it_interval.tv_sec = 60 * motorTimeout;
                itv.it_interval.tv_usec = 0;
                itv.it_value.tv_sec = 60 * motorTimeout;
                itv.it_value.tv_usec = 0;
                setitimer (ITIMER_REAL, &itv, NULL);

                printf("Motor has started.\n");
            }
            else
                fprintf(stderr, "Error when starting motor.\n");
        }
        else if(!motorFlag && getMotorStatus())
        {
            if(stopMotor())
            {
                itv.it_interval.tv_sec = 0;
                itv.it_interval.tv_usec = 0;
                itv.it_value.tv_sec = 0;
                itv.it_value.tv_usec = 0;
                setitimer (ITIMER_REAL, &itv, NULL);

                printf("Motor has stopped.\n");
            }
            else
                fprintf(stderr, "Error when stopping motor.\n");
        }
    }
}

void *tUpdateDatabase(void *arg)
{
    unsigned int ret;
    _Bool dMotorFlag, dStreamFlag;
    Py_Initialize();
    initDatabase();

    while(1)
    {
        /*  
        *   Update temperature and humidity 
        *   of the database
        */
        if(sensorFlag)
        {
                pthread_mutex_lock(&sensorFlag_mutex);
                sensorFlag = 0;
                send_temp_hum(databaseTemperature,databaseHumidity);
                pthread_mutex_unlock(&sensorFlag_mutex);
                printf("Temperature and Humidity were updated in the database.\n");
        }

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
                free(msg);
                fprintf(stderr,"malloc() got error.\n");
                break;
            }

            if(dStreamFlag)
                strcpy(msg,"LV-1");
            else
                strcpy(msg,"LV-0");
            ret = mq_send(msgq_id, msg, strlen(msg)+1, 1);
            if(ret != 0)
            {
                fprintf(stderr,"mq_send() got error %d\n",ret);
                errno=ret;
                perror("mq_send()");
            }
            free(msg);
        }
        streamFlag = dStreamFlag;
        pthread_mutex_unlock(&streamFlag_mutex);
        
        /* Pause for 5 seconds */
        sleep(5); 
    }
    mq_close(msgq_id);
    Py_FinalizeEx(); // for redundancy purposes
}

void initThread(int priority, pthread_attr_t *pthread_attr, struct sched_param *pthread_param);
void checkErrors(int status);

int main (int argc, char *argv[])
{
    msgq_attr.mq_flags = 0;  
    msgq_attr.mq_maxmsg = 1;  
    msgq_attr.mq_msgsize = 5;  
    msgq_attr.mq_curmsgs = 0; 
    msgq_id = mq_open(MSGQOBJ_NAME, O_WRONLY | O_CREAT | O_EXCL | O_NONBLOCK, S_IRWXU | S_IRWXG | S_IRWXO, &msgq_attr);
    if(msgq_id == EEXIST)
    {
        mq_unlink(MSGQOBJ_NAME);
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
    fd = shm_open(SHMEMOBJ_NAME, O_RDWR | O_CREAT | O_EXCL, 0666);
    ftruncate(fd, sizeof(pid_t));
    void* ptr;
    ptr = mmap(0, sizeof(pid_t),PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
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
    munmap(0, sizeof(pid_t));
    shm_unlink(SHMEMOBJ_NAME);

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
    checkErrors(anyError);

	initThread(streamPrio,&thread_attr,&thread_param);
    anyError = pthread_create (&StartStopStreamID, &thread_attr, tStartStopStream, NULL);
    checkErrors(anyError);

    initThread(motorPrio,&thread_attr,&thread_param);
    anyError = pthread_create (&StartStopMotorID, &thread_attr, tStartStopMotor, NULL);
    checkErrors(anyError);

    initThread(updateDatabasePrio,&thread_attr,&thread_param);
    anyError = pthread_create (&updateDatabaseID, &thread_attr, tUpdateDatabase, NULL);
    checkErrors(anyError);
	
    pthread_detach (updateDatabaseID);
	pthread_detach(StartStopStreamID);
    pthread_detach (StartStopMotorID);
    pthread_detach (readSensorID);

	while(1){}
}

void initThread(int priority, pthread_attr_t *pthread_attr, struct sched_param *pthread_param)
{
	int min, max;

	min = sched_get_priority_min (SCHED_RR);
	max = sched_get_priority_max (SCHED_RR);

    if(priority < min || priority > max)
    {
        fprintf(stderr,"Thread priorities not valid.\n");
        perror("initThread()");
        exit(1);
    }

    pthread_param->sched_priority = priority;

    pthread_attr_setschedparam(&pthread_attr, &pthread_param);
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
