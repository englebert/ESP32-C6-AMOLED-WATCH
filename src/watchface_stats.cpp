#include "watchface_stats.h"

// Static pointers to access objects during updates
static lv_obj_t *ui_Container      = NULL;
static lv_obj_t *ui_TitleLabel     = NULL;
static lv_obj_t *ui_ChipLabel      = NULL;
static lv_obj_t *ui_ChipRevLabel   = NULL;
static lv_obj_t *ui_ChipCoresLabel = NULL;
static lv_obj_t *ui_ChipFreqLabel  = NULL;
static lv_obj_t *ui_ChipSizeLabel  = NULL;
static lv_obj_t *ui_ChipHeapLabel  = NULL;
static lv_obj_t *ui_ChipFreeLabel  = NULL;
static lv_obj_t *ui_ChipMacLabel   = NULL;
static lv_obj_t *ui_BattVolLabel   = NULL;
static lv_obj_t *ui_VBusVolLabel   = NULL;
static lv_obj_t *ui_SysVolLabel    = NULL;
static lv_obj_t *ui_TempLabel      = NULL;
static lv_obj_t *ui_ChargingLabel  = NULL;
static lv_obj_t *ui_DisplayLabel   = NULL;

// Helper to format time
static char buffer[64];

void load_watchface_stats() {
    lv_obj_clean(lv_scr_act());

    ui_Container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_Container, LCD_WIDTH, LCD_HEIGHT); 
    lv_obj_center(ui_Container);
    lv_obj_set_style_bg_color(ui_Container, lv_color_hex(0x000010), LV_PART_MAIN); // Dark Gray
    lv_obj_set_style_border_width(ui_Container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ui_Container, LV_OBJ_FLAG_SCROLLABLE);

    ui_TitleLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_TitleLabel, LV_ALIGN_TOP_MID, 0, -10);
    lv_obj_set_style_text_font(ui_TitleLabel, &lv_font_montserrat_28, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_TitleLabel, lv_color_hex(0x00FF00), LV_PART_MAIN);
    lv_label_set_text(ui_TitleLabel, "STATS");
    
    uint16_t y = 0;

    ui_ChipLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_ChipLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_ChipLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_ChipLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    sprintf(buffer, "Chip Model: %s", ESP.getChipModel());
    lv_label_set_text(ui_ChipLabel, buffer);

    ui_ChipRevLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_ChipRevLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_ChipRevLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_ChipRevLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    sprintf(buffer, "Chip Revision: %d", ESP.getChipRevision());
    lv_label_set_text(ui_ChipRevLabel, buffer);

    ui_ChipCoresLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_ChipCoresLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_ChipCoresLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_ChipCoresLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    sprintf(buffer, "Chip Cores: %d", ESP.getChipCores());
    lv_label_set_text(ui_ChipCoresLabel, buffer);

    ui_ChipFreqLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_ChipFreqLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_ChipFreqLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_ChipFreqLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    sprintf(buffer, "Chip Frequency: %d", ESP.getCpuFreqMHz());
    lv_label_set_text(ui_ChipFreqLabel, buffer);

    ui_ChipSizeLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_ChipSizeLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_ChipSizeLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_ChipSizeLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    sprintf(buffer, "Flash Size: %dMB", ESP.getFlashChipSize() / (1024 * 1024));
    lv_label_set_text(ui_ChipSizeLabel, buffer);

    ui_ChipHeapLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_ChipHeapLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_ChipHeapLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_ChipHeapLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    sprintf(buffer, "Total Heap: %d byte(s)", ESP.getHeapSize());
    lv_label_set_text(ui_ChipHeapLabel, buffer);

    ui_ChipFreeLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_ChipFreeLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_ChipFreeLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_ChipFreeLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    sprintf(buffer, "Total Free: %d byte(s)", ESP.getFreeHeap());
    lv_label_set_text(ui_ChipFreeLabel, buffer);

    ui_ChipMacLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_ChipMacLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_ChipMacLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_ChipMacLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    uint64_t chipid = ESP.getEfuseMac();
    sprintf(buffer, "Mac Address: %02X:%02X:%02X:%02X:%02X:%02X",
                (uint8_t)(chipid),
                (uint8_t)(chipid >> 8),
                (uint8_t)(chipid >> 16),
                (uint8_t)(chipid >> 24),
                (uint8_t)(chipid >> 32),
                (uint8_t)(chipid >> 40));
    lv_label_set_text(ui_ChipMacLabel, buffer);

    ui_BattVolLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_BattVolLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_BattVolLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_BattVolLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text(ui_BattVolLabel, "");

    ui_VBusVolLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_VBusVolLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_VBusVolLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_VBusVolLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text(ui_VBusVolLabel, "");

    ui_SysVolLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_SysVolLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_SysVolLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_SysVolLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text(ui_SysVolLabel, "");

    ui_TempLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_TempLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_TempLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_TempLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text(ui_TempLabel, "");

    ui_ChargingLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_ChargingLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_ChargingLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_ChargingLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text(ui_ChargingLabel, "");

    ui_DisplayLabel = lv_label_create(ui_Container);
    lv_obj_align(ui_DisplayLabel, LV_ALIGN_TOP_LEFT, 0, y+=24);
    lv_obj_set_style_text_font(ui_DisplayLabel, &lv_font_montserrat_24, LV_PART_MAIN); 
    lv_obj_set_style_text_color(ui_DisplayLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    sprintf(buffer, "Screen Resolution: %d x %d", LCD_WIDTH, LCD_HEIGHT);
    lv_label_set_text(ui_DisplayLabel, buffer);

    // Force an immediate update
    update_watchface_stats();
}

void update_watchface_stats() {
    if (!ui_TitleLabel) return; // Safety check

    // sprintf(buffer, "Batt: %smV", power.getBattVoltage());
    // info += "Charging: " + String(power.isCharging() ? "YES" : "NO") + "\n";
    // info += "VbusIn: " + String(power.isVbusIn() ? "YES" : "NO") + "\n";
    // info += "VbusGood: " + String(power.isVbusGood() ? "YES" : "NO") + "\n";
    // if (power.isBatteryConnect()) {
    lv_label_set_text_fmt(ui_BattVolLabel, "Batt: %dmV %d%%", power.getBattVoltage(), power.getBatteryPercent());
    lv_label_set_text_fmt(ui_VBusVolLabel, "VBus: %dmV", power.getVbusVoltage());
    lv_label_set_text_fmt(ui_SysVolLabel, "Sys: %dmV", power.getSystemVoltage());
    sprintf(buffer, "Temperature: %.2f°C", power.getTemperature());
    lv_label_set_text_fmt(ui_TempLabel, buffer);
    uint8_t charge_status = power.getChargerStatus();
    switch (charge_status) {
        case XPOWERS_AXP2101_CHG_TRI_STATE:
            sprintf(buffer, "Charger Status: tri_charge");
            break;
        case XPOWERS_AXP2101_CHG_PRE_STATE:
            sprintf(buffer, "Charger Status: pre_charge");
            break;
        case XPOWERS_AXP2101_CHG_CC_STATE:
            sprintf(buffer, "Charger Status: constant charge");
            break;
        case XPOWERS_AXP2101_CHG_CV_STATE:
            sprintf(buffer, "Charger Status: constant voltage");
            break;
        case XPOWERS_AXP2101_CHG_DONE_STATE:
            sprintf(buffer, "Charger Status: charge done");
            break;
        case XPOWERS_AXP2101_CHG_STOP_STATE:
            sprintf(buffer, "Charger Status: not charging");
            break;
    }
    lv_label_set_text_fmt(ui_ChargingLabel, buffer);
}
