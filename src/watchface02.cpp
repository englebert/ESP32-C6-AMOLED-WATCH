#include "watchface02.h"

void show_watchface02(lv_obj_t *label) {
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    String info = "";

    uint8_t charge_status = power.getChargerStatus();
    info += "Temp: " + String(power.getTemperature()) + "*C\n";
    info += "Charging: " + String(power.isCharging() ? "YES" : "NO") + "\n";
    info += "VbusIn: " + String(power.isVbusIn() ? "YES" : "NO") + "\n";
    info += "VbusGood: " + String(power.isVbusGood() ? "YES" : "NO") + "\n";

    switch (charge_status) {
        case XPOWERS_AXP2101_CHG_TRI_STATE:
            info += "Charger Status: tri_charge\n";
            break;
        case XPOWERS_AXP2101_CHG_PRE_STATE:
            info += "Charger Status: pre_charge\n";
            break;
        case XPOWERS_AXP2101_CHG_CC_STATE:
            info += "Charger Status: constant charge\n";
            break;
        case XPOWERS_AXP2101_CHG_CV_STATE:
            info += "Charger Status: constant voltage\n";
            break;
        case XPOWERS_AXP2101_CHG_DONE_STATE:
            info += "Charger Status: charge done\n";
            break;
        case XPOWERS_AXP2101_CHG_STOP_STATE:
            info += "Charger Status: not charging\n";
            break;
    }

    info += "Battery Voltage: " + String(power.getBattVoltage()) + "mV\n";
    info += "Vbus Voltage: " + String(power.getVbusVoltage()) + "mV\n";
    info += "System Voltage: " + String(power.getSystemVoltage()) + "mV\n";

    if (power.isBatteryConnect()) {
        info += "Battery Percent: " + String(power.getBatteryPercent()) + "%\n";
    }

    // lv_label_set_text(info_label, info.c_str());
    // lv_obj_set_style_text_font(info_label, &lv_font_montserrat_20, LV_PART_MAIN);

    lv_label_set_text(label, info.c_str());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);
}
