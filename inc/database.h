#ifndef DATABASE_H
#define DATABASE_H

#define ERROR 7

int send_temp_hum( float temp, float hum);

int get_swing_flag();

int get_live_flag();

int send_notification_flag( int notification_flag);

#endif