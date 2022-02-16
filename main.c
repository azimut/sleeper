#include "file.h"
#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <upower.h>

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
  float diff_hours;
  time_t last_awake, before, now;
  // dpms
  Display *dpy;
  CARD16 power_level, prev_power_level;
  BOOL state;
  // upower
  UpClient *up_client;
  gboolean on_battery, prev_on_battery;

  if (argc != 2)
    errx(EXIT_FAILURE, "needs 1 argument");

  sleep_time = string_to_long(argv[1]);
  if (sleep_time == -1)
    errx(EXIT_FAILURE, "argv[1] invalid");

  last_awake = load("awaketime");
  before = 0;

  power_level = prev_power_level = DPMSModeOn;
  if (!(dpy = XOpenDisplay(0)))
    errx(EXIT_FAILURE, "cannot open display '%s'", XDisplayName(0));

  up_client = up_client_new();
  on_battery = prev_on_battery = 0;

  // Unbuffer stdout
  if (setvbuf(stdout, NULL, _IONBF, 0) != 0)
    errx(EXIT_FAILURE, "setvbuf");

  umask(0);
  chdir("/");

  while (1) {
    sleep(sleep_time);

    now = time(NULL);
    diff_hours = (now - last_awake) / 60.0f / 60.0f;
    on_battery = up_client_get_on_battery(up_client);
    DPMSInfo(dpy, &power_level, &state);

    if (prev_on_battery != on_battery) {
      printf("Battery status changed to `%d`\n", on_battery);
    }

    if ((now - before) > sleep_time * 2) {
      diff_hours = (now - last_awake) / 60.0f / 60.0f;
      printf("Suspension sytem wake after `%.2f` hours\n", diff_hours);
      save("awaketime");
      last_awake = now;
    }

    if ((power_level == DPMSModeOn) && (prev_power_level != power_level)) {
      printf("DPMS screen wake up after `%.2f` hours of sleep\n", diff_hours);
      if (diff_hours > 5.0f)
        save("awaketime");
      last_awake = now;
    }

    if ((power_level == DPMSModeOff) && (prev_power_level != power_level)) {
      printf("DPMS screen sleeping after `%.2f` hours awake\n", diff_hours);
      if (diff_hours > 1.0f)
        save("sleeptime");
      last_awake = now;
    }

    prev_on_battery = on_battery;
    prev_power_level = power_level;
    before = now;
  }

  return EXIT_SUCCESS;
}