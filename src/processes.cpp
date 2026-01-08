#include <Arduino.h>
#include "processes.h"

void backgroundUpdate(void *pvParameters) {
    (void) pvParameters;

    uint32_t last_update = millis();
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    for(;;) {
        lv_task_handler();

        // Main watch face
        if(page == 0) {
            if((uint32_t)(millis() - last_update) > 500) {
                if(page_change) {
                    page_change = false;
                    gfx->flush();
                    gfx->fillScreen(BLACK);
                }

                String info = "";


                char buf[32];
                RTC_DateTime datetime = rtc.getDateTime();
                snprintf(buf, sizeof(buf), "%02d:%02d:%02d\n%02d-%02d-%04d",
                    datetime.getHour(), datetime.getMinute(), datetime.getSecond(),
                    datetime.getDay(), datetime.getMonth(), datetime.getYear());

                info += buf;

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
                lv_obj_set_style_text_font(label, &lv_font_montserrat_40, LV_PART_MAIN);

                /**
                gfx->fillScreen(BLACK);
                RTC_DateTime datetime = rtc.getDateTime();
                gfx->setCursor(0, 0);
                gfx->setTextColor(WHITE);
                gfx->setTextSize(6);
                gfx->print("Watch");
                gfx->setTextColor(BLUE);
                gfx->setCursor(0, 80);
                gfx->setTextSize(4);
                char buf[32];
                snprintf(buf, sizeof(buf), "%02d:%02d:%02d\n%02d-%02d-%04d",
                    datetime.getHour(), datetime.getMinute(), datetime.getSecond(),
                    datetime.getDay(), datetime.getMonth(), datetime.getYear());
                gfx->print(buf);
                 **/
                last_update = millis();
            }
        } else if(page == 1) {
            gfx->setCursor(random(gfx->width()), random(gfx->height()));
            gfx->setTextColor(random(0xffff), random(0xffff));
            gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
            gfx->println("Hello World!");
            
            gfx->flush();
        } else {
            gfx->flush();
            gfx->fillScreen(BLACK);
        }

        vTaskDelay(5);
    }
}
