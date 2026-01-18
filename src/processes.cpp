#include <Arduino.h>
#include "processes.h"

uint32_t total_awake_time = 0;
uint32_t awake_time = 0;

void backgroundUpdate(void *pvParameters) {
    (void) pvParameters;

    uint32_t last_update = millis();
    bool is_dimmed = false; // Track dim state

    // Motion Detection Variables
    float acc_x, acc_y, acc_z;
    static float last_acc_x = 0, last_acc_y = 0, last_acc_z = 0;
    
    // --- FIX 1: Removed the "Ghost" label creation here ---
    // lv_obj_t *label = lv_label_create(lv_scr_act()); 

    // Initially force an update
    page_change = true;

    for(;;) {
        // Let LVGL manage the UI (inputs, animations, etc.)
        lv_task_handler();

        // ---------------------------------------------------------
        // CHECK FOR MOVEMENT (IMU)
        // ---------------------------------------------------------
        if (imu.getAccelerometer(acc_x, acc_y, acc_z)) {
            // Calculate total change in acceleration (Sensitivity)
            float delta = fabs(acc_x - last_acc_x) + fabs(acc_y - last_acc_y) + fabs(acc_z - last_acc_z);
            
            // If movement > 0.2g (adjust as needed), reset timer
            if (delta > 0.2) {
                awake_time = 0; 
            }
            
            last_acc_x = acc_x;
            last_acc_y = acc_y;
            last_acc_z = acc_z;
        }

        // ---------------------------------------------------------
        // AUTO-DIM LOGIC
        // ---------------------------------------------------------
        // If idle for 10+ seconds: DIM
        if(awake_time >= 10) {
            if(!is_dimmed) {
                dimDisplay(); 
                is_dimmed = true;
                // USBSerial.println("Auto-Dim: ON");
            }
        }
        // If active (< 10 seconds): RESTORE
        else {
            if(is_dimmed) {
                restoreBrightness();
                is_dimmed = false;
                // USBSerial.println("Auto-Dim: OFF");
            }
        }

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

        // --- NEW PAGE 4: WiFi Manager ---
        else if(page == 4) {
            if(page_change) {
                page_change = false;
                lv_obj_clean(lv_scr_act()); 
                gfx->fillScreen(BLACK);
                
                load_watchface_wifi();
            }
            
            // Need to update often to check scan status
            if((uint32_t)(millis() - last_update) > 200) {
                update_watchface_wifi();
                last_update = millis();
            }
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
    uint32_t last_ntp_sync = millis();
    uint32_t seconds_to_resync = 0;
    for(;;) {
        if((uint32_t)(millis() - last_ntp_sync) > 999) {
            if(_was_connected)
                syncNTP();
            last_ntp_sync = millis();
        }

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

void backgroundMonitorWiFi(void *pvParameters) {
    (void) pvParameters;

    init_wifi_manager();
    uint32_t last_update = millis();

    for(;;) {
        if((uint32_t)(millis() - last_update) > 2000) {
            monitor_wifi();
            last_update = millis();
        }

        vTaskDelay(10);
    }
}
