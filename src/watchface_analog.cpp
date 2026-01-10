#include "watchface_analog.h"
#include <sys/time.h> // Required for gettimeofday (milliseconds)

// Screen Constants
#define SCREEN_WIDTH  410
#define SCREEN_HEIGHT 502
#define FACE_SIZE     400  // Diameter of the clock face

static lv_obj_t *ui_Bg = NULL;
static lv_obj_t *ui_Face = NULL;
static lv_obj_t *ui_HourHand = NULL;
static lv_obj_t *ui_MinHand = NULL;
static lv_obj_t *ui_SecHand = NULL;
static lv_obj_t *ui_CenterDot = NULL;
static lv_obj_t *ui_BattLabel = NULL;

/**
 * Helper to create a hand
 */
static lv_obj_t* create_hand(lv_obj_t *parent, int w, int h, int color_hex, int pivot_y_offset) {
    lv_obj_t *hand = lv_obj_create(parent);
    lv_obj_set_size(hand, w, h);
    lv_obj_set_style_bg_color(hand, lv_color_hex(color_hex), LV_PART_MAIN);
    lv_obj_set_style_border_width(hand, 0, LV_PART_MAIN);
    
    // Round the edges for a smoother look
    lv_obj_set_style_radius(hand, w / 2, LV_PART_MAIN); 

    // Align center then shift up
    lv_obj_align(hand, LV_ALIGN_CENTER, 0, -(h / 2) + pivot_y_offset);

    // Set Pivot point logic
    lv_obj_set_style_transform_pivot_x(hand, w / 2, LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_y(hand, h - pivot_y_offset, LV_PART_MAIN);
    
    return hand;
}

void load_watchface_analog() {
    lv_obj_clean(lv_scr_act());

    // 1. Black Background
    ui_Bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_Bg, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_center(ui_Bg);
    lv_obj_set_style_bg_color(ui_Bg, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_Bg, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ui_Bg, LV_OBJ_FLAG_SCROLLABLE);

    // 2. Clock Face Container
    ui_Face = lv_obj_create(ui_Bg);
    lv_obj_set_size(ui_Face, FACE_SIZE, FACE_SIZE);
    lv_obj_center(ui_Face);
    lv_obj_set_style_bg_color(ui_Face, lv_color_hex(0x111111), LV_PART_MAIN); 
    lv_obj_set_style_radius(ui_Face, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_Face, 3, LV_PART_MAIN);
    lv_obj_set_style_border_color(ui_Face, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_clear_flag(ui_Face, LV_OBJ_FLAG_SCROLLABLE);

    // 3. Create Tick Marks
    for(int i=0; i<12; i++) {
        bool is_major = (i % 3 == 0);
        int w = is_major ? 8 : 4;
        int h = is_major ? 25 : 15;
        
        lv_obj_t *tick = lv_obj_create(ui_Face);
        lv_obj_set_size(tick, w, h);
        lv_obj_set_style_bg_color(tick, lv_color_hex(is_major ? 0xFFFFFF : 0x808080), LV_PART_MAIN);
        lv_obj_set_style_border_width(tick, 0, LV_PART_MAIN);
        
        lv_obj_align(tick, LV_ALIGN_TOP_MID, 0, -8); 
        
        lv_obj_set_style_transform_pivot_x(tick, w/2, LV_PART_MAIN);
        lv_obj_set_style_transform_pivot_y(tick, (FACE_SIZE/2) - 10, LV_PART_MAIN);
        
        lv_obj_set_style_transform_angle(tick, i * 300, LV_PART_MAIN);
    }

    // 4. Create Hands
    ui_HourHand = create_hand(ui_Face, 16, 120, 0xFFFFFF, 0);
    ui_MinHand = create_hand(ui_Face, 10, 180, 0xAAAAAA, 0);
    ui_SecHand = create_hand(ui_Face, 4, 230, 0xFF4500, 40);

    // 5. Center Cap
    ui_CenterDot = lv_obj_create(ui_Face);
    lv_obj_set_size(ui_CenterDot, 20, 20);
    lv_obj_set_style_radius(ui_CenterDot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_CenterDot, lv_color_hex(0xFF4500), LV_PART_MAIN);
    lv_obj_center(ui_CenterDot);

    // Label
    lv_obj_t *date_label = lv_label_create(ui_Face);
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 80);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0x888888), 0);
    lv_label_set_text(date_label, "WATCH");

    // Battery Label
    ui_BattLabel = lv_label_create(ui_Bg);
    lv_obj_align(ui_BattLabel, LV_ALIGN_TOP_RIGHT, -20, 10);
    lv_obj_set_style_text_font(ui_BattLabel, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_label_set_text(ui_BattLabel, "");

    update_watchface_analog();
}

void update_watchface_analog() {
    if(!ui_SecHand) return;

    // 1. Get High Resolution Time
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Convert to standard struct tm for hours/min
    struct tm *timeinfo = localtime(&tv.tv_sec);
    
    if (timeinfo) {
        // Calculate Milliseconds from Microseconds
        int millis = tv.tv_usec / 1000;

        // --- Seconds Calculation (Smooth) ---
        // 1 Second = 60 units (6.0 degrees)
        // 1 ms = 0.06 units
        // Angle = (Seconds * 60) + (Millis * 0.06)
        int sec_angle = (timeinfo->tm_sec * 60) + (millis * 6 / 100);

        // --- Minute Calculation (Smooth) ---
        // 1 Minute = 60 units * 60 = 3600 total (wait, no)
        // 1 Minute = 6 degrees (60 units)
        // Contribution from seconds = Seconds (0..59)
        int min_angle = (timeinfo->tm_min * 60) + timeinfo->tm_sec; 

        // --- Hour Calculation ---
        int hour_angle = (timeinfo->tm_hour % 12) * 300 + (timeinfo->tm_min * 5);

        // Apply rotation
        lv_obj_set_style_transform_angle(ui_SecHand, sec_angle, LV_PART_MAIN);
        lv_obj_set_style_transform_angle(ui_MinHand, min_angle, LV_PART_MAIN);
        lv_obj_set_style_transform_angle(ui_HourHand, hour_angle, LV_PART_MAIN);
    }

    // 2. Battery Update (Throttled to once per second)
    // Reading I2C every 100ms can cause stutter, so we limit it.
    static uint32_t last_batt_update = 0;
    if ((uint32_t)(millis() - last_batt_update) > 999) {
        last_batt_update = millis();

        int batt = power.getBatteryPercent();
        lv_label_set_text_fmt(ui_BattLabel, "%d%%", batt);
        
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
}
