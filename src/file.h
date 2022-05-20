#ifndef FILE_H_
#define FILE_H_

#include <time.h>

time_t load(const char *filename);
void save(const char *filename, time_t offset);

#endif /* FILE_H_ */
