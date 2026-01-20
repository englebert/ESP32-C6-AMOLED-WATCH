#include <Arduino.h>
#include "imu.h"

SensorQMI8658 imu;

void init_imu(void) {
    // 1. Initialize the I2C connection
    // if (!imu.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
    while (!imu.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
        USBSerial.println("IMU: Failed to find QMI8658!");
        delay(10);
    }
    USBSerial.println("IMU: Initialized");

    // 2. Configure for normal usage (High performance)
    // Range: 4G, ODR: 1000Hz
    imu.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz);
    // Range: 64DPS, ODR: 896Hz
    imu.configGyroscope(SensorQMI8658::GYR_RANGE_64DPS, SensorQMI8658::GYR_ODR_896_8Hz);
    
    imu.enableGyroscope();
    imu.enableAccelerometer();
    
    // Ensure ESP32 pin is ready to read interrupts
    pinMode(IMU_INT1, INPUT_PULLUP);
}

void enable_imu_wakeup(void) {
    USBSerial.println("IMU: Configuring Wake-on-Motion...");

    // configWakeOnMotion automatically:
    // 1. Resets the sensor
    // 2. Sets Low Power Mode (128Hz)
    // 3. Sets the Threshold (approx 200mg)
    // 4. Configures the interrupt pin to go LOW when motion is detected
    
    // Params: Threshold (mg), ODR, IntPin, DefaultLevel (1=High), BlankingTime
    imu.configWakeOnMotion(
        125, // Threshold: 125mg (Lower = more sensitive)
        SensorQMI8658::ACC_ODR_LOWPOWER_128Hz, 
        QMI_INT_LOGIC,
        1    // 1 means the pin stays HIGH and drops LOW on interrupt
    );
}
