#ifndef UPOWER_H
#define UPOWER_H

#include <upower.h>

typedef struct {
  UpClient *upclient;
  gboolean battery, prev_battery;
} UpState;

UpState upower_new(void);
void upower_update(UpState *u);
void upower_check(UpState u);

#endif /* UPOWER_H */
