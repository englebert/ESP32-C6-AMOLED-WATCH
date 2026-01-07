#include <Arduino.h>
#include "main.h"


HWCDC USBSerial;

uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_display_t *disp;
lv_color_t *disp_draw_buf;

XPowersPMU power;
lv_obj_t *info_label;

lv_obj_t *label;  // Global label object
SensorPCF85063 rtc;
uint32_t lastMillis;

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

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(
        IIC_SDA,
        IIC_SCL,
        &Wire
);

std::unique_ptr<Arduino_IIC> FT3168(
    new Arduino_FT3x68(
        IIC_Bus,
        FT3168_DEVICE_ADDRESS,
        DRIVEBUS_DEFAULT_VALUE,
        TP_INT,
        Arduino_IIC_Touch_Interrupt
    )
);


void Arduino_IIC_Touch_Interrupt(void) {
    FT3168->IIC_Interrupt_Flag = true;
}

void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data);
void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    int32_t touch_x = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
    int32_t touch_y = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);

    if(FT3168->IIC_Interrupt_Flag == true) {
        FT3168->IIC_Interrupt_Flag = false;

        data->state = LV_INDEV_STATE_PR;

        data->point.x = touch_x;
        data->point.y = touch_y;

        USBSerial.print("X: "); USBSerial.print(touch_x);
        USBSerial.print("Y: "); USBSerial.println(touch_y);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

/**
void adcOn(void) {
    power.enableTemperatureMeasure();
    // Enable internal ADC detection
    power.enableBattDetection();
    power.enableVbusVoltageMeasure();
    power.enableBattVoltageMeasure();
    power.enableSystemVoltageMeasure();
}

void adcOff(void) {
    power.disableTemperatureMeasure();
    // Enable internal ADC detection
    power.disableBattDetection();
    power.disableVbusVoltageMeasure();
    power.disableBattVoltageMeasure();
    power.disableSystemVoltageMeasure();
}


 **/

// Global Sprite Object (reuse one to save RAM)
// Arduino_Canvas *canvas = new Arduino_Canvas(
//     410 /* width */,
//     200 /* height */,
//    gfx)
// ;
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

void setup(void) {
    USBSerial.begin(115200);
    delay(50);

    USBSerial.println("[My Watch 1.0]");
    String LVGL_Arduino =
        String('V') + 
        lv_version_major() + "." + 
        lv_version_minor() + "." + 
        lv_version_patch();
    USBSerial.println(LVGL_Arduino);

    // Initialize Display
    if(!gfx->begin()) {
        USBSerial.println("GFX failed.");
    }
    gfx->fillScreen(RGB565_BLACK);

    Wire.begin(IIC_SDA, IIC_SCL);

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

    // RTC Initialization
    if(!rtc.begin(Wire, IIC_SDA, IIC_SCL)) {
        USBSerial.println("Failed to find PCF8563 - check your wiring!");

        // RTC Error. Hang here.
        while(true) {
            delay(1000);
        }
    }

    // Display initialization
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
    

    // BLACK screen
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);

    // Creating tasks
    xTaskCreate(
        backgroundUpdate,
        "Background update",
        4096,
        NULL,
        1,
        NULL
    );

    USBSerial.println("Setup - OK");
}

void loop() {
}
