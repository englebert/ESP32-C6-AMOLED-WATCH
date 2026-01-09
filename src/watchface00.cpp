#include <Arduino.h>
#include "watchface00.h"

#define ONE_MINUTE_MS (60 * 1000)
#define ONE_HOUR_MS (60 * 60 * 1000)
#define TWELVE_HOUR_MS (12 * 60 * 60 * 1000)

static uint8_t conv2d(const char *p) {
    uint8_t v = 0;
    return (10 * (*p - '0')) + (*++p - '0');
}

static unsigned long ms_offset;

void show_watchface00(void) {
    unsigned long ms = millis();

    // set watch arms' angle
    unsigned long clock_ms = (ms_offset + ms) % TWELVE_HOUR_MS;
    uint8_t hour = clock_ms / ONE_HOUR_MS;
    uint8_t minute = (clock_ms % ONE_HOUR_MS) / ONE_MINUTE_MS;
    int16_t angle = (clock_ms % ONE_MINUTE_MS) * 3600 / ONE_MINUTE_MS;

    /**
    lv_img_set_angle(ui_ImageArmSecond, angle);
    angle = (angle + (minute * 3600)) / 60;
    lv_img_set_angle(ui_ImageArmMinute, angle);
    angle = (angle + (hour * 3600)) / 12;
    lv_img_set_angle(ui_ImageArmHour, angle); 
     **/
}
