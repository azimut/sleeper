#ifndef SQL_H
#define SQL_H

#include <time.h>

int sql_initdb(void);
void sql_insert_event(const char *etype, const time_t prev, const time_t curr);

#endif /* SQL_H */
