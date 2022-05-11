#ifndef FILE_H_
#define FILE_H_

#include <time.h>

time_t load(char *filename);
void save(char *filename, time_t offset);

#endif
