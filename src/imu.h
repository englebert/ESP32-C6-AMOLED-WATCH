#ifndef IMU_H
#define IMU_H

#include "main.h"

// IMU interrupt pin
#define IMU_INT1 16
#define IMU_INT2 17

#define QMI_INT_LOGIC SensorQMI8658::INTERRUPT_PIN_1

// Expose the IMU object so we can read data in other files if needed
extern SensorQMI8658 imu;

void init_imu(void);
void enable_imu_wakeup(void);

#endif
