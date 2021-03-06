#include "./upower.h"

#include <stdio.h>

UpState upower_new(void) { return (UpState){up_client_new(), 0, 0}; }

void upower_update(UpState *u) {
  u->prev_battery = u->battery;
  u->battery = up_client_get_on_battery(u->upclient);
}

void upower_check(UpState u) {
  if (u.battery != u.prev_battery)
    printf("Battery status changed to `%d`\n", u.battery);
}
