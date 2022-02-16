#include "file.h"
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

time_t load(char *filename) {
  struct stat s;
  time_t ret = time(NULL);
  char *path;
  path = malloc(strlen(getenv("HOME")) + strlen("/.cache/") + strlen(filename) +
                1);
  strcpy(path, getenv("HOME"));
  strcat(path, "/.cache/");
  strcat(path, filename);
  if (stat(path, &s) == 0) {
    ret = s.st_mtim.tv_sec;
  }
  free(path);
  return ret;
}

void save(char *filename) {
  FILE *fd;
  char *file;
  time_t offset;
  offset = time(NULL);
  file = malloc(strlen(getenv("HOME")) + strlen("/.cache/") + strlen(filename) +
                1);
  strcpy(file, getenv("HOME"));
  strcat(file, "/.cache/");
  strcat(file, filename);
  fd = fopen(file, "a");
  if (fd == NULL)
    errx(EXIT_FAILURE, "opening file: %d", errno);
  putw(offset, fd);
  fclose(fd);
  free(file);
  return;
}
