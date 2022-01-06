#include "../inc/livestream.h"
#include <stdlib.h>
#include <sys/wait.h>

static _Bool streamRunning; /* Status of the stream */

void initServer()
{
    system("nginx");
    streamRunning = 0;
}

void endServer()
{
    system("nginx -s stop");
}

int startLivestream()
{
    if(!streamRunning)
    {
        int ret = 0;
        
        ret = system("ffmpeg -re \
        -f alsa -ac 1 -thread_queue_size 1024 -ar 44100 -i plughw:0,0 \
        -f v4l2 -video_size 320x240 -thread_queue_size 16384 -i /dev/video0 \
        -c:a copy -c:v h264 -b:v 2048k -preset ultrafast -filter:v fps=fps=30 -tune zerolatency \
        -f flv rtmp:localhost/live/bbb \
        -nostdin -nostats > /var/log/ffmpeg_output.log 2>&1 < /dev/null &");

        //return value from the command on the upper 8-bits of the return value
        // 0 on sucess, 1 on failure
        if(WEXITSTATUS(ret))
            return 1;

        streamRunning = 1;
    }
    return 0;
}

void stopLivestream()
{
    if(streamRunning)
    {
        system("pidof ffmpeg | xargs kill -9");
        streamRunning = 0;
    }
}

_Bool getStreamStatus()
{
    return streamRunning;
}