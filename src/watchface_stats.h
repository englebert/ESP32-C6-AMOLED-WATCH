#ifndef WATCHFACE_STATS_H
#define WATCHFACE_STATS_H

#include "main.h"

// Called once when we switch to this page to set up the styles and objects
void load_watchface_stats();

// Called every loop to update the time/battery
void update_watchface_stats();

#endif
