#include "./upower.h"

#include <stdio.h>

UpState upower_new(void) {
  UpState upstate = (UpState){up_client_new(), 0, 0};
  upower_update(&upstate);
  printf("UPower: initial battery state is %d\n", upstate.battery);
  return upstate;
}

void upower_update(UpState *u) {
  u->prev_battery = u->battery;
  u->battery = up_client_get_on_battery(u->upclient);
}

void upower_check(UpState u) {
  if (u.battery != u.prev_battery)
    printf("UPower: Battery status changed to `%d`\n", u.battery);
}
