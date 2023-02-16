#ifndef SUSPEND_H
#define SUSPEND_H

#include <time.h>

void suspension_check(time_t now, time_t before, time_t *last_sleep, time_t *last_wakeup);

#endif /* SUSPEND_H */
