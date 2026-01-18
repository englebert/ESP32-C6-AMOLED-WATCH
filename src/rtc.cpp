#include <Arduino.h>
#include "rtc.h"
#include "esp_sntp.h"

// Use standard "C" struct tm for time comparisons
// Define Timezone: UTC+8 (Malaysia/China/Singapore)
// #define GMT_OFFSET_SEC  (8 * 3600) 
// #define GMT_OFFSET_SEC  0
#define DAYLIGHT_OFFSET 0
#define TIMEZONE_STR    "MYT-8"
#define NTP_SERVER1     "time.google.com"
#define NTP_SERVER2     "pool.ntp.org"
#define NTP_SERVER3     "time.nist.gov"

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

void init_ntp(void) {
    configTime(0, 0, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);    // 0, 0 because we will use TZ in the next line
    setenv("TZ", TIMEZONE_STR, 1);                              // Set environment variable with your time zone
    tzset();
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
    USBSerial.println("NTP: Starting Sync Process...");

    // 1. Check for WiFi connection first
    if (WiFi.status() != WL_CONNECTED) {
        USBSerial.println("NTP: No WiFi connection. Aborting.");
        return;
    }

    // 2. Force SNTP to reset and start a fresh request
    esp_sntp_stop();
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_setservername(1, "time.google.com");
    esp_sntp_setservername(2, "time.nist.gov");
    esp_sntp_init();

    // 3. WAIT for the sync to complete (Critical Step!)
    // If we don't wait, getLocalTime just returns the old RTC time immediately.
    int retry = 0;
    while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED && retry < 20) {
        USBSerial.print(".");
        delay(500); // Wait 500ms
        retry++;
    }
    USBSerial.println();

    // 4. Check if we actually got a new time
    if (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
        USBSerial.println("NTP: Timeout! Failed to reach server.");
        return;
    }

    // 5. NOW fetch the *fresh* time from the system
    struct tm ntp_tm;
    getLocalTime(&ntp_tm); 

    // --- (Rest of your comparison logic works fine now) ---
    
    // Get RTC time
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
    
    long diff = ntp_epoch - rtc_epoch;

    USBSerial.printf("NTP Epoch: %ld, RTC Epoch: %ld, Diff: %ld\n", ntp_epoch, rtc_epoch, diff);

    if (abs(diff) > 2) {
        USBSerial.println("NTP: Updating RTC...");
        rtc.setDateTime(
            ntp_tm.tm_year + 1900,
            ntp_tm.tm_mon + 1,
            ntp_tm.tm_mday,
            ntp_tm.tm_hour,
            ntp_tm.tm_min,
            ntp_tm.tm_sec
        );
        // Important: Re-sync system time to ensure milliseconds are aligned
        syncTimeFromRTC(); 
        USBSerial.println("NTP: Sync Complete.");
    } else {
        USBSerial.println("NTP: RTC is already accurate.");
    }
}
