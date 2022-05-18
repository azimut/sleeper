#include "./suspend.h"
#include "./config.h"
#include "./file.h"

#include <stdbool.h>
#include <stdio.h>

static float dt_hours(time_t a, time_t b) { return (a - b) / 60.0f / 60.0f; }

static bool suspension_wake(time_t now, time_t before) {
  return ((now - before) > (WHILE_SLEEP_TIME * 3)) && before != 0;
}

void suspension_check(time_t now, time_t before, time_t *last_sleep,
                      time_t *last_wakeup) {
  float dt;
  if (suspension_wake(now, before)) {
    *last_sleep = before;
    *last_wakeup = now;
    dt = dt_hours(*last_wakeup, *last_sleep);
    printf("Suspension sytem wake after `%.2f` hours\n", dt);
    if (dt > DT_SUSPENSION) {
      save(AWAKE_FILE, *last_wakeup);
      save(SLEEP_FILE, *last_sleep);
    }
  }
}
