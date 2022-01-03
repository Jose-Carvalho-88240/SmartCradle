#ifndef DATABASE_H
#define DATABASE_H

#include </home/carvalho/Downloads/buildroot/buildroot-2021.02.6/output/host/include/python3.9/Python.h>

#define ERROR 7

void initDatabase();

int send_temp_hum( float temp, float hum);

int get_swing_flag();

int get_live_flag();

int send_notification_flag( int notification_flag);

#endif