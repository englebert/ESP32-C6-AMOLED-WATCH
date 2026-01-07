#include <Arduino.h>
#include "display.h"

uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_display_t *disp;
lv_color_t *disp_draw_buf;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS,
    LCD_SCLK,
    LCD_SDIO0,
    LCD_SDIO1,
    LCD_SDIO2,
    LCD_SDIO3
);

Arduino_GFX *gfx = new Arduino_CO5300(
    bus,
    LCD_RESET,
    0,  // Rotation
    LCD_WIDTH,
    LCD_HEIGHT,
    22, // Column Offset 1
    0,  // Row Offset 1
    0,  // Column Offset 2
    0   // Row Offset 2
);

void init_display(void) {
    // Initialize Display
    if(!gfx->begin()) {
        USBSerial.println("GFX failed.");
    }
    gfx->fillScreen(RGB565_BLACK);

    lv_init();

    // Set a tick source so that LVGL will know how much time elapsed.
    lv_tick_set_cb(millis);

    screenWidth   = gfx->width();
    screenHeight  = gfx->height();
    bufSize       = screenWidth * 50;
    disp_draw_buf = (lv_color_t*)malloc(bufSize * 2);

    if(!disp_draw_buf) {
        USBSerial.println("LVGL buffer error");
    } else {
        disp = lv_display_create(screenWidth, screenHeight);
        lv_display_set_flush_cb(disp, disp_flush);
        lv_display_set_buffers(
            disp,
            disp_draw_buf,
            NULL,
            bufSize * 2,
            LV_DISPLAY_RENDER_MODE_PARTIAL
        );
    }

    // Setting up touch
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchpad_read);
    lv_display_add_event_cb(disp, rounder_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);
}

// Event callback to handle swipes
void gesture_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
            case LV_DIR_LEFT:
                USBSerial.println("Gesture: Swipe Left");
                break;
            case LV_DIR_RIGHT:
                USBSerial.println("Gesture: Swipe Right");
                break;
            case LV_DIR_TOP:
                USBSerial.println("Gesture: Swipe Up");
                break;
            case LV_DIR_BOTTOM:
                USBSerial.println("Gesture: Swipe Down");
                break;
        }
    }
}

void rounder_event_cb(lv_event_t *e) {
    lv_area_t *area = (lv_area_t *)lv_event_get_param(e);
    uint16_t x1 = area->x1;
    uint16_t x2 = area->x2;

    uint16_t y1 = area->y1;
    uint16_t y2 = area->y2;

    // round the start of coordinate down to the nearest 2M number
    area->x1 = (x1 >> 1) << 1;
    area->y1 = (y1 >> 1) << 1;

    // round the end of coordinate up to the nearest 2N+1 number
    area->x2 = ((x2 >> 1) << 1) + 1;
    area->y2 = ((y2 >> 1) << 1) + 1;
}

void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
    lv_disp_flush_ready(disp);
}

