#include "watchface_analog.h"

// Screen Constants
#define SCREEN_WIDTH  410
#define SCREEN_HEIGHT 502
#define FACE_SIZE     410  // Diameter of the clock face

static lv_obj_t *ui_Bg = NULL;
static lv_obj_t *ui_Face = NULL;
static lv_obj_t *ui_HourHand = NULL;
static lv_obj_t *ui_MinHand = NULL;
static lv_obj_t *ui_SecHand = NULL;
static lv_obj_t *ui_CenterDot = NULL;
static lv_obj_t *ui_BattLabel = NULL;

/**
 * Helper to create a hand
 * w: width, h: height, color: hex color
 * pivot_y_offset: How far from the BOTTOM of the hand is the pivot point?
 * 0 = Pivot is at the very bottom edge.
 * 20 = Pivot is 20px up from the bottom (creates a "tail").
 */
static lv_obj_t* create_hand(lv_obj_t *parent, int w, int h, int color_hex, int pivot_y_offset) {
    lv_obj_t *hand = lv_obj_create(parent);
    lv_obj_set_size(hand, w, h);
    lv_obj_set_style_bg_color(hand, lv_color_hex(color_hex), LV_PART_MAIN);
    lv_obj_set_style_border_width(hand, 0, LV_PART_MAIN);
    
    // Round the edges for a smoother look
    lv_obj_set_style_radius(hand, w / 2, LV_PART_MAIN); 

    // Calculate position:
    // We want the Pivot Point to land exactly at the center of the parent.
    // By default, LVGL aligns the CENTER of the object.
    // We need to shift it UP so the pivot point matches the parent center.
    // Shift = -(Height/2 - PivotOffset)
    lv_obj_align(hand, LV_ALIGN_CENTER, 0, -(h / 2) + pivot_y_offset);

    // Set Pivot point logic (relative to the hand's top-left corner)
    lv_obj_set_style_transform_pivot_x(hand, w / 2, LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_y(hand, h - pivot_y_offset, LV_PART_MAIN);
    
    return hand;
}

void load_watchface_analog() {
    lv_obj_clean(lv_scr_act());

    // 1. Black Background (Fills the whole 410x502 rectangle)
    ui_Bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_Bg, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_center(ui_Bg);
    lv_obj_set_style_bg_color(ui_Bg, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_Bg, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ui_Bg, LV_OBJ_FLAG_SCROLLABLE);

    // 2. Clock Face Container (Circular, 410x410)
    // We put this in the center. The top/bottom 46px will be empty black space.
    ui_Face = lv_obj_create(ui_Bg);
    lv_obj_set_size(ui_Face, FACE_SIZE, FACE_SIZE);
    lv_obj_center(ui_Face);
    lv_obj_set_style_bg_color(ui_Face, lv_color_hex(0x111111), LV_PART_MAIN); // Very Dark Grey
    lv_obj_set_style_radius(ui_Face, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_Face, 3, LV_PART_MAIN);
    lv_obj_set_style_border_color(ui_Face, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_clear_flag(ui_Face, LV_OBJ_FLAG_SCROLLABLE);

    // 3. Create Tick Marks (12, 3, 6, 9)
    // Radius is 205. We place ticks near the edge.
    for(int i=0; i<12; i++) {
        bool is_major = (i % 3 == 0); // 12, 3, 6, 9 are Major ticks
        int w = is_major ? 8 : 4;
        int h = is_major ? 25 : 15;
        
        lv_obj_t *tick = lv_obj_create(ui_Face);
        lv_obj_set_size(tick, w, h);
        lv_obj_set_style_bg_color(tick, lv_color_hex(is_major ? 0xFFFFFF : 0x808080), LV_PART_MAIN);
        lv_obj_set_style_border_width(tick, 0, LV_PART_MAIN);
        
        // Start at Top Middle inside the Face
        lv_obj_align(tick, LV_ALIGN_TOP_MID, 0, 10); 
        
        // Pivot around the center of the face (Radius = 205)
        // Y-Pivot = Center (205) - TopMargin (10) = 195
        lv_obj_set_style_transform_pivot_x(tick, w/2, LV_PART_MAIN);
        lv_obj_set_style_transform_pivot_y(tick, (FACE_SIZE/2) - 10, LV_PART_MAIN);
        
        lv_obj_set_style_transform_angle(tick, i * 300, LV_PART_MAIN); // 30 degrees per hour
    }

    // 4. Create Hands (Scaled for 410px width)
    
    // Hour Hand: Thick (16px), Length 120px
    ui_HourHand = create_hand(ui_Face, 16, 120, 0xFFFFFF, 0);
    
    // Minute Hand: Medium (10px), Length 180px (reaches near ticks)
    ui_MinHand = create_hand(ui_Face, 10, 180, 0xAAAAAA, 0);
    
    // Second Hand: Thin (4px), Length 210px (Total), Tail 30px
    // Color: Orange-Red
    ui_SecHand = create_hand(ui_Face, 4, 230, 0xFF4500, 40);

    // 5. Center Cap
    ui_CenterDot = lv_obj_create(ui_Face);
    lv_obj_set_size(ui_CenterDot, 20, 20); // Bigger cap
    lv_obj_set_style_radius(ui_CenterDot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_CenterDot, lv_color_hex(0xFF4500), LV_PART_MAIN);
    lv_obj_center(ui_CenterDot);

    // Add a date label at the bottom (optional, since we have space)
    lv_obj_t *date_label = lv_label_create(ui_Face);
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 80); // Below center
    lv_obj_set_style_text_color(date_label, lv_color_hex(0x888888), 0);
    lv_label_set_text(date_label, "WATCH");

    ui_BattLabel = lv_label_create(ui_Bg);
    lv_obj_align(ui_BattLabel, LV_ALIGN_TOP_RIGHT, -20, 10);
    lv_obj_set_style_text_font(ui_BattLabel, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_label_set_text(ui_BattLabel, "");

    update_watchface_analog();
}

void update_watchface_analog() {
    if(!ui_SecHand) return;

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        // Calculate Angles (0.1 degree precision)
        int sec_angle  = timeinfo.tm_sec * 60;
        int min_angle  = timeinfo.tm_min * 60 + timeinfo.tm_sec; 
        int hour_angle = (timeinfo.tm_hour % 12) * 300 + (timeinfo.tm_min * 5);

        lv_obj_set_style_transform_angle(ui_SecHand, sec_angle, LV_PART_MAIN);
        lv_obj_set_style_transform_angle(ui_MinHand, min_angle, LV_PART_MAIN);
        lv_obj_set_style_transform_angle(ui_HourHand, hour_angle, LV_PART_MAIN);
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
