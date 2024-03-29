#include "./dpms.h"
#include "./config.h"
#include "./file.h"
#include "./sql.h"

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

DPMSState dpms_new(void) {
  DPMSState result = {XOpenDisplay(0), DPMSModeOn, DPMSModeOn, 0};
  if (!result.display)
    errx(EXIT_FAILURE, "cannot open display '%s'", XDisplayName(0));
  return result;
}

void dpms_update(DPMSState *s) {
  s->prev_mode = s->mode;
  DPMSInfo(s->display, &s->mode, &s->client);
}

static bool dpms_wake(CARD16 prev_pw, CARD16 pw) {
  return (pw == DPMSModeOn) && (prev_pw != pw);
}

static bool dpms_sleep(CARD16 prev_pw, CARD16 pw) {
  return (pw == DPMSModeOff) && (prev_pw != pw);
}

void dpms_check(DPMSState s, time_t now, time_t *last_sleep, time_t *last_wakeup) {
  double dt;
  if (dpms_wake(s.prev_mode, s.mode)) {
    *last_wakeup = now;
    dt = difftime(*last_wakeup, *last_sleep);
    printf("DPMS screen wake up after `%.2f` hours of sleep\n", dt);
    sql_insert_event("dpms", *last_sleep, *last_wakeup);
    if (dt > DT_DPMS) {
      save(SLEEP_FILE, *last_sleep);
      save(AWAKE_FILE, *last_wakeup);
    }
  }
  if (dpms_sleep(s.prev_mode, s.mode)) {
    *last_sleep = now;
    dt = difftime(*last_sleep, *last_wakeup);
    printf("DPMS screen sleeping after `%.2f` hours awake\n", dt);
  }
}

void dpms_free(DPMSState *s) { XCloseDisplay(s->display); }
