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

    bootuptime = rtc.hwClockRead();
}
