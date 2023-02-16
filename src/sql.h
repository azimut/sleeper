#ifndef SQL_H
#define SQL_H

#include <time.h>

void sql_insert_event(const time_t prev, const time_t curr);
void sql_ping(void);

#endif /* SQL_H */
