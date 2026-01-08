#include <Arduino.h>
#include "processes.h"

uint32_t total_awake_time = 0;
uint32_t awake_time = 0;


void backgroundUpdate(void *pvParameters) {
    (void) pvParameters;

    uint32_t last_update = millis();
    lv_obj_t *label = lv_label_create(lv_scr_act());

    for(;;) {
        lv_task_handler();

        if(awake_time > 30) {
            modeSleep();
        }

        // Main watch face
        if(page == 0) {
            if((uint32_t)(millis() - last_update) > 500) {
                if(page_change) {
                    page_change = false;
                    gfx->flush();
                    gfx->fillScreen(BLACK);
                }

                show_watchface01(label);

                last_update = millis();
            }
        } else if(page == 1) {
            if((uint32_t)(millis() - last_update) > 500) {
                if(page_change) {
                    page_change = false;
                    gfx->flush();
                    gfx->fillScreen(BLACK);
                }

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
                
                last_update = millis();
            }
        } else if(page == 2) {
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

void backgroundUptime(void *pvParameters) {
    (void) pvParameters;

    uint32_t last_update = millis();

    for(;;) {
        if((uint32_t)(millis() - last_update) > 999) {
            if(display_status) 
                awake_time++;

            last_update = millis();
            total_awake_time++;
        }

        vTaskDelay(10);
    }
}
