#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h> 
#include <sys/stat.h>
#include <linux/types.h>
#include <signal.h>
#include "./inc/database.h"
#include "./inc/wifi.h"
#include "./inc/microphone.h"
#include "./inc/livestream.h"
#include "./inc/motor.h"
#include "./inc/dht.h"

#define MSGQOBJ_NAME    "/mqLocalDaemon"
#define MAX_MSG_LEN     15

/* priorites of each thread */
#define streamPrio 2
#define motorPrio 2
#define updateflagsPrio 3
#define sensorPrio 4

bool motorFlag = 0;
bool streamFlag = 0;

pthread_mutex_t motorFlag_mutex = PTHREAD_MUTEX_INITIALIZER; //shared variable (motorFlag)
pthread_mutex_t streamFlag_mutex = PTHREAD_MUTEX_INITIALIZER; //shared variable (streamFlag)

int daemonPID;

static void signalHandler(int signo) 
{
	switch (signo)
	{
	    case (SIGALRM):
		break;

        case (SIGUSR1):
		break;
	}
}

void *tReadSensor(void *arg)
{
    while(1)
    {

    }

}

void *tStartStopStream (void *arg)
{
    while(1)
    {

    }
}

void *tStartStopMotor (void *arg)
{
    while(1)
    {

    }
}

void *tUpdateFlags(void *arg)
{
    while(1)
    {

    }
}

void initThread(int priority, pthread_attr_t *pthread_attr, struct sched_param *pthread_param);
void checkErrors(int status);

int main (int argc, char *argv[])
{
    /*
    *   Open message queue to drop PID for Daemon
    */
    mqd_t msgq_id;
    pid_t my_pid = getpid();
    
    msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
    if (msgq_id == (mqd_t)-1) {
        perror("In mq_open()");
        exit(1);
    }

    mq_send(msgq_id, my_pid, 1, 1);
    mq_close(msgq_id);

    /*
    *   Call the Daemon
    */
    system("./daemon");
    
    /*
    *   Define the signal handler
    *       @ SIGALRM for the motor timeout
    *       @ SIGUSR for the notification generation request
    *       from the Daemon
    */
    signal(SIGALRM, signalHandler);
    signal(SIGUSR1, signalHandler);

    /*
    *   Threads initialization with predefined priorities:
    *
    *       @ tReadSensor with priority sensorPrio
    *       @ tStartStopStream with priority streamPrio
    *       @ tStartStopMotor with priority motorPrio
    *       @ tUpdateFlags with priority updateflagsPrio
    *   
    *   Create threads and detach them
    */
    int anyError=0;
   	int thread_policy;
    pthread_attr_t thread_attr;
	struct sched_param thread_param;

	pthread_t readSensorID, StartStopStreamID, updateFlagsID, StartStopMotorID;

	pthread_attr_init (&thread_attr);
	pthread_attr_getschedpolicy (&thread_attr, &thread_policy);
	pthread_attr_getschedparam (&thread_attr, &thread_param);

	SetupThread(sensorPrio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
	anyError = pthread_create (&readSensorID, &thread_attr, tReadSensor, NULL);
    checkErrors(anyError);

	SetupThread(streamPrio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
    anyError = pthread_create (&StartStopStreamID, &thread_attr, tStartStopStream, NULL);
    checkErrors(anyError);

    SetupThread(motorPrio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
    anyError = pthread_create (&StartStopMotorID, &thread_attr, tStartStopMotor, NULL);
    checkErrors(anyError);

    SetupThread(updateflagsPrio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
    anyError = pthread_create (&updateFlagsID, &thread_attr, tUpdateFlags, NULL);
    checkErrors(anyError);

	pthread_detach (updateFlagsID);
	pthread_detach(StartStopStreamID);
    pthread_detach (StartStopMotorID);
    pthread_detach (readSensorID);

	while(1){}
}

void initThread(int priority, pthread_attr_t *pthread_attr, struct sched_param *pthread_param)
{
	int min, max;

	pthread_attr_setschedpolicy (pthread_attr, SCHED_RR);
	min = sched_get_priority_min (SCHED_RR);
	max = sched_get_priority_max (SCHED_RR);

    if(priority < min || priority > max)
    {
        fprintf(stderr,"Thread priorities not valid.\n");
        perror("initThread()");
        exit(1);
    }

	pthread_param->sched_priority = priority;

	pthread_attr_setschedparam (pthread_attr, pthread_param);
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