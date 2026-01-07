#ifndef TOUCH_H
#define TOUCH_H

#include "main.h"

extern std::unique_ptr<Arduino_IIC> FT3168;

void init_touch(void);
void Arduino_IIC_Touch_Interrupt(void);
void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data);

#endif
