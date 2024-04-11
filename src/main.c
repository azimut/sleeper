#include "./config.h"
#include "./dpms.h"
#include "./file.h"
#include "./sql.h"
#include "./suspend.h"
#include "./upower.h"
#include "./utils.h"

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <unistd.h>

bool quit = false;

void stop(__attribute__((unused)) int sig) {
  printf("Stopping loop...\n");
  quit = true;
}

int main(void) {

  time_t last_sleep = load(SLEEP_FILE);
  printf("Last sleep: %s\n", format_date(last_sleep));

  time_t last_wakeup = load(AWAKE_FILE);
  printf("Last wakeup: %s\n", format_date(last_wakeup));

  struct sysinfo s_info;
  if (sysinfo(&s_info) != 0)
    return EXIT_FAILURE;

  time_t last_poweron = time(NULL) - s_info.uptime;
  printf("System uptime: %d\n", (int)s_info.uptime);
  printf("System up since: %s\n", format_date(last_poweron));
  if ((time(NULL) - last_wakeup) / 60 / 60 > DT_SHUTDOWN) {
    last_wakeup = last_poweron;
    save(AWAKE_FILE, last_poweron);
    printf("Fast-forwarding wakeup time to match uptime.\n");
  }

  time_t before = 0;
  time_t now = time(NULL);

  UpState upstate = upower_new();
  DPMSState dpms_state = dpms_new();

  // Unbuffer stdout
  if (setvbuf(stdout, NULL, _IONBF, 0) != 0)
    err(EXIT_FAILURE, "setvbuf");

  umask(0);
  chdir("/");
  signal(SIGTERM, stop);

  if (sql_initdb() != 0)
    return EXIT_FAILURE;

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
