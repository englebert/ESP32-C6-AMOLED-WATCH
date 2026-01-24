#include "watchface_analog_simple.h"
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
static lv_obj_t *ui_DateLabel = NULL;
static lv_obj_t *ui_BattLabel = NULL;
static lv_obj_t *ui_BattIcon = NULL;
static lv_obj_t *ui_SleepCounterLabel = NULL;
static lv_obj_t *ui_GyroDeltaLabel = NULL;

static char date_buffer[32];

// Get the correct battery icon symbol based on percentage
const char* get_batt_symbol(int pct, bool charging) {
    if(charging) return LV_SYMBOL_CHARGE;
    if(pct >= 90) return LV_SYMBOL_BATTERY_FULL;
    if(pct >= 70) return LV_SYMBOL_BATTERY_3;
    if(pct >= 50) return LV_SYMBOL_BATTERY_2;
    if(pct >= 20) return LV_SYMBOL_BATTERY_1;
    return LV_SYMBOL_BATTERY_EMPTY;
}

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

void load_watchface_analog_simple() {
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
    lv_obj_set_style_border_width(ui_Face, 0, LV_PART_MAIN);
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

    // Label
    ui_DateLabel = lv_label_create(ui_Face);
    lv_obj_align(ui_DateLabel, LV_ALIGN_CENTER, 0, 80);
    lv_obj_set_style_text_font(ui_DateLabel, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_DateLabel, lv_color_hex(0x888888), 0);
    lv_label_set_text(ui_DateLabel, "");

    // 4. Create Hands
    ui_HourHand = create_hand(ui_Face, 16, 120, 0xFFFF00, 0);
    ui_MinHand = create_hand(ui_Face, 10, 180, 0xAAAAAA, 0);
    ui_SecHand = create_hand(ui_Face, 4, 230, 0xFF4500, 40);

    // 5. Center Cap
    ui_CenterDot = lv_obj_create(ui_Face);
    lv_obj_set_size(ui_CenterDot, 20, 20);
    lv_obj_set_style_radius(ui_CenterDot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_CenterDot, lv_color_hex(0xFF4500), LV_PART_MAIN);
    lv_obj_center(ui_CenterDot);

    // Battery Label
    ui_BattLabel = lv_label_create(ui_Bg);
    lv_obj_align(ui_BattLabel, LV_ALIGN_TOP_RIGHT, -60, 5);
    lv_obj_set_style_text_font(ui_BattLabel, &lv_font_montserrat_22, LV_PART_MAIN);
    lv_label_set_text(ui_BattLabel, "");

    // Battery Icon
    ui_BattIcon = lv_label_create(ui_Bg);
    lv_obj_align(ui_BattIcon, LV_ALIGN_TOP_RIGHT, -30, 4);
    lv_obj_set_style_text_color(ui_BattIcon, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(ui_BattIcon, &lv_font_montserrat_24, 0);
    // lv_image_set_scale(ui_BattIcon, 512);
    lv_label_set_text(ui_BattIcon, LV_SYMBOL_BATTERY_FULL);

    // Sleep counter Label
    ui_SleepCounterLabel = lv_label_create(ui_Bg);
    lv_obj_align(ui_SleepCounterLabel, LV_ALIGN_TOP_LEFT, 20, 6);
    lv_obj_set_style_text_color(ui_SleepCounterLabel, lv_color_hex(0xBBBBBB), 0);
    lv_obj_set_style_text_font(ui_SleepCounterLabel, &lv_font_montserrat_24, 0);
    lv_label_set_text(ui_SleepCounterLabel, "");
   
    // Gyro Delta 
    ui_GyroDeltaLabel = lv_label_create(ui_Bg);
    lv_obj_align(ui_GyroDeltaLabel, LV_ALIGN_TOP_MID, 0, 4);
    lv_obj_set_style_text_color(ui_GyroDeltaLabel, lv_color_hex(0x009900), 0);
    lv_obj_set_style_text_font(ui_GyroDeltaLabel, &lv_font_montserrat_24, 0);
    lv_label_set_text(ui_GyroDeltaLabel, "");
    

    update_watchface_analog_simple();
}

void update_watchface_analog_simple() {
    if(!ui_SecHand) return;

    // 1. Get High Resolution Time
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Convert to standard struct tm for hours/min
    struct tm *timeinfo = localtime(&tv.tv_sec);
    
    // Battery Update (Throttled to once per second)
    // Reading I2C every 100ms can cause stutter, so we limit it.
    static uint32_t last_batt_update = 0;
    if ((uint32_t)(millis() - last_batt_update) > 999) {
        last_batt_update = millis();

        // Get Time
        if (getLocalTime(timeinfo)) {
            // Update Time
            // strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &timeinfo);
            // lv_label_set_text(ui_TimeLabel, time_buffer);

            // Update Date
            strftime(date_buffer, sizeof(date_buffer), "%a, %d %b", timeinfo);
            lv_label_set_text(ui_DateLabel, date_buffer);
        }

        int batt = power.getBatteryPercent();
        bool chg = power.isCharging();
        lv_label_set_text_fmt(ui_BattLabel, "%d%%", batt);
        lv_label_set_text(ui_BattIcon, get_batt_symbol(batt, chg));
        lv_label_set_text_fmt(ui_SleepCounterLabel, "On: %d", sleep_count);
        
        if(batt < 20) {
            lv_obj_set_style_text_color(ui_BattLabel, lv_color_hex(0xFF0000), LV_PART_MAIN);
        } else {
            if(power.isCharging()) {
                lv_obj_set_style_text_color(ui_BattLabel, lv_color_hex(0xAAAAFF), LV_PART_MAIN);
            } else {
                lv_obj_set_style_text_color(ui_BattLabel, lv_color_hex(0x00FF00), LV_PART_MAIN);
            }
        }
    }

    static uint32_t last_gyro_update = 0;
    static char buffer[64];
    static uint16_t delay_last_gyro_update = 99;
    if ((uint32_t)(millis() - last_gyro_update) > delay_last_gyro_update) {
        sprintf(buffer, "Delta: %.2f", delta);
        lv_label_set_text_fmt(ui_GyroDeltaLabel, buffer);
        if(delta < 1.0) {
            delay_last_gyro_update = 99;
        } else {
            delay_last_gyro_update = 1999;
        }
        
        last_gyro_update = millis();
    }

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
}
