/**
 * @file livestream.h
 * @author José Carvalho, João Carneiro
 * @brief Livestream module header
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef LIVESTREAM_H
#define LIVESTREAM_H

/**
 * @brief Start the livestream
 */
void startLivestream();

/**
 * @brief Stop the livestream
 */
void stopLivestream();

/**
 * @brief Get the Streaming Status object
 * 
 * Returns the status of the stream
 * 
 * @return bool 
 */
_Bool getStreamStatus();

#endif