/**
 * @file microphone.c
 * @author José Carvalho, João Carneiro
 * @brief Microphone module implementation
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#include <sndfile.h>
#include <sys/syslog.h>
#include <sys/wait.h>
#include "../inc/microphone.h"

#define THRESHOLD -35  // Audio threshold [dB]
#define LIMIT 3

int startRecording()
{
    int ret = 0;

    ret = system("ffmpeg \
    -f alsa -ac 1 -ar 44100 -i plughw:0,0 \
    -c:a copy \
    -t 2 -y \
    /etc/audiorecord.wav");

    //return value from the command on the upper 8-bits of the return value
    // 0 on sucess, 1 on failure
    return WEXITSTATUS(ret); 
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

    inFileName = "/etc/audiorecord.wav"; //Audio file name

    inFile = sf_open(inFileName, SFM_READ, &inFileInfo); //Open the .wav file with read permissions
    if(inFile == NULL)
        return ERR_OPEN;

    int samples =  inFileInfo.frames*inFileInfo.channels; //Get the size of the audio file in samples
    float buffer[samples];

    if(sf_read_float(inFile,buffer,samples) != samples) //Read the audio file values to the buffer
        return ERR_READ;
    
    sf_close(inFile);

    //Calculate the RMS value of the audio sample
    for(i = 0; i<inFileInfo.frames*inFileInfo.channels; i++)
    {
        sum += (float)pow((double)buffer[i],2);
    }
    float ms = sqrt(sum)/samples;
    
    return 10*log10(ms); //Returns in dB
}

int processAudio(float *loudness)
{
    int ret = 0;
    static u_int8_t crying_counter = 0;
    
    ret = calculateLoudness();

    if(ret == ERR_OPEN || ret == ERR_READ)
        return ret;

    *loudness = ret;
    
    if(ret >= THRESHOLD)
    {
        ret = 0;
        if(++crying_counter >= LIMIT)
        {
            crying_counter = 0;
            ret = 1;
        }
        return ret;
    }

    if(crying_counter > 0)
        crying_counter--;
    
    return 0;
}

