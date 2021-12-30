#ifndef LIVESTREAM_H
#define LIVESTREAM_H

/**
 * @brief Start the server
 */
void initStream();

/**
 * @brief Start the livestream
 */
void startLivestream();

/**
 * @brief Stop the server
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