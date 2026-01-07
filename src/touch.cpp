#include <Arduino.h>
#include "touch.h"

std::unique_ptr<Arduino_IIC> FT3168;

void init_touch(void) {
    FT3168.reset(new Arduino_FT3x68(
        IIC_Bus,
        FT3168_DEVICE_ADDRESS,
        DRIVEBUS_DEFAULT_VALUE,
        TP_INT,
        Arduino_IIC_Touch_Interrupt
    ));

    // Touchpad: Start to initialize FT3168
    while(FT3168->begin() == false) {
        USBSerial.println("FT3168 Failed.");
        delay(2000);
    }
    USBSerial.println("FT3168 OK");


    FT3168->IIC_Write_Device_State(
        FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
        FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR
    );
}

void Arduino_IIC_Touch_Interrupt(void) {
    if(FT3168) {
        FT3168->IIC_Interrupt_Flag = true;
    }
}

void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    // Safety check
    if (!FT3168) return;

    int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
    int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);

    if(FT3168->IIC_Interrupt_Flag == true) {
        FT3168->IIC_Interrupt_Flag = false;

        data->state = LV_INDEV_STATE_PR;

        data->point.x = touch_x;
        data->point.y = touch_y;

        USBSerial.printf("Touch: %d, %d\n", touch_x, touch_y);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

