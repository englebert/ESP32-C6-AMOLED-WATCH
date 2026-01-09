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
        if(page == 3) {
            gfx->setCursor(random(gfx->width()), random(gfx->height()));
            gfx->setTextColor(random(0xffff), random(0xffff));
            gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
            gfx->println("Hello World!");
            
            gfx->flush();
        } else if(page == 0) {
            if(page_change) {
                page_change = false;
                gfx->fillScreen(BLACK);

                // Build the LVGL UI
                load_watchface_graphical();
            }

            // HANDLE UPDATES (Run every 500ms)
            if((uint32_t)(millis() - last_update) > 500) {
                update_watchface_graphical();
                last_update = millis();
            }
        } else if(page == 1) {
            if(page_change) {
                page_change = false;
                gfx->fillScreen(BLACK);

                // Build the LVGL UI
                load_watchface_analog();
            }

            // HANDLE UPDATES (Run every 100ms)
            if((uint32_t)(millis() - last_update) > 50) {
                update_watchface_analog();
                last_update = millis();
            }
        } else if(page == 2) {
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
