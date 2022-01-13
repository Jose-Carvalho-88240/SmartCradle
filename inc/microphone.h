/**
 * @file microphone.h
 * @author José Carvalho, João Carneiro
 * @brief Microphone module header
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */


#ifndef MICROPHONE_H
#define MICROPHONE_H

#define ERR_OPEN 12
#define ERR_READ 15

/**
 * @brief Records a 2 second .wav audio from microphone
 * 
 * Recording is possible by using FFmpeg. The parameters used are:
 *      -f alsa : force audio input device to be of ALSA
 *      -ac 1  : set audio channel to 1
 *      -ar 44100 : set audio sampling rate to 44100 Hz
 *      -i plughw:0,0 : select the audio card referencing the microphone
 *      -c:a copy : set audio codec to copy (copies the frames directly
 *      instead of decoding/filtering/encoding)
 *      -t 2 : recording duration
 *      -y : overwrite output files
 *      audiorecord.wav output file
 * 
 * @return int 
 */
int startRecording();

/**
 * @brief Processes the audio to check whether the baby is crying
 * 
 * The loudness of the audio samples is calculated and, if the
 * predefined threshold is passed, a counter is
 * incremented. At 3 consecutive samples that have breached the 
 * threshold, it is considered that the baby is crying.
 * 
 * @return int
 */
int processAudio(float *f);

#endif