#include "./file.h"

#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <upower.h>

#define SLEEP_FILE "sleeptime"
#define AWAKE_FILE "awaketime"

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

float dt_hours(time_t a, time_t b) { return (a - b) / 60.0f / 60.0f; }

bool dpms_wake(CARD16 prev_pw, CARD16 pw) {
  return (pw == DPMSModeOn) && (prev_pw != pw);
}

bool dpms_sleep(CARD16 prev_pw, CARD16 pw) {
  return (pw == DPMSModeOff) && (prev_pw != pw);
}

bool suspension_wake(time_t a, time_t b, long sleep_seconds) {
  return (a - b) > (sleep_seconds * 3);
}

int main(int argc, char *argv[]) {
  long sleep_seconds;
  float dt;
  time_t last_wakeup;
  time_t last_sleep;
  time_t before, now;
  // dpms
  Display *dpy;
  CARD16 power_level, prev_power_level;
  BOOL dpms_state;
  // upower
  UpClient *up_client;
  gboolean on_battery, prev_on_battery;

  if (argc != 2)
    errx(EXIT_FAILURE, "needs 1 argument");

  sleep_seconds = string_to_long(argv[1]);
  if (sleep_seconds == -1)
    errx(EXIT_FAILURE, "argv[1] invalid");

  last_wakeup = load(AWAKE_FILE);
  last_sleep = load(SLEEP_FILE);
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

  printf("Starting loop...\n");

  while (1) {
    sleep(sleep_seconds);

    now = time(NULL);

    on_battery = up_client_get_on_battery(up_client);
    DPMSInfo(dpy, &power_level, &dpms_state);

    if (prev_on_battery != on_battery)
      printf("Battery status changed to `%d`\n", on_battery);

    if (suspension_wake(now, before, sleep_seconds) && before != 0) {
      last_sleep = before;
      last_wakeup = now;
      dt = dt_hours(last_wakeup, last_sleep);
      printf("Suspension sytem wake after `%.2f` hours\n", dt);
      if (dt > 5.0f) {
        save(AWAKE_FILE, last_wakeup);
        save(SLEEP_FILE, last_sleep);
      }
    }

    if (dpms_wake(prev_power_level, power_level)) {
      last_wakeup = now;
      dt = dt_hours(now, last_sleep);
      printf("DPMS screen wake up after `%.2f` hours of sleep\n", dt);
      if (dt > 5.0f)
        save(AWAKE_FILE, last_wakeup);
    }

    if (dpms_sleep(prev_power_level, power_level)) {
      last_sleep = now;
      dt = dt_hours(now, last_wakeup);
      printf("DPMS screen sleeping after `%.2f` hours awake\n", dt);
      if (dt > 5.0f)
        save(SLEEP_FILE, last_sleep);
    }

    prev_on_battery = on_battery;
    prev_power_level = power_level;
    before = now;
  }
  return EXIT_SUCCESS;
}
