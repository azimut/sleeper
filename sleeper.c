#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

void checkpoint(char *filename, int offset) {
  FILE *fd;
  char *file;
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

long string_to_long(char *arg) {
  long ret;
  char *p;
  if (strlen(arg) == 0)
    return -1;
  ret = strtol(arg, &p, 10);
  if (*p != '\0' || errno != 0)
    return -1;
  if (ret <= 0)
    return -1;
  return ret;
}

int main(int argc, char *argv[]) {
  long sleep_time;
  float diff;
  time_t last_awake, before, now;
  Display *dpy;
  CARD16 power_level, prev_power_level;
  BOOL state;

  if (argc != 2)
    errx(EXIT_FAILURE, "needs 1 argument");

  sleep_time = string_to_long(argv[1]);
  if (sleep_time == -1)
    errx(EXIT_FAILURE, "argv[1] invalid");

  last_awake = time(NULL);
  before = 0;

  power_level = prev_power_level = DPMSModeOn;
  if (!(dpy = XOpenDisplay(0)))
    errx(EXIT_FAILURE, "cannot open display '%s'", XDisplayName(0));

  // Unbuffer stdout
  if (setvbuf(stdout, NULL, _IONBF, 0) != 0)
    errx(EXIT_FAILURE, "setvbuf");

  umask(0);
  chdir("/");

  while (1) {
    sleep(sleep_time);

    now = time(NULL);
    diff = (now - last_awake) / 60.0f / 60.0f;

    DPMSInfo(dpy, &power_level, &state);

    if ((now - before) > sleep_time * 2) {
      printf("Suspension sytem wake after `%.2f` hours\n", diff);
      checkpoint("awaketime", diff);
      last_awake = now;
    }

    if ((power_level == DPMSModeOn) && (prev_power_level != power_level)) {
      printf("DPMS screen wake up after `%.2f` hours of sleep\n", diff);
      checkpoint("awaketime", diff);
      last_awake = now;
    }

    if ((power_level == DPMSModeOff) && (prev_power_level != power_level)) {
      printf("DPMS screen sleeping after `%.2f` hours awake\n", diff);
      checkpoint("sleeptime", diff);
      last_awake = now;
    }

    prev_power_level = power_level;
    before = now;
  }

  return EXIT_SUCCESS;
}
