#include "../inc/livestream.h"
#include <stdlib.h>

static _Bool streamRunning;

void initStream()
{
    system("nginx");
    streamRunning = 0;
}

void startLivestream()
{
    streamRunning = 1;
    system("ffmpeg -re \
    -f alsa -ac 1 -thread_queue_size 2048 -ar 44100 -i plughw:1,0 \
    -f v4l2 -video_size 320x240 -thread_queue_size 16384 -i /dev/video0 \
    -c:a copy -c:v h264 -b:v 2048k -preset ultrafast -filter:v fps=fps=30 -tune zerolatency \
    -f flv rtmp:localhost/live/bbb \
    -nostdin -nostats > /var/log/ffmpeg_output.log 2>&1 < /dev/null &");
}

void stopLivestream()
{
    streamRunning = 0;
    system("pidof ffmpeg | xargs kill -9");
}

int getStreamStatus()
{
    return streamRunning;
}