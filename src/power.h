#ifndef POWER_H
#define POWER_H

#include "main.h"

extern XPowersPMU power;
extern uint16_t sleep_count;

void init_power(void);
void adcOn(void);
void adcOff(void);
void modeSleep(void);

#endif
