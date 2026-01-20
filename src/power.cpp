#include <Arduino.h>
#include "power.h"

XPowersPMU power;
uint16_t sleep_count = 0;

static uint8_t interrupt_count = 0;

void init_power(void) {
    power.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    power.setChargeTargetVoltage(3);
    // Clear all interrupt flags
    power.clearIrqStatus();
    // Enable the required interrupt function
    power.enableIRQ(
        XPOWERS_AXP2101_PKEY_SHORT_IRQ  //POWER KEY
    );

    // Ensure AMOLED power is ON and STABLE
    // power.setBLDO1Voltage(3000); // Set to 3.3V (or 3000 depending on board spec)
    // power.enableBLDO1();

    adcOn();
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

void modeSleep(void) {
    USBSerial.println("Going to Sleep...");
    display_status = false;

    if(gfx) 
        gfx->displayOff();

    disable_wifi();

    // Turn off backlight
    power.disableBLDO1();

    if(FT3168) {
        FT3168->IIC_Write_Device_State(
            Arduino_IIC_Touch::Device::TOUCH_POWER_MODE, 
            Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR
        );
    }
    
    // Prepare IMU for Sleep
    enable_imu_wakeup();

    // Light sleep uses 'gpio_wakeup_enable', NOT 'esp_deep_sleep...'
    // gpio_wakeup_enable((gpio_num_t)TP_INT, GPIO_INTR_LOW_LEVEL);

    // Wake on IMU Motion (IO connected to IMU_INT)
    // configured the IMU to drop LOW on movement
    gpio_wakeup_enable((gpio_num_t)IMU_INT1, GPIO_INTR_LOW_LEVEL);

    // Enable GPIO wake-up feature for Light Sleep
    esp_sleep_enable_gpio_wakeup();
    time_t last_wake_time = rtc.hwClockRead();

    while(interrupt_count < 20) {
        // Enter Light Sleep
        USBSerial.println("Goodnight.");
    
        esp_light_sleep_start();
        time_t now = rtc.hwClockRead();
        if (now != -1 && last_wake_time != -1) {
            time_t diff = now - last_wake_time;
            // User Requirement: If > 1 second passed, reset counter
            if (diff > 1) {
                USBSerial.printf("Long Sleep (%ld sec). Resetting Count.\n", (long)diff);
                interrupt_count = 0;
            }
        }

        last_wake_time = now;
        interrupt_count++;
    }

    interrupt_count = 0;

    USBSerial.println("Woke up!");
    awake_time = 0;
    display_status = true;


    init_touch();

    // IMPORTANT: Restore IMU to normal mode (Gyro/Accel active)
    // configWakeOnMotion disabled the Gyro and put Accel in low power.
    // This need to re-init to use it for other things (like games or pedometer).
    init_imu();

    if(gfx)
        gfx->displayOn();

    // Turn on backlight
    power.enableBLDO1();
    sleep_count++;
}
