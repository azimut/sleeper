#include "./file.h"

#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>

static char *fullpath(const char *filename) {
  char *path;
  path = malloc(strlen(getenv("HOME")) + strlen("/.cache/") + strlen(filename) + 1);
  strcpy(path, getenv("HOME"));
  strcat(path, "/.cache/");
  strcat(path, filename);
  return path;
}

static void create_if_missing(const char *path) {
  int fd;
  if (access(path, F_OK) == 0)
    return;
  if ((fd = creat(path, 0666)) == 0)
    close(fd);
  else
    err(EXIT_FAILURE, "failed to creat()");
}

time_t load(const char *filename) {
  struct stat s;
  time_t ret = time(NULL);
  char *path = fullpath(filename);
  create_if_missing(path);
  if (stat(path, &s) == 0)
    ret = s.st_mtim.tv_sec;
  else
    err(EXIT_FAILURE, "failed to stat()");
  free(path);
  return ret;
}

void save(const char *filename, time_t offset) {
  struct utimbuf t = {offset, offset};
  char *path = fullpath(filename);
  if (utime(path, &t) < 0)
    err(EXIT_FAILURE, "failed to utime()");
  free(path);
}
