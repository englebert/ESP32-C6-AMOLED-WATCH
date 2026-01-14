#ifndef RTC_H
#define RTC_H

#include "main.h"

extern SensorPCF85063 rtc;

void init_rtc(void);
void syncTimeFromRTC(void);
void syncNTP(void);

#endif
