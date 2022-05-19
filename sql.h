#ifndef SQL_H
#define SQL_H

#include <time.h>

void sql_insert_sleep(time_t time);
void sql_insert_awake(time_t time);
void sql_ping(void);

#endif /* SQL_H */
