#ifndef DISPLAY_H
#define DISPLAY_H

#include "main.h"

extern uint32_t screenWidth;
extern uint32_t screenHeight;
extern uint32_t bufSize;
extern lv_display_t *disp;
extern lv_color_t *disp_draw_buf;
extern Arduino_DataBus *bus;
extern Arduino_GFX *gfx;
extern bool display_status;

void init_display(void);
void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
void rounder_event_cb(lv_event_t *e);
void gesture_event_cb(lv_event_t *e);
void setBrightness(uint8_t percent);
uint8_t getBrightness(void);

#endif
