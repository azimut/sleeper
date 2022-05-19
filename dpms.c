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

static float dt_hours(time_t a, time_t b) { return (a - b) / 60.0f / 60.0f; }

static bool dpms_wake(CARD16 prev_pw, CARD16 pw) {
  return (pw == DPMSModeOn) && (prev_pw != pw);
}

static bool dpms_sleep(CARD16 prev_pw, CARD16 pw) {
  return (pw == DPMSModeOff) && (prev_pw != pw);
}

void dpms_check(DPMSState s, time_t now, time_t *last_sleep,
                time_t *last_wakeup) {
  float dt;
  if (dpms_wake(s.prev_mode, s.mode)) {
    *last_wakeup = now;
    dt = dt_hours(*last_wakeup, *last_sleep);
    printf("DPMS screen wake up after `%.2f` hours of sleep\n", dt);
    if (dt > DT_DPMS) {
      save(SLEEP_FILE, *last_sleep);
      save(AWAKE_FILE, *last_wakeup);
      sql_insert_sleep(*last_sleep);
      sql_insert_awake(*last_wakeup);
    }
  }
  if (dpms_sleep(s.prev_mode, s.mode)) {
    *last_sleep = now;
    dt = dt_hours(*last_sleep, *last_wakeup);
    printf("DPMS screen sleeping after `%.2f` hours awake\n", dt);
  }
}

void dpms_free(DPMSState *s) { XCloseDisplay(s->display); }
