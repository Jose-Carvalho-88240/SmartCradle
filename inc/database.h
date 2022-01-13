#ifndef DATABASE_H
#define DATABASE_H

#include </home/carvalho/Downloads/buildroot/buildroot-2021.02.6/output/host/include/python3.9/Python.h>

#define ERROR 7

/**
 * @brief Initializes all the functions
 * 
 * @return int : 1 on success, -ERROR or 0 on error
 */
int initDatabase();

/**
 * @brief Update temperature and humidity in database
 * 
 * @param temp Temperature
 * @param hum Humidity
 * @return int : 1 on success, 0 on error
 */
int send_temp_hum( float temp, float hum);

/**
 * @brief Get the swing flag from database
 * 
 * @return int : 1 on success, -ERROR or 0 on error
 */
int get_swing_flag();

/**
 * @brief Get the live flag from database
 * 
 * @return int : 1 on success, -ERROR or 0 on error
 */
int get_live_flag();

/**
 * @brief Update notification flag in database
 * 
 * @param notification_flag : 1 (set) or 0 (reset)
 * @return int : 1 on success, 0 on error
 */
int send_notification_flag(int notification_flag);

/**
 * @brief Update swing flag in database
 * 
 * @param swing_flag : 1 (set) or 0 (reset)
 * @return int : 1 on success, 0 on error
 */
int send_swing_flag(int swing_flag);

#endif