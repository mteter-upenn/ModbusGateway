#ifndef HANDLERTC_H
#define HANDLERTC_H

#include <Time.h>

time_t getNtpTime(void);
time_t getRtcTime(void);
void printTime(time_t t_time);

#endif // HANDLERTC_H
