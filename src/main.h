#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "pin_config.h"

#include <WiFi.h>

#include <time.h>
#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "lv_conf.h"
#include "SensorPCF85063.hpp"
#include "SensorQMI8658.hpp"
#include "XPowersLib.h"
#include "HWCDC.h"
#include <LittleFS.h>
#include <vector>

extern time_t bootuptime;
extern HWCDC USBSerial;
extern std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus;

#include "processes.h"
#include "touch.h"
#include "rtc.h"
#include "display.h"
#include "page.h"
#include "power.h"
#include "imu.h"
#include "wifi_manager.h"

#include "watchface_graphical.h"
#include "watchface_stats.h"
#include "watchface_analog.h"
#include "watchface_analog_simple.h"
#include "watchface_terminal.h"
#include "watchface_settings.h"
#include "watchface_wifi.h"


/**
void adcOn(void);
void adcOff(void);
 **/

#endif
