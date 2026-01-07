#include <Arduino.h>
#include "processes.h"

void backgroundUpdate(void *pvParameters) {
    (void) pvParameters;

    uint32_t last_update = millis();

    for(;;) {
        lv_task_handler();

        // Main watch face
        if(page == 0) {
            if((uint32_t)(millis() - last_update) > 500) {
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
