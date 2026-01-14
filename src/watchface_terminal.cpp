#include "watchface_terminal.h"
// #include <Wire.h> // Needed for I2C scan

// --- UI Objects ---
static lv_obj_t *ui_Bg = NULL;
static lv_obj_t *ui_Container = NULL;
static lv_obj_t *ui_LinePrompt = NULL; 
static lv_obj_t *ui_LineOutput = NULL; 
static lv_obj_t *ui_LineCursor = NULL; 

// --- Logic State ---
enum TerminalState {
    STATE_TYPING_CMD,
    STATE_TYPING_OUTPUT,
    STATE_WAITING
};

static TerminalState current_state = STATE_TYPING_CMD;

// Buffers
static char full_output_text[512]; // Storage for the complete multi-line info
static char current_output_buf[512]; // Buffer for animation
static char current_cmd_buf[64];     // Buffer for command line
static int  type_idx = 0;            // Current character index
static uint32_t state_timer = 0;     // Timer for delays

static const char* PROMPT_PREFIX = "root@watch:~# ";
static const char* CMD_TEXT = "./sys_info.sh";

// --- Styles ---
static lv_style_t style_prompt; // Yellow
static lv_style_t style_output; // Light Grey (Monospaced preferred if available)
static bool style_initialized = false;

// --- Helper: I2C Scan ---
// Scans the bus and returns a comma-separated string of found addresses
void get_i2c_devices(char* out_str) {
    int found = 0;
    out_str[0] = '\0'; // Clear string

    for(uint8_t address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();

        if (error == 0) {
            char addr_buf[10];
            if(found > 0) strcat(out_str, ", ");
            sprintf(addr_buf, "0x%02X", address);
            strcat(out_str, addr_buf);
            found++;
        }
    }
    if(found == 0) strcpy(out_str, "None");
}

void reset_terminal_cycle() {
    // 1. Reset State
    current_state = STATE_TYPING_CMD;
    type_idx = 0;
    current_output_buf[0] = '\0';
    
    // 2. Clear UI
    lv_label_set_text(ui_LinePrompt, PROMPT_PREFIX); 
    lv_label_set_text(ui_LineOutput, "");
    lv_label_set_text(ui_LineCursor, "");
    
    // 3. Gather System Info
    struct tm timeinfo;
    char date_str[32];
    char time_str[32];
    
    if (getLocalTime(&timeinfo)) {
        strftime(date_str, sizeof(date_str), "%a %d %b %Y", &timeinfo);
        strftime(time_str, sizeof(time_str), "%H:%M:%S", &timeinfo);
    } else {
        strcpy(date_str, "---");
        strcpy(time_str, "---");
    }

    int batt_pct = power.getBatteryPercent();
    int batt_mv  = power.getBattVoltage();
    
    char i2c_list[64];
    get_i2c_devices(i2c_list);

    // Format the full output string
    // We use \n for line breaks
    sprintf(full_output_text, 
        "Date   : %s\n"
        "Time   : %s\n"
        "Uptime : %d s\n"
        "Battery: %d mV (%d%%)\n"
        "I2C Bus: %s",
        date_str, 
        time_str, 
        total_awake_time, // Defined in processes.h
        batt_mv, 
        batt_pct,
        i2c_list
    );
}

