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

                show_watchface02(label);
                
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
