#ifndef MICROPHONE_H
#define MICROPHONE_H

/**
 * @brief Recording of an audio sample
 * 
 * A 2 second .wav audio sample from the
 * microphone is captured and stored
 */
void startRecording();

/**
 * @brief Processes the audio to check whether the baby is crying
 * 
 * The loudness of the audio samples is calculated and, if the
 * predefined threshold is passed, a counter is
 * incremented. At 3 consecutive samples that have breached the 
 * threshold, it is considered that the baby is crying.
 * 
 * @return bool  
 */
_Bool processAudio();

#endif