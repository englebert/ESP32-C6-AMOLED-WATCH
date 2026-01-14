#include <Arduino.h>
#include "processes.h"

uint32_t total_awake_time = 0;
uint32_t awake_time = 0;

void backgroundUpdate(void *pvParameters) {
    (void) pvParameters;

    uint32_t last_update = millis();
    
    // --- FIX 1: Removed the "Ghost" label creation here ---
    // lv_obj_t *label = lv_label_create(lv_scr_act()); 

    // Initially force an update
    page_change = true;

    for(;;) {
        // Let LVGL manage the UI (inputs, animations, etc.)
        lv_task_handler();

        if(awake_time > 30) {
            modeSleep();
        }

        // --- Page 2: Hello World (GFX) ---
        if(page == 2) {
            // --- FIX 2: Handle Page Entry for Page 2 ---
            if(page_change) {
                page_change = false;
                // Clean any LVGL objects from previous pages so they don't render in background
                lv_obj_clean(lv_scr_act()); 
                gfx->fillScreen(BLACK);
            }

            // Draw Random Text
            gfx->setCursor(random(gfx->width()), random(gfx->height()));
            gfx->setTextColor(random(0xffff), random(0xffff));
            gfx->setTextSize(random(6), random(6), random(2));
            gfx->println("Hello World!");
            gfx->flush();
        } 
        
        // --- Page 0: Main Watchfaces (LVGL) ---
        else if(page == 0) {
            if(page_change || watchface_change) {
                page_change = false;
                watchface_change = false;
                
                // Clear GFX screen to ensure clean background
                gfx->fillScreen(BLACK);

                // Build the LVGL UI
                if(watchface == 0) {
                    load_watchface_graphical();
                } else if(watchface == 1) {
                    load_watchface_analog();
                } else if(watchface == 2) {
                    load_watchface_analog_simple();
                } else if(watchface == 3) {
                    load_watchface_terminal();
                }
            }

            // HANDLE UPDATES
            if(watchface == 0) {
                if((uint32_t)(millis() - last_update) > 500) {
                    update_watchface_graphical();
                    last_update = millis();
                }
            } else if(watchface == 1) {
                if((uint32_t)(millis() - last_update) > 100) {
                    update_watchface_analog();
                    last_update = millis();
                }
            } else if(watchface == 2) {
                if((uint32_t)(millis() - last_update) > 100) {
                    update_watchface_analog_simple();
                    last_update = millis();
                }
            } else if(watchface == 3) {
                // Update FAST (50ms) for typing effect
                if((uint32_t)(millis() - last_update) > 30) {
                    update_watchface_terminal();
                    last_update = millis();
                }
            }
        } 
        
        // --- Page 1: Stats (LVGL) ---
        else if(page == 1) {
            if(page_change) {
                page_change = false;
                gfx->fillScreen(BLACK);
                // Load Stats UI
                load_watchface_stats();
            }

            // Update Stats
            if((uint32_t)(millis() - last_update) > 500) {
                update_watchface_stats();
                last_update = millis();
            }
        }

        // --- NEW PAGE 3: Settings ---
        else if(page == 3) {
            if(page_change) {
                page_change = false;
                // Clear screen
                lv_obj_clean(lv_scr_act()); 
                gfx->fillScreen(BLACK);
                
                // Load UI
                load_watchface_settings();
            }
            
            // No frequent update needed for settings, just let LVGL handle input
        }
        
        // --- Fallback/Error ---
        else {
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

        if(awake_time == 0)  {
            syncTimeFromRTC();
            USBSerial.println("Sync time after wake up.");
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
