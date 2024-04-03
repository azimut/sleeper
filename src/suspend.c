#include "./suspend.h"
#include "./config.h"
#include "./file.h"
#include "./sql.h"

#include <stdbool.h>
#include <stdio.h>

static bool suspension_wake(time_t now, time_t before) {
  return ((now - before) > (WHILE_SLEEP_TIME * 3)) && before != 0;
}

void suspension_check(time_t now, time_t before, time_t *last_sleep,
                      time_t *last_wakeup) {
  double dt;
  if (suspension_wake(now, before)) {
    *last_sleep = before;
    *last_wakeup = now;
    dt = difftime(*last_wakeup, *last_sleep) / 60 / 60;
    printf("Suspension sytem wake after `%.2f` hours\n", dt);
    sql_insert_event("suspension wakeup", *last_sleep, *last_wakeup);
    if (dt > DT_SUSPENSION) {
      save(AWAKE_FILE, *last_wakeup);
      save(SLEEP_FILE, *last_sleep);
    }
  }
}
