/**
 * @file livestream.c
 * @author José Carvalho, João Carneiro
 * @brief Livestream module implementation
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#include <stdlib.h>
#include <sys/wait.h>
#include "../inc/livestream.h"

static _Bool streamRunning = 0; /* Status of the stream */

void startLivestream()
{
    if(!streamRunning)
    {
        system("ffmpeg -re \
        -f alsa -ac 1 -thread_queue_size 1024 -ar 44100 -i plughw:0,0 \
        -f v4l2 -video_size 320x240 -thread_queue_size 16384 -i /dev/video0 \
        -c:a copy -c:v h264 -b:v 2048k -preset ultrafast -filter:v fps=fps=30 -tune zerolatency \
        -f flv rtmp:localhost/live \
        -nostdin -nostats > /var/log/ffmpeg_output.log 2>&1 < /dev/null &");

        streamRunning = 1;
    }
}

void stopLivestream()
{
    if(streamRunning)
    {
        /* Get ffmpeg PID and kill the process */
        system("pidof ffmpeg | xargs kill -9"); //9 = SIGKILL
        streamRunning = 0;
    }
}

_Bool getStreamStatus()
{
    return streamRunning;
}