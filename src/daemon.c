/**
 * @file daemon.c
 * @author José Carvalho, João Carneiro
 * @brief Daemon implementation
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#include <sys/syslog.h>
#include <stdio.h>
#include <sys/socket.h>
#include <mqueue.h> /* mq_* functions */
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
#include <sys/mman.h>
#include <sys/shm.h>
#include "../inc/microphone.h"

#define detectCryingPrio 3 /**<tDetectCrying priority*/
#define watchFlagPrio 2 /**< tWatchFlag priority */

#define MSGQOBJ_NAME "/mqLocalDaemon" /**< Message queue name */
#define SHMEMOBJ_NAME "/shLocalDaemon" /**< Shared memory name */

mqd_t msgq_id; /**< Message queue ID */

_Bool isStreamActive = 0; /**< Defines if livestream is active in main process */

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
 * @brief Process signal handler
 * 
 * @param signo : Signal received
 */
static void signalHandler(int signo)
{
    switch (signo)
    {
    case (SIGTERM):
        syslog(LOG_INFO, "SIGTERM received. Closing daemon...\n");
        exit(1);
    }
}

/**
 * @brief Detects crying from microphone
 * 
 * @param local_pid : Local process ID
 */
void *tDetectCrying(void *local_pid)
{
    int ret = 0;
    while (1)
    {
        if (!isStreamActive) //If streaming is not active
        {
            float loudness = 0;

            syslog(LOG_INFO, "Recording will start.\n");
            ret = startRecording();
            sleep(2);
            if(!ret)
            {
                syslog(LOG_INFO, "Recording ended.\n");  
                ret = processAudio(&loudness);
                switch(ret)
                {
                    case 1: //Loudness exceeds the limit
                        syslog(LOG_INFO, "Signaling local system at PID: %d\n", local_pid);
                        kill(local_pid, SIGUSR1);
                    case 0: //Loudness does not exceed the limit
                        syslog(LOG_INFO, "Audio process returned: %d | loudness : %.3f\n", ret, loudness);
                        break;     
                    case ERR_OPEN: //Error in processAudio()
                        syslog(LOG_ERR, "Error when opening file audiorecord.wav.\n" );
                        break;
                    case ERR_READ: //Error in processAudio()
                        syslog(LOG_ERR, "Error when reading from file audiorecord.wav.\n" );
                        break;
                    default: //Error in processAudio()
                        syslog(LOG_ERR, "In processAudio()\n" );
                        break;
                }
            }
            else
                syslog(LOG_ERR, "In startRecording().\n");  
        }

        /* Sleep for 2 seconds */
        sleep(2);
    }
}

/**
 * @brief Watches message queue for any changes in livestream flag
 */
void *tWatchStreamFlag(void *arg)
{
    struct mq_attr msgq_attr;
    char *msg;
    const char msg_type[4] = "LV-"; //Command structure
    unsigned int ret;

    while (1)
    {
        mq_getattr(msgq_id, &msgq_attr);
        if (msgq_attr.mq_curmsgs) //If there are messages on the message queue
        {
            msg = malloc(6);
            ret = mq_receive(msgq_id, msg, 6, NULL); //Receive the message
            if (ret > 0)
            {
                syslog(LOG_INFO, "Command received: %s\n", msg);
                if (!strncmp(msg, msg_type, strlen(msg_type))) //Checks if the command is valid
                {
                    syslog(LOG_INFO, "Command valid.\n");
                    isStreamActive = msg[3]; //Update variable with the current streamFlag value
                    syslog(LOG_INFO, "Streaming is now %d.\n", isStreamActive);
                }
                else
                    syslog(LOG_ERR, "Command not valid.\n");
            }
            else
            {
                errno = ret;
                syslog(LOG_ERR, "Error (%d) when receiving message.\n",errno);
            }
            free(msg);
        }

        /* Sleep for 3 seconds */
        sleep(3);
    }
}

int main(int argc, char *args[])
{
    pid_t localPID = 0;
    int ret;

    msgq_id = mq_open(MSGQOBJ_NAME, O_RDONLY | O_NONBLOCK);
    if (msgq_id == (mqd_t)-1) {
        syslog(LOG_ERR, "mq_open() got error %d\n",msgq_id);
        exit(1);
    }
    
    /*
    *   Define the signal handler to received SIGTERM from
    *   the main process
    */
    signal(SIGTERM, signalHandler);

    /*
    *   Fork, create new session, change root dir,
    *   umask and close file descriptors
    */
    pid_t pid, sid;

    pid = fork(); // create a new process
    if (pid < 0)
    { // on error exit
        syslog(LOG_ERR, "%s\n", "fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        printf("Deamon PID: %d\n", pid);

        // parent process (exit)
        exit(EXIT_SUCCESS);
    }
    sid = setsid(); // create a new session
    if (sid < 0)
    { // on error exit
        syslog(LOG_ERR, "%s\n", "setsid");
        exit(EXIT_FAILURE);
    }

    // make '/' the root directory
    if (chdir("/") < 0)
    { // on error exit
        syslog(LOG_ERR, "%s\n", "chdir");
        exit(EXIT_FAILURE);
    }
    umask(0);

    close(STDIN_FILENO);  // close standard input file descriptor
    close(STDOUT_FILENO); // close standard output file descriptor
    close(STDERR_FILENO); // close standard error file descriptor

    /*
    *   Opens shared memory to read local's PID
    *   and send Daemon's PID
    */
    int fd;
    fd = shm_open(SHMEMOBJ_NAME, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    void *ptr;
    ptr = mmap(0, sizeof(pid_t), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if(ptr == (void *)-1)
    {
        shm_unlink(SHMEMOBJ_NAME);
        syslog(LOG_ERR, "Error %s in mmap()\n",errno);
        exit(1);
    }
    localPID = atoi((char *)ptr);
    sprintf(ptr, "%d", getpid());
    ret = munmap(0, sizeof(pid_t));
    if(ret < 0)
    {
        shm_unlink(SHMEMOBJ_NAME);
        syslog(LOG_ERR, "Error %s in munmap()\n",errno);
        exit(1);
    }
    syslog(LOG_INFO, "Received local PID: %d\n",localPID);

    /*
    *   Threads initialization with predefined priorities:
    *
    *       @ tDetectCrying with priority detectCryingPrio
    *       @ tWatchStreamFlag with priority watchFlagPrio
    *   
    *   Create threads and detach them
    */
    int anyError = 0;
    pthread_attr_t thread_attr;
    struct sched_param thread_param;

    pthread_t detectCryingID, watchStreamFlagID;

    pthread_attr_init(&thread_attr);
    pthread_attr_getschedparam(&thread_attr, &thread_param);

    initThread(watchFlagPrio, &thread_attr, &thread_param);
    anyError = pthread_create(&watchStreamFlagID, &thread_attr, tWatchStreamFlag, NULL);
    checkErrors(anyError); //Check for errors when creating thread

    initThread(detectCryingPrio, &thread_attr, &thread_param);
    anyError = pthread_create(&detectCryingID, &thread_attr, tDetectCrying, (void *)localPID);
    checkErrors(anyError); //Check for errors when creating thread

    pthread_detach(watchStreamFlagID);
    pthread_detach(detectCryingID);

    while (1){}
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