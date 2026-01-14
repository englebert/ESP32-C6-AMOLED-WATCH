#include "watchface_wifi.h"
#include "wifi_manager.h"
#include <WiFi.h>

// UI Objects
static lv_obj_t *ui_List = NULL;
static lv_obj_t *ui_StatusLabel = NULL;
static lv_obj_t *ui_Keyboard = NULL;
static lv_obj_t *ui_TextArea = NULL;
static lv_obj_t *ui_PassContainer = NULL;

// Global Styles for this page
static lv_style_t style_large_text;
static bool styles_initialized = false;

// State
static bool is_scanning = false;
static String selected_ssid = "";

// Forward declarations
static void event_keyboard_cb(lv_event_t * e);
static void event_list_btn_cb(lv_event_t * e);

void init_wifi_styles() {
    if (styles_initialized) return;
    
    lv_style_init(&style_large_text);
    // Use Size 28 Font
    lv_style_set_text_font(&style_large_text, &lv_font_montserrat_28);
    lv_style_set_text_color(&style_large_text, lv_color_hex(0xFFFFFF));
    
    styles_initialized = true;
}

// 1. Load the Page
void load_watchface_wifi() {
    init_wifi_styles();
    lv_obj_clean(lv_scr_act());

    // Title
    lv_obj_t * title = lv_label_create(lv_scr_act());
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);
    lv_obj_add_style(title, &style_large_text, 0); // Apply Size 28
    lv_label_set_text(title, "WiFi Manager");

    // Status Label
    ui_StatusLabel = lv_label_create(lv_scr_act());
    lv_obj_align(ui_StatusLabel, LV_ALIGN_TOP_MID, 0, 50);
    // Override color for status, but keep font size 28
    lv_obj_add_style(ui_StatusLabel, &style_large_text, 0); 
    lv_obj_set_style_text_color(ui_StatusLabel, lv_color_hex(0x00FF00), 0);
    
    if(is_wifi_connected()) {
        lv_label_set_text_fmt(ui_StatusLabel, "Connected");
    } else {
        lv_label_set_text(ui_StatusLabel, "Scanning...");
    }

    // List Container
    ui_List = lv_list_create(lv_scr_act());
    lv_obj_set_size(ui_List, 400, 380); // Made wider/taller for big fonts
    lv_obj_align(ui_List, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(ui_List, lv_color_hex(0x202020), 0);
    lv_obj_set_style_border_width(ui_List, 0, 0);
    
    // Set the Font for the entire List (buttons will inherit this)
    lv_obj_set_style_text_font(ui_List, &lv_font_montserrat_28, 0);

    // Start Scan
    WiFi.scanNetworks(true); 
    is_scanning = true;
}

// 2. Update Loop
void update_watchface_wifi() {
    if (is_scanning) {
        int n = WiFi.scanComplete();
        if (n >= 0) {
            is_scanning = false;
            lv_label_set_text(ui_StatusLabel, "Select WiFi:");
            
            lv_obj_clean(ui_List);
            for (int i = 0; i < n; ++i) {
                String ssid = WiFi.SSID(i);
                int rssi = WiFi.RSSI(i);
                
                // Truncate name if too long to fit 410px with size 28 font
                if(ssid.length() > 18) ssid = ssid.substring(0, 15) + "...";

                char buf[64];
                sprintf(buf, "%s (%d)", ssid.c_str(), rssi);
                
                lv_obj_t * btn = lv_list_add_btn(ui_List, LV_SYMBOL_WIFI, buf);
                lv_obj_add_event_cb(btn, event_list_btn_cb, LV_EVENT_CLICKED, NULL);
                
                // Increase button padding for larger text
                lv_obj_set_style_pad_top(btn, 15, 0);
                lv_obj_set_style_pad_bottom(btn, 15, 0);
            }
            WiFi.scanDelete();
        }
    }
    
    // Status Updates
    if (ui_StatusLabel && !is_scanning && !ui_Keyboard) {
        if(WiFi.status() == WL_CONNECTED) {
             lv_label_set_text(ui_StatusLabel, "Connected");
             lv_obj_set_style_text_color(ui_StatusLabel, lv_color_hex(0x00FF00), 0);
        } else if (WiFi.status() == WL_CONNECT_FAILED) {
             lv_label_set_text(ui_StatusLabel, "Failed");
             lv_obj_set_style_text_color(ui_StatusLabel, lv_color_hex(0xFF0000), 0);
        }
    }
}

// 3. Clicked Network
static void event_list_btn_cb(lv_event_t * e) {
    lv_obj_t * btn = (lv_obj_t*)lv_event_get_target(e); // Cast fixed
    const char * txt = lv_list_get_btn_text(ui_List, btn);
    
    String raw = String(txt);
    int p = raw.lastIndexOf(" (");
    if(p > 0) selected_ssid = raw.substring(0, p);
    else selected_ssid = raw;

    // Create Keyboard Overlay
    ui_PassContainer = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_PassContainer, 410, 502);
    lv_obj_set_style_bg_color(ui_PassContainer, lv_color_hex(0x000000), 0);
    lv_obj_center(ui_PassContainer);

    // Text Area (Password)
    ui_TextArea = lv_textarea_create(ui_PassContainer);
    lv_obj_set_size(ui_TextArea, 350, 60); // Taller for big font
    lv_obj_align(ui_TextArea, LV_ALIGN_TOP_MID, 0, 40);
    lv_textarea_set_placeholder_text(ui_TextArea, "Password...");
    lv_textarea_set_password_mode(ui_TextArea, false); 
    lv_obj_set_style_text_font(ui_TextArea, &lv_font_montserrat_28, 0); // Big Font

    // Keyboard
    ui_Keyboard = lv_keyboard_create(ui_PassContainer);
    lv_keyboard_set_textarea(ui_Keyboard, ui_TextArea);
    
    // --- KEYBOARD FONT MAGIC ---
    // Apply the font to the "ITEMS" (the keys)
    lv_obj_set_style_text_font(ui_Keyboard, &lv_font_montserrat_28, LV_PART_ITEMS);
    
    // Optional: Increase Keyboard height to make keys physically larger
    lv_obj_set_size(ui_Keyboard, 410, 260); 

    lv_obj_add_event_cb(ui_Keyboard, event_keyboard_cb, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(ui_Keyboard, event_keyboard_cb, LV_EVENT_CANCEL, NULL);
}

// 4. Input Done
static void event_keyboard_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_READY) { 
        const char * pass = lv_textarea_get_text(ui_TextArea);
        save_wifi_cred(selected_ssid, String(pass));
        
        lv_label_set_text(ui_StatusLabel, "Connecting...");
        WiFi.begin(selected_ssid.c_str(), pass);
        
        lv_obj_del(ui_PassContainer);
        ui_Keyboard = NULL;
        ui_TextArea = NULL;
        
    } else if (code == LV_EVENT_CANCEL) {
        lv_obj_del(ui_PassContainer);
        ui_Keyboard = NULL;
    }
}
