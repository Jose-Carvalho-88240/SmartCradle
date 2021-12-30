#include "../inc/microphone.h"
#include <sndfile.h>

#define THRESHOLD -15
#define LIMIT 3


void startRecording()
{
    system("ffmpeg \
    -f alsa -ac 1 -ar 44100 -i plughw:1,0 \
    -c:a copy \
    -t 2 -y \
    audiorecord.wav");
}

/**
 * @brief Calculates loudness of the .wav file
 * 
 * The value returned is expressed in the decibel 
 * (dB) scale being -1 the maximum value.
 * 
 * @return float (dB)
 */
static float calculateLoudness()
{
    char *inFileName;
    SNDFILE *inFile;
    SF_INFO inFileInfo;
    float sum=0;
    int i;

    inFileName = "audiorecord.wav";

    inFile = sf_open(inFileName, SFM_READ, &inFileInfo);

    int samples =  inFileInfo.frames*inFileInfo.channels;
    float buffer[samples];

    if(sf_read_float(inFile,buffer,samples) != samples)
    {
        fprintf(stderr, "Error when processing microphone samples.\n");
        return -1;
    }
    
    sf_close(inFile);

    for(i = 0; i<inFileInfo.frames*inFileInfo.channels; i++)
    {
        sum += (float)pow((double)buffer[i],2);
    }
    float ms = sqrt(sum)/(inFileInfo.frames*inFileInfo.channels);
    
    return 10*log10(ms);
}

_Bool processAudio()
{
    _Bool ret = 0;
    static u_int8_t crying_counter = 0;

    if(calculateLoudness() > THRESHOLD)
    {
        if(crying_counter++ >= LIMIT)
        {
            crying_counter = 0;
            ret = 1;
        }
            
        return ret;
    }

    crying_counter = 0;
    return ret;
}

