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
 * @brief Start the server
 */
void initServer();

/**
 * @brief Close the server
 */
void endServer();

/**
 * @brief Start the livestream
 * 
 * @return int : 0 on success, 1 on error
 */
int startLivestream();

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