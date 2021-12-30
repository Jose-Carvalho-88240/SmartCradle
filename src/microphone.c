#include "../inc/microphone.h"

void startRecording()
{
    system("ffmpeg \
    -f alsa -ac 1 -ar 44100 -i plughw:1,0 \
    -c:a copy \
    -t 2 -y \
    audiorecord.wav");
}

static float calculateLoudness()
{
    return 1;
}

int processAudio()
{
    return 1;
}

