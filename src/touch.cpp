#include <Arduino.h>
#include "touch.h"

std::unique_ptr<Arduino_IIC> FT3168;

// Keep track of the last known coordinates to prevent jumps
static int32_t last_x = 0;
static int32_t last_y = 0;

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
        delay(10);
    }
    USBSerial.println("FT3168 OK");

    /**
    FT3168->IIC_Write_Device_State(
        FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
        FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR
    );
     **/
    FT3168->IIC_Write_Device_State(
        Arduino_IIC_Touch::Device::TOUCH_GESTUREID_MODE,
        Arduino_IIC_Touch::Device_State::TOUCH_DEVICE_ON
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


    /***
    if(FT3168->IIC_Interrupt_Flag == true) {
        FT3168->IIC_Interrupt_Flag = false;

        String gesture = FT3168->IIC_Read_Device_State(Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID);
        int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
        int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
        int touch_fingers = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

        if (gesture != "No Gesture") {
            USBSerial.print("Gesture Detected: ");
            USBSerial.println(gesture);

            if (gesture == "Swipe Left") {
                // Action: Next Page?
            } else if (gesture == "Swipe Right") {
                // Action: Previous Page?
            } else if (gesture == "Double Click") {
                USBSerial.println("ACTION: Double Click Triggered!");
            }
        }

        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_x;
        data->point.y = touch_y;

        USBSerial.printf("[Touch] x:%d, y:%d finger(s):%d - %s\n", touch_x, touch_y, touch_fingers, gesture);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
     ***/

    // Check if the interrupt triggered OR if the pin is still low (touch held)
    // (Assuming TP_INT is defined in pin_config.h)
    bool touched = (FT3168->IIC_Interrupt_Flag) || (digitalRead(TP_INT) == LOW);

    if (touched) {
        FT3168->IIC_Interrupt_Flag = false;

        // 1. Check how many fingers are present
        int fingers = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

        if (fingers > 0) {
            // 2. Read Coordinates
            int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            
            // --- FIX: READ Y-AXIS CORRECTLY (Was reading X before) ---
            int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            
            // Update last known position
            last_x = touch_x;
            last_y = touch_y;

            data->state = LV_INDEV_STATE_PR;
            data->point.x = touch_x;
            data->point.y = touch_y;
            
            // Optional debug to see the smooth stream
            USBSerial.printf("[Touch] x: %d, y: %d, fingers: %d\n", touch_x, touch_y, fingers);
        } else {
            // Register says 0 fingers, so we are released
            data->state = LV_INDEV_STATE_REL;
        }
    } else {
        // No interrupt = No activity
        data->state = LV_INDEV_STATE_REL;
    }
}

