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

bool quit = false;

void stop(__attribute__((unused)) int sig) {
  printf("Stopping loop...\n");
  quit = true;
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
  time_t before = 0, now = time(NULL);

  Display *dpms_display;
  CARD16 dpms_mode = DPMSModeOn, dpms_prev_mode = DPMSModeOn;
  BOOL dpms_state = 0;

  UpClient *upower_client = up_client_new();
  gboolean upower_battery = 0, upower_prev_battery = 0;

  dpms_display = XOpenDisplay(0);
  if (!dpms_display)
    errx(EXIT_FAILURE, "cannot open display '%s'", XDisplayName(0));

  // Unbuffer stdout
  if (setvbuf(stdout, NULL, _IONBF, 0) != 0)
    err(EXIT_FAILURE, "setvbuf");

  umask(0);
  chdir("/");
  signal(SIGTERM, stop);
  printf("Starting loop...\n");

  while (!quit) {
    sleep(WHILE_SLEEP_TIME);

    upower_prev_battery = upower_battery;
    dpms_prev_mode = dpms_mode;
    before = now;
    now = time(NULL);
    upower_battery = up_client_get_on_battery(upower_client);
    DPMSInfo(dpms_display, &dpms_mode, &dpms_state);

    if (upower_prev_battery != upower_battery)
      printf("Battery status changed to `%d`\n", upower_battery);

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

    if (dpms_wake(dpms_prev_mode, dpms_mode)) {
      last_wakeup = now;
      dt = dt_hours(last_wakeup, last_sleep);
      printf("DPMS screen wake up after `%.2f` hours of sleep\n", dt);
      if (dt > DT_DPMS) {
        save(SLEEP_FILE, last_sleep);
        save(AWAKE_FILE, last_wakeup);
      }
    }

    if (dpms_sleep(dpms_prev_mode, dpms_mode)) {
      last_sleep = now;
      printf("DPMS screen sleeping after `%.2f` hours awake\n",
             dt_hours(last_sleep, last_wakeup));
    }
  }
  XCloseDisplay(dpms_display);
  return EXIT_SUCCESS;
}
