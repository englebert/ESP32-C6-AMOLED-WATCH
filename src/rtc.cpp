#include <Arduino.h>
#include "rtc.h"

SensorPCF85063 rtc;

void init_rtc(void) {
    // RTC Initialization
    if(!rtc.begin(Wire, IIC_SDA, IIC_SCL)) {
        USBSerial.println("Failed to find PCF8563 - check your wiring!");

        // RTC Error. Hang here.
        while(true) {
            delay(10);
        }
    }
    syncTimeFromRTC();
    bootuptime = rtc.hwClockRead();
}

void syncTimeFromRTC(void) {
    RTC_DateTime dt = rtc.getDateTime();

    // Convert to standard C 'struct tm'
    struct tm t;
    t.tm_year = dt.getYear() - 1900;  // Year since 1900
    t.tm_mon  = dt.getMonth() - 1;    // Month 0-11
    t.tm_mday = dt.getDay();
    t.tm_hour = dt.getHour();
    t.tm_min  = dt.getMinute();
    t.tm_sec  = dt.getSecond();
    t.tm_isdst = -1;             // Let system determine DST (or set 0)

    // Convert to time_t (seconds since Epoch)
    time_t timeNow = mktime(&t);

    // Set the ESP32 internal system time
    struct timeval tv = { .tv_sec = timeNow, .tv_usec = 0 };
    settimeofday(&tv, NULL);

    USBSerial.printf("RTC Sync: %02d:%02d:%02d\n", t.tm_hour, t.tm_min, t.tm_sec);
}
