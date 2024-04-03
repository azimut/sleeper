#include "./utils.h"
#include <err.h>
#include <stdlib.h>

char *format_date(const time_t t) {
  struct tm *tmp = localtime(&t);
  if (!tmp)
    err(EXIT_FAILURE, "failed localtime()");

  char *buf = malloc(sizeof(char) * 100);
  if (!buf)
    err(EXIT_FAILURE, "failed malloc()");

  if (strftime(buf, 100, "%a %b %d %T %Y", tmp) == 0)
    err(EXIT_FAILURE, "failed strftime()");

  return buf;
}
