#ifndef PROCESSES_H
#define PROCESSES_H

#include "main.h"

void backgroundUpdate(void *pvParameters);
void backgroundUptime(void *pvParameters);
void backgroundSyncTime(void *pvParameters);
void backgroundMonitorWiFi(void *pvParameters);

extern uint32_t total_awake_time;
extern uint32_t awake_time;
extern float delta;

#endif
