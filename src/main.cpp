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

/***
std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus = std::make_shared<Arduino_HWIIC>(
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


void setup(void) {
    USBSerial.begin(115200);
    USBSerial.println("[My Watch 1.0]");
    String LVGL_Arduino =
        String('V') + 
        lv_version_major() + "." + 
        lv_version_minor() + "." + 
        lv_version_patch();
    USBSerial.println(LVGL_Arduino);

    if(!gfx->begin()) {
        USBSerial.println("GFX failed.");
    }

    gfx->fillScreen(RGB565_BLACK);
    // Wire.begin(IIC_SDA, IIC_SCL);

    // while(FT3168->begin() == false) {
    //     USBSerial.println("FT3168 Failed.");
    //     delay(2000);
    // }

    // Creating tasks
    xTaskCreate(
        backgroundUpdate,
        "Background update",
        4096,
        NULL,
        1,
        NULL
    );
}

void loop() {
}
