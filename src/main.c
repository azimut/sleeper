#include "./config.h"
#include "./dpms.h"
#include "./file.h"
#include "./sql.h"
#include "./suspend.h"
#include "./upower.h"

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

bool quit = false;

void stop(__attribute__((unused)) int sig) {
  printf("Stopping loop...\n");
  quit = true;
}

int main() {
  time_t last_wakeup = load(AWAKE_FILE);
  time_t last_sleep = load(SLEEP_FILE);
  time_t before = 0, now = time(NULL);

  DPMSState dpms_state = dpms_new();
  UpState upstate = upower_new();

  // Unbuffer stdout
  if (setvbuf(stdout, NULL, _IONBF, 0) != 0)
    err(EXIT_FAILURE, "setvbuf");

  umask(0);
  chdir("/");
  signal(SIGTERM, stop);
  sql_ping();
  sql_create_table();
  sql_insert_event("startup", now, now);
  printf("Starting loop...\n");

  while (!quit) {
    sleep(WHILE_SLEEP_TIME);

    upower_update(&upstate);
    dpms_update(&dpms_state);

    before = now;
    now = time(NULL);

    upower_check(upstate);
    dpms_check(dpms_state, now, &last_sleep, &last_wakeup);
    suspension_check(now, before, &last_sleep, &last_wakeup);
  }

  dpms_free(&dpms_state);
  return EXIT_SUCCESS;
}
