#ifndef FILE_INC
#define FILE_INC

#include <time.h>

time_t load(char *filename);
void save(char *filename, time_t offset);

#endif
