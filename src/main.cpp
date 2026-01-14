#include <Arduino.h>
#include "main.h"

HWCDC USBSerial;

time_t bootuptime;

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(
        IIC_SDA,
        IIC_SCL,
        &Wire
);

/**
void adcOn(void) {
    power.enableTemperatureMeasure();
    // Enable internal ADC detection
    power.enableBattDetection();
    power.enableVbusVoltageMeasure();
    power.enableBattVoltageMeasure();
    power.enableSystemVoltageMeasure();
}

void adcOff(void) {
    power.disableTemperatureMeasure();
    // Enable internal ADC detection
    power.disableBattDetection();
    power.disableVbusVoltageMeasure();
    power.disableBattVoltageMeasure();
    power.disableSystemVoltageMeasure();
}


 **/

void setup(void) {
    USBSerial.begin(115200);
    delay(50);

    // USBSerial.println("[My Watch 1.0]");
    String LVGL_Arduino =
        String('V') + 
        lv_version_major() + "." + 
        lv_version_minor() + "." + 
        lv_version_patch();
    USBSerial.println(LVGL_Arduino);

    Wire.begin(IIC_SDA, IIC_SCL);

    init_rtc();
    init_display();
    init_power();

    // Checking for wake up reason
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    if(wakeup_reason == ESP_SLEEP_WAKEUP_GPIO) {
        USBSerial.println("Woke up by Touch!");
    } else {
        USBSerial.println("Normal Power On");
    }

    init_touch();
    init_imu();
   
    // BLACK screen
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(BLACK), LV_PART_MAIN);

    // Enable gestures
    lv_obj_add_event_cb(lv_scr_act(), gesture_event_cb, LV_EVENT_GESTURE, NULL);

    // Creating tasks
    xTaskCreate(
        backgroundUpdate,
        "Background update",
        16384,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        backgroundUptime,
        "Uptime updater",
        1024,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        backgroundSyncTime,
        "Sync Time",
        8192,
        NULL,
        1,
        NULL
    );

    // USBSerial.println("Setup - OK");
}

void loop() {
}
