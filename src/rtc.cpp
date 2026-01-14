#include <Arduino.h>
#include "rtc.h"
#include <time.h>

// Use standard "C" struct tm for time comparisons
// Define Timezone: UTC+8 (Malaysia/China/Singapore)
#define GMT_OFFSET_SEC  (8 * 3600) 
#define DAYLIGHT_OFFSET 0

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

void syncNTP(void) {
    USBSerial.println("NTP: Attempting sync...");

    // 1. Initialize NTP (Standard Arduino/ESP32 Time Lib)
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET, "pool.ntp.org", "time.nist.gov");

    // 2. Wait for NTP Time (Timeout after 3 seconds)
    struct tm ntp_tm;
    if (!getLocalTime(&ntp_tm, 3000)) {
        USBSerial.println("NTP: Failed to fetch time (Timeout)");
        return;
    }

    USBSerial.println("NTP: Got time! Checking RTC...");

    // 3. Get Current RTC Time
    // (Assuming 'rtc' is the name of your PCF85063 object defined in rtc.cpp)
    // We need to convert RTC custom struct to standard time_t for easy comparison
    // SensorPCF85063::DateTime rtc_now = rtc.getDateTime();
    RTC_DateTime rtc_now = rtc.getDateTime();
    
    struct tm rtc_tm;
    rtc_tm.tm_year = rtc_now.getYear() - 1900;
    rtc_tm.tm_mon  = rtc_now.getMonth() - 1;
    rtc_tm.tm_mday = rtc_now.getDay();
    rtc_tm.tm_hour = rtc_now.getHour();
    rtc_tm.tm_min  = rtc_now.getMinute();
    rtc_tm.tm_sec  = rtc_now.getSecond();
    rtc_tm.tm_isdst = -1;

    time_t ntp_epoch = mktime(&ntp_tm);
    time_t rtc_epoch = mktime(&rtc_tm);

    // 4. Compare (Diff > 2 seconds)
    double diff = difftime(ntp_epoch, rtc_epoch);
    USBSerial.printf("NTP: %ld, RTC: %ld, Diff: %.0f sec\n", ntp_epoch, rtc_epoch, diff);

    if (abs(diff) > 2) {
        USBSerial.println("NTP: Updating RTC...");
        
        // Update RTC
        rtc.setDateTime(
            ntp_tm.tm_year + 1900,
            ntp_tm.tm_mon + 1,
            ntp_tm.tm_mday,
            ntp_tm.tm_hour,
            ntp_tm.tm_min,
            ntp_tm.tm_sec
        );

        // Also sync the internal ESP32 system time immediately
        syncTimeFromRTC(); 
        
        USBSerial.println("NTP: Sync Complete.");
    } else {
        USBSerial.println("NTP: RTC is already accurate.");
    }
}
