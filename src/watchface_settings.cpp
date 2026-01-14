#include <Arduino.h>
#include "watchface_settings.h"

static lv_obj_t *ui_Slider = NULL;
static lv_obj_t *ui_Label = NULL;
static lv_obj_t *ui_Title = NULL;

// Callback when slider moves
static void slider_event_cb(lv_event_t * e) {
    // FIX: Explicitly cast (void*) to (lv_obj_t*)
    lv_obj_t * slider = (lv_obj_t*)lv_event_get_target(e);
    
    int val = (int)lv_slider_get_value(slider);
    
    // Update Hardware
    setBrightness(val);
    
    // Update Label
    lv_label_set_text_fmt(ui_Label, "Brightness: %d%%", val);
    // USBSerial.printf("Slider: %d\n", val);
}

void load_watchface_settings() {
    lv_obj_clean(lv_scr_act());

    int current_val = getBrightness();

    ui_Title = lv_label_create(lv_scr_act());
    lv_obj_align(ui_Title, LV_ALIGN_TOP_MID, 0, 30); // Position at top
    lv_obj_set_style_text_font(ui_Title, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_color(ui_Title, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(ui_Title, "SETTINGS");

    // 2. Create Slider
    ui_Slider = lv_slider_create(lv_scr_act());
    lv_obj_set_size(ui_Slider, 30, 300);
    lv_obj_center(ui_Slider);
    lv_slider_set_range(ui_Slider, 5, 100);
    
    // 3. Set the slider to the saved position
    lv_slider_set_value(ui_Slider, current_val, LV_ANIM_OFF); 
    
    lv_obj_add_event_cb(ui_Slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 4. Update Label immediately
    ui_Label = lv_label_create(lv_scr_act());
    lv_obj_align(ui_Label, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_set_style_text_font(ui_Label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(ui_Label, lv_color_hex(0xFFFFFF), 0);
    
    // Set text to match current value
    lv_label_set_text_fmt(ui_Label, "Brightness: %d%%", current_val);
}
