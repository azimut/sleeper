#include "./config.h"
#include "./dpms.h"
#include "./file.h"
#include "./sql.h"
#include "./suspend.h"
#include "./upower.h"
#include "./utils.h"

#include <err.h>
#include <paths.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>

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

  time_t last_reboot, last_shutdown;
  struct utmp data;
  FILE *fp = fopen(_PATH_WTMP, "r");
  memset(&data, 0, sizeof(struct utmp));
  while (fread(&data, sizeof(struct utmp), 1, fp) == 1) {
    if (strcmp("shutdown", data.ut_user) == 0)
      last_shutdown = data.ut_tv.tv_sec;
    if (strcmp("reboot", data.ut_user) == 0)
      last_reboot = data.ut_tv.tv_sec;
  }

  printf("System up since: %s\n", format_date(last_reboot));
  if ((time(NULL) - last_wakeup) / 60 / 60 > DT_SHUTDOWN) {
    last_wakeup = last_reboot;
    last_sleep = last_shutdown;
    save(SLEEP_FILE, last_sleep);
    save(AWAKE_FILE, last_wakeup);
    printf("Fast-forwarding wakeup/sleep time to match uptime.\n");
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