void load_watchface_terminal() {
    lv_obj_clean(lv_scr_act());

    // 1. Initialize Styles
    if (!style_initialized) {
        lv_style_init(&style_prompt);
        lv_style_set_text_color(&style_prompt, lv_color_hex(0xFFFF00)); // Yellow
        lv_style_set_text_font(&style_prompt, &lv_font_montserrat_20);  // Smaller font to fit more text

        lv_style_init(&style_output);
        lv_style_set_text_color(&style_output, lv_color_hex(0xCCCCCC)); // Light Grey
        lv_style_set_text_font(&style_output, &lv_font_montserrat_20); 
        lv_style_set_text_line_space(&style_output, 8); // Add spacing between lines

        style_initialized = true;
    }

    // 2. Background
    ui_Bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_Bg, 410, 502); 
    lv_obj_center(ui_Bg);
    lv_obj_set_style_bg_color(ui_Bg, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_Bg, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ui_Bg, LV_OBJ_FLAG_SCROLLABLE);

    // 3. Container for text (Padding)
    ui_Container = lv_obj_create(ui_Bg);
    lv_obj_set_size(ui_Container, 390, 480);
    lv_obj_center(ui_Container);
    lv_obj_set_style_bg_color(ui_Container, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_Container, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(ui_Container, LV_FLEX_FLOW_COLUMN); // Auto-layout items vertically
    
    // Line 1: Prompt
    ui_LinePrompt = lv_label_create(ui_Container);
    lv_obj_add_style(ui_LinePrompt, &style_prompt, 0);
    lv_label_set_long_mode(ui_LinePrompt, LV_LABEL_LONG_WRAP); // Allow wrap
    lv_obj_set_width(ui_LinePrompt, 380);
    
    // Line 2: Output Block
    ui_LineOutput = lv_label_create(ui_Container);
    lv_obj_add_style(ui_LineOutput, &style_output, 0);
    lv_label_set_long_mode(ui_LineOutput, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(ui_LineOutput, 380);

    // Line 3: Next Cursor
    ui_LineCursor = lv_label_create(ui_Container);
    lv_obj_add_style(ui_LineCursor, &style_prompt, 0);

    // Start
    reset_terminal_cycle();
}

void update_watchface_terminal() {
    if(!ui_LineOutput) return;

    switch (current_state) {
        
        case STATE_TYPING_CMD:
            // Type "./sys_info.sh"
            if (CMD_TEXT[type_idx] == '\0') {
                // Finished command
                lv_label_set_text_fmt(ui_LinePrompt, "%s%s", PROMPT_PREFIX, CMD_TEXT); 
                current_state = STATE_TYPING_OUTPUT;
                type_idx = 0;
            } else {
                // Build string: "root@watch:~# ./s_"
                strcpy(current_cmd_buf, PROMPT_PREFIX);
                strncat(current_cmd_buf, CMD_TEXT, type_idx + 1);
                strcat(current_cmd_buf, "_"); 
                
                lv_label_set_text(ui_LinePrompt, current_cmd_buf);
                type_idx++;
            }
            break;

        case STATE_TYPING_OUTPUT:
            // Type the multi-line result char by char
            if (full_output_text[type_idx] == '\0') {
                // Done
                current_state = STATE_WAITING;
                state_timer = millis();
                lv_label_set_text(ui_LineCursor, "root@watch:~# _");
            } else {
                // Append next char to buffer
                current_output_buf[type_idx] = full_output_text[type_idx];
                current_output_buf[type_idx + 1] = '\0';
                
                // Add a "block cursor" at the end for effect
                // (Optional, can just set text directly for faster speed)
                lv_label_set_text_fmt(ui_LineOutput, "%s_", current_output_buf);
                
                // Type faster (increment by 2 or 3 chars per frame if it's too slow)
                type_idx++; 
            }
            break;

        case STATE_WAITING:
            // Wait 5 seconds before refreshing info
            if ((uint32_t)(millis() - state_timer) > 5000) {
                reset_terminal_cycle();
            } else {
                // Blink cursor
                static uint32_t blink_tmr = 0;
                if ((uint32_t)(millis() - blink_tmr) > 500) {
                    blink_tmr = millis();
                    const char* txt = lv_label_get_text(ui_LineCursor);
                    if (strcmp(txt, "root@watch:~# _") == 0) {
                        lv_label_set_text(ui_LineCursor, "root@watch:~#");
                    } else {
                        lv_label_set_text(ui_LineCursor, "root@watch:~# _");
                    }
                }
            }
            break;
    }
}
