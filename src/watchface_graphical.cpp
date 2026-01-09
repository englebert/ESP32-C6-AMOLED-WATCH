#include "watchface_graphical.h"

// Static pointers to access objects during updates
static lv_obj_t *ui_Container = NULL;
static lv_obj_t *ui_TimeLabel = NULL;
static lv_obj_t *ui_DateLabel = NULL;
static lv_obj_t *ui_BattLabel = NULL;

// Helper to format time
static char time_buffer[16];
static char date_buffer[32];

void load_watchface_graphical() {
    // 1. Clean the screen first (remove objects from previous pages)
    lv_obj_clean(lv_scr_act());

    // 2. Create a main container (acts as the background)
    ui_Container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_Container, LCD_WIDTH, LCD_HEIGHT); 
    lv_obj_center(ui_Container);
    // lv_obj_set_style_bg_color(ui_Container, lv_color_hex(0x101010), LV_PART_MAIN); // Dark Gray
    lv_obj_set_style_bg_color(ui_Container, lv_color_hex(0x000000), LV_PART_MAIN); // Dark Gray
    lv_obj_set_style_border_width(ui_Container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ui_Container, LV_OBJ_FLAG_SCROLLABLE);

    // 3. Create Time Label (Big Font)
    ui_TimeLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_TimeLabel, LV_ALIGN_CENTER, 0, -20);
    // You can use standard fonts like lv_font_montserrat_48 if enabled in lv_conf.h
    // Or use default large font:
    lv_obj_set_style_text_font(ui_TimeLabel, &lv_font_montserrat_48, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_TimeLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text(ui_TimeLabel, "");

    // 4. Create Date Label (Small Font)
    ui_DateLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_DateLabel, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_text_font(ui_DateLabel, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_DateLabel, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_label_set_text(ui_DateLabel, "");

    // 5. Battery Label (Top Right)
    ui_BattLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_BattLabel, LV_ALIGN_TOP_RIGHT, -20, 10);
    lv_obj_set_style_text_font(ui_BattLabel, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_label_set_text(ui_BattLabel, "");
    
    // Force an immediate update
    update_watchface_graphical();
}

void update_watchface_graphical() {
    if (!ui_TimeLabel) return; // Safety check

    // Get Time
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        // Update Time
        strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &timeinfo);
        lv_label_set_text(ui_TimeLabel, time_buffer);

        // Update Date
        strftime(date_buffer, sizeof(date_buffer), "%a, %d %b", &timeinfo);
        lv_label_set_text(ui_DateLabel, date_buffer);
    }

    // Get Battery (using your power object)
    int batt = power.getBatteryPercent();
    lv_label_set_text_fmt(ui_BattLabel, "%d%%", batt);
    
    // Change color based on battery level
    if(batt < 20) {
        lv_obj_set_style_text_color(ui_BattLabel, lv_color_hex(0xFF0000), LV_PART_MAIN);
    } else {
        if(power.isCharging()) {
            lv_obj_set_style_text_color(ui_BattLabel, lv_color_hex(0x0000FF), LV_PART_MAIN);
        } else {
            lv_obj_set_style_text_color(ui_BattLabel, lv_color_hex(0x00FF00), LV_PART_MAIN);
        }
    }
}
