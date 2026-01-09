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
#include "SensorPCF85063.hpp"
#include "XPowersLib.h"
#include "HWCDC.h"

extern time_t bootuptime;
extern HWCDC USBSerial;
extern std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus;

#include "processes.h"
#include "touch.h"
#include "rtc.h"
#include "display.h"
#include "page.h"
#include "power.h"

#include "watchface00.h"
#include "watchface01.h"
#include "watchface02.h"

/**
void adcOn(void);
void adcOff(void);
 **/

#endif
