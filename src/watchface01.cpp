#include "watchface01.h"

void show_watchface01(lv_obj_t *label) {
    lv_obj_set_style_text_color(label, lv_color_hex(0x404040), LV_PART_MAIN);
    String info = "";

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    char buf[32];
    RTC_DateTime datetime = rtc.getDateTime();
    time_t current_hwtimestamp = rtc.hwClockRead();
    uint32_t seconds_uptime_since_boot = (uint32_t) difftime(current_hwtimestamp, bootuptime);

    snprintf(buf, sizeof(buf), "%02d:%02d:%02d\n%02d-%02d-%04d\nUp: %d\n\n",
        datetime.getHour(), datetime.getMinute(), datetime.getSecond(),
        datetime.getDay(), datetime.getMonth(), datetime.getYear(), seconds_uptime_since_boot);

    info += buf;

    uint8_t charge_status = power.getChargerStatus();
    // info += "Temp: " + String(power.getTemperature()) + "*C\n";

    if (power.isBatteryConnect()) {
        info += "Batt(%): " + String(power.getBatteryPercent()) + "%\n";
    }

    // lv_label_set_text(info_label, info.c_str());
    // lv_obj_set_style_text_font(info_label, &lv_font_montserrat_20, LV_PART_MAIN);

    lv_label_set_text(label, info.c_str());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_40, LV_PART_MAIN);
}
