#include "watchface_terminal.h"

// --- UI Objects ---
static lv_obj_t *ui_Bg = NULL;
static lv_obj_t *ui_StatusBar = NULL;
static lv_obj_t *ui_BattIcon = NULL;
static lv_obj_t *ui_BattPct = NULL;

static lv_obj_t *ui_ConsoleGroup = NULL;
static lv_obj_t *ui_LinePrompt = NULL; // "root@watch:~# date"
static lv_obj_t *ui_LineOutput = NULL; // The date string being typed
static lv_obj_t *ui_LineCursor = NULL; // The blinking cursor / next prompt

// --- Logic State ---
static char target_date_str[64];   // The full string we want to display
static char current_typed_str[64]; // What is currently displayed
static int  typing_index = 0;      // Current character position
static bool is_typing = false;
static int  prev_minute = -1;      // To trigger re-typing on minute change

// --- Helpers ---

// Get the correct battery icon symbol based on percentage
const char* get_batt_symbol(int pct, bool charging) {
    if(charging) return LV_SYMBOL_CHARGE;
    if(pct >= 90) return LV_SYMBOL_BATTERY_FULL;
    if(pct >= 70) return LV_SYMBOL_BATTERY_3;
    if(pct >= 50) return LV_SYMBOL_BATTERY_2;
    if(pct >= 20) return LV_SYMBOL_BATTERY_1;
    return LV_SYMBOL_BATTERY_EMPTY;
}

void load_watchface_terminal() {
    lv_obj_clean(lv_scr_act());

    // 1. Background (Terminal Black)
    ui_Bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_Bg, 410, 502); // Full Screen
    lv_obj_center(ui_Bg);
    lv_obj_set_style_bg_color(ui_Bg, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_Bg, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ui_Bg, LV_OBJ_FLAG_SCROLLABLE);

    // 2. Top Status Bar (Grey strip for battery)
    ui_StatusBar = lv_obj_create(ui_Bg);
    lv_obj_set_size(ui_StatusBar, 410, 30);
    lv_obj_align(ui_StatusBar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(ui_StatusBar, lv_color_hex(0x202020), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_StatusBar, 0, LV_PART_MAIN);
    
    // Battery Percentage
    ui_BattPct = lv_label_create(ui_StatusBar);
    lv_obj_align(ui_BattPct, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_text_color(ui_BattPct, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(ui_BattPct, &lv_font_montserrat_14, 0);
    lv_label_set_text(ui_BattPct, "100%");

    // Battery Icon
    ui_BattIcon = lv_label_create(ui_StatusBar);
    lv_obj_align(ui_BattIcon, LV_ALIGN_RIGHT_MID, -45, 0); // Left of percentage
    lv_obj_set_style_text_color(ui_BattIcon, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(ui_BattIcon, LV_SYMBOL_BATTERY_FULL);

    // 3. Console Group (Container for text)
    ui_ConsoleGroup = lv_obj_create(ui_Bg);
    lv_obj_set_size(ui_ConsoleGroup, 400, 460);
    lv_obj_align(ui_ConsoleGroup, LV_ALIGN_TOP_LEFT, 5, 35); // Below status bar
    lv_obj_set_style_bg_color(ui_ConsoleGroup, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_ConsoleGroup, 0, LV_PART_MAIN);
    
    // Style for all terminal text (Green, Monospace-ish look)
    lv_style_t style_term;
    lv_style_init(&style_term);
    lv_style_set_text_color(&style_term, lv_color_hex(0x00FF00)); // Hacker Green
    lv_style_set_text_font(&style_term, &lv_font_montserrat_20); // Using 20 for readability

    // Line 1: The Prompt and Command
    ui_LinePrompt = lv_label_create(ui_ConsoleGroup);
    lv_obj_add_style(ui_LinePrompt, &style_term, 0);
    lv_obj_align(ui_LinePrompt, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(ui_LinePrompt, "root@watch:~# date");

    // Line 2: The Output (Date/Time)
    ui_LineOutput = lv_label_create(ui_ConsoleGroup);
    lv_obj_add_style(ui_LineOutput, &style_term, 0);
    lv_obj_align(ui_LineOutput, LV_ALIGN_TOP_LEFT, 0, 30);
    lv_label_set_text(ui_LineOutput, ""); // Start empty

    // Line 3: The Next Prompt (appears after typing)
    ui_LineCursor = lv_label_create(ui_ConsoleGroup);
    lv_obj_add_style(ui_LineCursor, &style_term, 0);
    lv_obj_align(ui_LineCursor, LV_ALIGN_TOP_LEFT, 0, 60);
    lv_label_set_text(ui_LineCursor, ""); 

    // Reset State
    typing_index = 0;
    is_typing = true;
    prev_minute = -1;
    
    // Prepare the text immediately
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        // Format: "Tue Jan 13 11:53:00 2026"
        strftime(target_date_str, sizeof(target_date_str), "%a %b %d %H:%M:%S %Y", &timeinfo);
        prev_minute = timeinfo.tm_min;
    } else {
        sprintf(target_date_str, "Time Sync Error...");
    }
    
    update_watchface_terminal();
}

void update_watchface_terminal() {
    if(!ui_LineOutput) return;

    // --- 1. Battery Update Logic ---
    static uint32_t last_batt = 0;
    if(millis() - last_batt > 2000) { // Update every 2s
        last_batt = millis();
        int pct = power.getBatteryPercent();
        bool chg = power.isCharging();
        
        lv_label_set_text_fmt(ui_BattPct, "%d%%", pct);
        lv_label_set_text(ui_BattIcon, get_batt_symbol(pct, chg));
        
        // Color code battery
        if(pct < 20) lv_obj_set_style_text_color(ui_BattPct, lv_color_hex(0xFF0000), 0);
        else lv_obj_set_style_text_color(ui_BattPct, lv_color_hex(0xFFFFFF), 0);
    }

    // --- 2. Check for Minute Change (Reset Animation) ---
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        if(timeinfo.tm_min != prev_minute && !is_typing) {
            // Minute changed! Restart the typing process
            prev_minute = timeinfo.tm_min;
            typing_index = 0;
            is_typing = true;
            
            // Generate new string with current seconds
            strftime(target_date_str, sizeof(target_date_str), "%a %b %d %H:%M:%S %Y", &timeinfo);
            
            // Clear screen lines
            lv_label_set_text(ui_LineOutput, "");
            lv_label_set_text(ui_LineCursor, "");
        }
    }

    // --- 3. Typing Animation ---
    if(is_typing) {
        // Check if we reached the end of the string
        if(target_date_str[typing_index] == '\0') {
            is_typing = false;
            // Show the prompt again at the bottom
            lv_label_set_text(ui_LineCursor, "root@watch:~# _");
        } else {
            // Copy one character
            current_typed_str[typing_index] = target_date_str[typing_index];
            current_typed_str[typing_index + 1] = '\0'; // Null terminate
            
            // Update Label
            lv_label_set_text(ui_LineOutput, current_typed_str);
            
            // Move to next char
            typing_index++;
        }
    } else {
        // Blinking Cursor effect when idle
        static uint32_t cursor_blink = 0;
        if(millis() - cursor_blink > 800) {
            cursor_blink = millis();
            // Toggle cursor
            const char* txt = lv_label_get_text(ui_LineCursor);
            if(strcmp(txt, "root@watch:~# _") == 0) {
                lv_label_set_text(ui_LineCursor, "root@watch:~#");
            } else {
                lv_label_set_text(ui_LineCursor, "root@watch:~# _");
            }
        }
    }
}
