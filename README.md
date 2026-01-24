# ESP32-C6 AMOLED Smartwatch Firmware

A custom, power-efficient smartwatch firmware built for the **Waveshare ESP32-C6 AMOLED** development board. This project integrates LVGL for a smooth UI, advanced power management (Light Sleep & CPU scaling), and sensor fusion for wake-on-motion capabilities.

## 🚀 Features

### 🎨 User Interface (LVGL)

* **Multiple Watchfaces:** Graphical, Analog, Simple, and Terminal styles.
* **Gesture Navigation:** Swipe Left/Right to change pages, Up/Down to switch watch faces.
* **Stats Dashboard:** Real-time battery voltage, IMU data, and system uptime.
* **Settings & WiFi:** Dedicated pages for on-device configuration.

### ⚡ Power Management

* **Smart Sleep:** Enters **Light Sleep** (saving state) after 30 seconds of inactivity.
* **Dynamic CPU Scaling:** drops to **80MHz** during sleep to save power, boosts to **160MHz** on wake-up for smooth UI.
* **Auto-Dimming:** Dims screen brightness to 10% after 10 seconds of inactivity; restores immediately on motion or touch.
* **Hardware Control:** Manages AXP2101 PMU to cut power to the screen and peripherals during sleep.

### 📡 Connectivity & Time

* **WiFi Manager:** Auto-connects to known networks stored in persistent storage (LittleFS).
* **Precision Time:** Syncs onboard **PCF85063 RTC** via **NTP** (Google/NIST) every hour when connected.
* **Wake Sources:**
* **Double Tap / Touch:** (FT3168 Touch Controller in Monitor Mode).
* **Lift-to-Wake:** (QMI8658 IMU interrupt on wrist movement).



## 🛠 Hardware Specifications

* **MCU:** ESP32-C6 (RISC-V 32-bit Single-core + LP Core)
* **Display:** 1.47" AMOLED (454x454) with **CO5300** driver (QSPI)
* **Touch:** **FT3168** Capacitive Touch
* **PMU:** **AXP2101** Power Management Unit
* **IMU:** **QMI8658** 6-Axis Accelerometer & Gyroscope
* **RTC:** **PCF85063** I2C Real-Time Clock

## 📦 Libraries & Dependencies

This project is built using **PlatformIO** / **Arduino IDE**. Ensure you have the following libraries installed:

* **LVGL** (Light and Versatile Graphics Library) - *v8.x or v9.x*
* **Arduino_GFX_Library** - *For CO5300 Display Driver*
* **XPowersLib** - *For AXP2101 PMU control*
* **SensorLib** (by Lewis He) - *For QMI8658 IMU and PCF85063 RTC*
* **LittleFS** - *For saving WiFi credentials*

## 🎮 Controls & Gestures

| Gesture | Action |
| --- | --- |
| **Swipe Left/Right** | Switch between Watchface, Stats, Settings, WiFi pages |
| **Swipe Up/Down** | Cycle through different Watchface designs (on Home screen) |
| **Cover/Idle (10s)** | Dims screen to 10% brightness |
| **Idle (30s)** | Enters Sleep Mode (Screen Off) |
| **Wrist Shake / Touch** | Wakes device from Sleep |

## 📂 File Structure Overview

* `src/main.cpp` - Entry point, initialization.
* `src/display.cpp` - LVGL & GFX setup, buffer management, brightness control.
* `src/power.cpp` - Sleep logic, AXP2101 PMU config, CPU frequency scaling.
* `src/processes.cpp` - Background tasks (Time sync, Auto-dim logic, WiFi monitoring).
* `src/rtc.cpp` - RTC time reading and NTP synchronization logic.
* `src/imu.cpp` - QMI8658 setup and "Wake-on-Motion" interrupt config.
* `src/wifi_manager.cpp` - Handles scanning, saving, and connecting to WiFi.

## ⚙️ Configuration Notes

### Adjusting Wake Sensitivity

If the watch wakes up too easily, adjust the threshold in `src/processes.cpp`:

```cpp
// Increase value (e.g., 0.5) for harder shake, decrease (0.1) for sensitive
if (delta > 0.5) { 
    awake_time = 0;  
}

```

### CPU Frequency

The system defaults to dynamic scaling to balance performance and battery:

* **Active:** 160MHz
* **Sleep:** 80MHz (Safe for USB logging) / 40MHz (Max savings, no USB)

---

**Author:** Englebert Lai
**License:** MIT
