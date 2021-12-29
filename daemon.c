#include  <sys/syslog.h>
#include <stdio.h>
#include <sys/socket.h>
#include <mqueue.h>	/* mq_* functions */
#include <sys/types.h>
#include <resolv.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define detectCryingPrio 2
#define watchFlagPrio 3

#define MSGQOBJ_NAME    "/mqLocalDaemon"
#define MAX_MSG_LEN 15

mqd_t msgq_id;
struct mq_attr msgq_attr;
int localPID;

bool isStreamActive = 0;

void *tDetectCrying(void *arg)
{
    int ret = 0;
    while(1)
    {
        if(!isStreamActive)
        {
            startRecording();
            ret = processAudio();
            if(ret)
                kill(localPID, SIGUSR1);
        }
        sleep(2); 
    }
}

void *tWatchStreamFlag(void *arg)
{
    msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR | O_CREAT , S_IRWXU | S_IRWXG, NULL);
    if (msgq_id == (mqd_t)-1) {
		perror("In mq_open()");
		exit(1);
	}

    while(1)
    {
        mq_getattr(msgq_id, &msgq_attr);
        if(msgq_attr.curmsgs)
        {
           mq_receive(msgq_id, isStreamActive, 1, 1);
        }
    }
}

void initThread(int priority, pthread_attr_t *pthread_attr, struct sched_param *pthread_param);
void checkErrors(int status);

int main(int argc, char *args[])
{
    /*
    *   Open message queue to read local's PID
    */
	msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR | O_CREAT , S_IRWXU | S_IRWXG, NULL);
	if (msgq_id == (mqd_t)-1) {
        fprintf(stderr,"Message Queue ID is not valid.\n");
		perror("In mq_open()");
		exit(1);
	}

	mq_getattr(msgq_id, &msgq_attr);
    if(!msgq_attr.curmsgs)
    {
        fprintf(stderr,"No messages to be received.\n");
		exit(1);
    }
    mq_receive(msgq_id, localPID, 1, 1);
    mq_close(msgq_id);

    /*
    *   Fork, create new session, change root dir,
    *   umask and close file descriptors
    */
    pid_t pid, sid;
    
    pid = fork(); // create a new process

    if (pid < 0) { // on error exit
        syslog(LOG_ERR, "%s\n", "fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0){  
        printf("Deamon PID: %d\n", pid);	
        exit(EXIT_SUCCESS); // parent process (exit)
    }
    sid = setsid(); // create a new session

    if (sid < 0) { // on error exit
        syslog(LOG_ERR, "%s\n", "setsid");
        exit(EXIT_FAILURE);
    }
    // make '/' the root directory
    if (chdir("/") < 0) { // on error exit
        syslog(LOG_ERR, "%s\n", "chdir");
        exit(EXIT_FAILURE);
    }
    umask(0);
    close(STDIN_FILENO);  // close standard input file descriptor
    close(STDOUT_FILENO); // close standard output file descriptor
    close(STDERR_FILENO); // close standard error file descriptor

    /*
    *   Threads initialization with predefined priorities:
    *
    *       @ tDetectCrying with priority detectCryingPrio
    *       @ tWatchStreamFlag with priority watchFlagPrio
    *   
    *   Create threads and detach them
    */
    int anyError=0;
   	int thread_policy;
    pthread_attr_t thread_attr;
	struct sched_param thread_param;

	pthread_t detectCryingID, watchStreamFlagID;

	pthread_attr_init (&thread_attr);
	pthread_attr_getschedpolicy (&thread_attr, &thread_policy);
	pthread_attr_getschedparam (&thread_attr, &thread_param);

	initThread(detectCryingPrio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
	anyError = pthread_create (&detectCryingID, &thread_attr, tDetectCrying, NULL);
    checkErrors(anyError);

	initThread(watchFlagPrio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
    anyError = pthread_create (&watchStreamFlagID, &thread_attr, tWatchStreamFlag, NULL);
    checkErrors(anyError);

	pthread_detach (detectCryingID);
	pthread_detach(watchStreamFlagID);

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