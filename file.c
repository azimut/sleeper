#include "./file.h"
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>

static char *fullpath(char *filename) {
  char *path;
  path = malloc(strlen(getenv("HOME")) + strlen("/.cache/") + strlen(filename) +
                1);
  strcpy(path, getenv("HOME"));
  strcat(path, "/.cache/");
  strcat(path, filename);
  return path;
}

time_t load(char *filename) {
  struct stat s;
  time_t ret = time(NULL);
  char *path = fullpath(filename);
  if (stat(path, &s) == 0)
    ret = s.st_mtim.tv_sec;
  free(path);
  return ret;
}

void save(char *filename, time_t offset) {
  struct utimbuf t = {offset, offset};
  char *path = fullpath(filename);
  utime(path, &t);
  free(path);
}
