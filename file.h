#ifndef FILE_H_
#define FILE_H_

#include <time.h>

extern time_t load(char *filename);
extern void save(char *filename, time_t offset);

#endif
