#ifndef FILE_H_
#define FILE_H_

#include <time.h>

extern time_t load(const char *filename);
extern void save(const char *filename, time_t offset);

#endif
