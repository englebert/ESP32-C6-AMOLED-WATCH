#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "pin_config.h"

#include <WiFi.h>

#include "time.h"
#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "lv_conf.h"
#include "XPowersLib.h"
#include "HWCDC.h"

#include "processes.h"

void Arduino_IIC_Touch_Interrupt(void);
void adcOn(void);
void adcOff(void);


#endif
