#include <Arduino.h>
#include "processes.h"

uint32_t total_awake_time = 0;
uint32_t awake_time = 0;


void backgroundUpdate(void *pvParameters) {
    (void) pvParameters;

    uint32_t last_update = millis();
    lv_obj_t *label = lv_label_create(lv_scr_act());

    // Initially force it update
    page_change = true;

    for(;;) {
        lv_task_handler();

        if(awake_time > 30) {
            modeSleep();
        }

        // Main watch face
        if(page == 2) {
            gfx->setCursor(random(gfx->width()), random(gfx->height()));
            gfx->setTextColor(random(0xffff), random(0xffff));
            gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
            gfx->println("Hello World!");
            
            gfx->flush();
        } else if(page == 0) {
            if(page_change || watchface_change) {
                page_change = false;
                watchface_change = false;
                gfx->fillScreen(BLACK);

                // Build the LVGL UI
                if(watchface == 0) {
                    load_watchface_graphical();
                } else if(watchface == 1) {
                    load_watchface_analog();
                } else if(watchface == 2) {
                    load_watchface_analog_simple();
                }
            }

            // HANDLE UPDATES (Run every 500ms)
            if(watchface == 0) {
                if((uint32_t)(millis() - last_update) > 500) {
                    update_watchface_graphical();
                    last_update = millis();
                }
            } else if(watchface == 1) {
                // HANDLE UPDATES (Run every 100ms)
                if((uint32_t)(millis() - last_update) > 100) {
                    update_watchface_analog();
                    last_update = millis();
                }
            } else if(watchface == 2) {
                // HANDLE UPDATES (Run every 100ms)
                if((uint32_t)(millis() - last_update) > 100) {
                    update_watchface_analog_simple();
                    last_update = millis();
                }
            }
        } else if(page == 1) {
            if(page_change) {
                page_change = false;
                gfx->fillScreen(BLACK);

                // Build the LVGL UI
                load_watchface_stats();
            }

            // HANDLE UPDATES (Run every 500ms)
            if((uint32_t)(millis() - last_update) > 500) {
                update_watchface_stats();
                last_update = millis();
            }
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

void backgroundSyncTime(void *pvParameters) {
    (void) pvParameters;

    uint32_t last_update = millis();
    uint32_t seconds_to_resync = 0;
    for(;;) {
        if((uint32_t)(millis() - last_update) > 999) {
            seconds_to_resync++;
            last_update = millis();
        }

        // if(awake_time == 0 && total_awake_time > 60)  {
        if(awake_time == 0)  {
            syncTimeFromRTC();
            USBSerial.println("Sync time after wake up.");
            // USBSerial.print("Display: "); USBSerial.println(display_status);
            // USBSerial.print("awake time: "); USBSerial.println(awake_time++);
            // To delay for at least 1 second later
            vTaskDelay(2000);
            seconds_to_resync = 0;
        }

        if(seconds_to_resync > 600) {
            syncTimeFromRTC();
            USBSerial.println("Sync time after 600 seconds");
            seconds_to_resync = 0;
        }

        vTaskDelay(10);
    }
}
