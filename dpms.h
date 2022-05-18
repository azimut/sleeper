#ifndef DPMS_H
#define DPMS_H

#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>

typedef struct {
  Display *display;
  CARD16 mode, prev_mode;
  BOOL client;
} DPMSState;

DPMSState dpms_new(void);
void dpms_update(DPMSState *s);
void dpms_check(DPMSState s, time_t now, time_t *last_sleep,
                time_t *last_wakeup);

#endif /* DPMS_H */
