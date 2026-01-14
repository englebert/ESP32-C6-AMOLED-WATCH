#include "watchface_settings.h"

static lv_obj_t *ui_Slider = NULL;
static lv_obj_t *ui_Label = NULL;

// Callback when slider moves
static void slider_event_cb(lv_event_t * e) {
    // FIX: Explicitly cast (void*) to (lv_obj_t*)
    lv_obj_t * slider = (lv_obj_t*)lv_event_get_target(e);
    
    int val = (int)lv_slider_get_value(slider);
    
    // Update Hardware
    setBrightness(val);
    
    // Update Label
    lv_label_set_text_fmt(ui_Label, "Brightness: %d%%", val);
}

void load_watchface_settings() {
    lv_obj_clean(lv_scr_act());

    // 1. Title
    lv_obj_t * title = lv_label_create(lv_scr_act());
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(title, "Settings");

    // 2. Create Slider
    ui_Slider = lv_slider_create(lv_scr_act());
    lv_obj_set_size(ui_Slider, 30, 300); // Vertical slider
    lv_obj_center(ui_Slider);
    lv_slider_set_range(ui_Slider, 5, 100); // Min 5% to prevent total blackout
    lv_slider_set_value(ui_Slider, 80, LV_ANIM_OFF); // Default value
    
    // Add Event
    lv_obj_add_event_cb(ui_Slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 3. Percentage Label
    ui_Label = lv_label_create(lv_scr_act());
    lv_obj_align(ui_Label, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_set_style_text_font(ui_Label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(ui_Label, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(ui_Label, "Brightness: 80%");
}
