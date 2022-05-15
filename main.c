#include "./config.h"
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

bool looping = true;

void stop(__attribute__((unused)) int sig) {
  printf("Stopping loop...\n");
  looping = false;
}

float dt_hours(time_t a, time_t b) { return (a - b) / 60.0f / 60.0f; }

bool dpms_wake(CARD16 prev_pw, CARD16 pw) {
  return (pw == DPMSModeOn) && (prev_pw != pw);
}

bool dpms_sleep(CARD16 prev_pw, CARD16 pw) {
  return (pw == DPMSModeOff) && (prev_pw != pw);
}

bool suspension_wake(time_t now, time_t before) {
  return ((now - before) > (WHILE_SLEEP_TIME * 3)) && before != 0;
}

int main() {
  float dt;
  time_t last_wakeup = load(AWAKE_FILE);
  time_t last_sleep = load(SLEEP_FILE);
  time_t before = 0, now;
  // dpms
  Display *display;
  CARD16 power_level = DPMSModeOn, prev_power_level = DPMSModeOn;
  BOOL dpms_state;
  // upower
  UpClient *up_client = up_client_new();
  gboolean on_battery = 0, prev_on_battery = 0;

  display = XOpenDisplay(0);
  if (!display)
    errx(EXIT_FAILURE, "cannot open display '%s'", XDisplayName(0));

  // Unbuffer stdout
  if (setvbuf(stdout, NULL, _IONBF, 0) != 0)
    errx(EXIT_FAILURE, "setvbuf");

  umask(0);
  chdir("/");
  signal(SIGTERM, stop);
  printf("Starting loop...\n");

  while (looping) {
    sleep(WHILE_SLEEP_TIME);

    now = time(NULL);
    on_battery = up_client_get_on_battery(up_client);
    DPMSInfo(display, &power_level, &dpms_state);

    if (prev_on_battery != on_battery)
      printf("Battery status changed to `%d`\n", on_battery);

    if (suspension_wake(now, before)) {
      last_sleep = before;
      last_wakeup = now;
      dt = dt_hours(last_wakeup, last_sleep);
      printf("Suspension sytem wake after `%.2f` hours\n", dt);
      if (dt > DT_SUSPENSION) {
        save(AWAKE_FILE, last_wakeup);
        save(SLEEP_FILE, last_sleep);
      }
    }

    if (dpms_wake(prev_power_level, power_level)) {
      last_wakeup = now;
      dt = dt_hours(last_wakeup, last_sleep);
      printf("DPMS screen wake up after `%.2f` hours of sleep\n", dt);
      if (dt > DT_DPMS) {
        save(SLEEP_FILE, last_sleep);
        save(AWAKE_FILE, last_wakeup);
      }
    }

    if (dpms_sleep(prev_power_level, power_level)) {
      last_sleep = now;
      printf("DPMS screen sleeping after `%.2f` hours awake\n",
             dt_hours(last_sleep, last_wakeup));
    }

    prev_on_battery = on_battery;
    prev_power_level = power_level;
    before = now;
  }
  XCloseDisplay(display);
  return EXIT_SUCCESS;
}
