#include "./utils.h"
#include <err.h>
#include <stdlib.h>
#include <string.h>

char *format_date(const time_t t) {
  struct tm *tmp = localtime(&t);
  if (!tmp)
    err(EXIT_FAILURE, "failed localtime()");

  char *buf = malloc(sizeof(char) * 100);
  if (!buf)
    err(EXIT_FAILURE, "failed malloc()");

  if (strftime(buf, 100, "%F %T", tmp) == 0)
    err(EXIT_FAILURE, "failed strftime()");

  return buf;
}

char *from_home(const char *path) {
  char *fullpath;
  fullpath = malloc(strlen(getenv("HOME")) + strlen(path) + 1);
  strcpy(fullpath, getenv("HOME"));
  strcat(fullpath, path);
  return fullpath;
}
