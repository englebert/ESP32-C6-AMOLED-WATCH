#include <Arduino.h>
#include "processes.h"

void backgroundUpdate(void *pvParameters) {
    (void) pvParameters;

    for(;;) {
        lv_task_handler();

        // Main watch face
        if(page == 0) {
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
