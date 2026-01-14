Based on the transition to the **ESP32-C6** and the new features we implemented (Terminal Watchface, Wake-on-Motion), here is the updated `README.md`.

---

# ESP32-C6 AMOLED Smartwatch Firmware

A high-performance smartwatch firmware optimized for the **ESP32-C6 (RISC-V)** microcontroller. This project drives a large **1.8" 410x502 AMOLED** display using **QSPI** and features a smooth, gesture-based UI built with **LVGL**.

## ⚡ Key Features

* **Core:** ESP32-C6 (RISC-V 32-bit RISC-V CPU) with WiFi 6 & BLE 5.3.
* **Display:** 410x502 AMOLED (CO5300 Controller) running in **QSPI Mode** for high frame rates.
* **User Interface:**
* **LVGL 9** integration for smooth animations.
* **Gestures:** Swipe Left/Right to change pages, Up/Down to switch watch faces.


* **Power Management:**
* **Deep/Light Sleep:** < 2mA standby current.
* **Wake Sources:** Touchscreen (FT3168), Power Button, and **Motion (Lift-to-Wake)**.


* **System Tools:**
* **Terminal Watchface:** Simulates a Linux shell to display real-time I2C scanning, battery voltage (mV), and system uptime.
* **Time Sync:** Automatic synchronization between internal ESP32 timer and external RTC (PCF85063).



## ⌚ Watch Faces

1. **Analog (Premium):** Features smooth sweeping hands (using milliseconds), dynamic geometry for 410px width, and battery status.
2. **Graphical (Digital):** Modern layout with large fonts and dynamic color changing based on battery level.
3. **Terminal (SysInfo):** A developer-focused face that "types" out system commands to show:
* Real-time Battery Voltage & Percentage.
* Live I2C Bus Scan (detects connected peripherals).
* Total System Uptime.
* Current Date/Time.



## 🛠 Hardware Specifications

This firmware is configured for **Waveshare ESP32-C6-Touch-AMOLED-1.8** (or compatible boards).

| Component | Model | Interface | Notes |
| --- | --- | --- | --- |
| **MCU** | ESP32-C6 | - | 160MHz RISC-V |
| **Display** | 1.8" AMOLED | **QSPI** | 410x502 Resolution, CO5300 Driver |
| **Touch** | FT3168 | I2C | Address `0x38` |
| **IMU** | QMI8658 | I2C | 6-Axis, Address `0x6B` |
| **RTC** | PCF85063 | I2C | Precision Timekeeping |
| **PMU** | AXP2101 | I2C | Power Management Unit |

## 🔌 Pin Configuration (QSPI)

The display uses the ESP32-C6's specialized SPI peripheral in Quad Mode.

```cpp
// src/pin_config.h

// Display (QSPI)
#define LCD_CS    5
#define LCD_SCLK  0
#define LCD_SDIO0 1  // Data 0
#define LCD_SDIO1 2  // Data 1
#define LCD_SDIO2 3  // Data 2
#define LCD_SDIO3 4  // Data 3
#define LCD_RESET 11

// Touch (I2C)
#define TP_SDA    6
#define TP_SCL    7
#define TP_INT    21
#define TP_RST    22

// IMU (Interrupt)
#define IMU_INT1  20 // Motion Wakeup Pin

```

## 📂 Project Structure

```text
├── src/
│   ├── main.cpp                 # Boot logic & Interrupt setup
│   ├── display.cpp              # QSPI Display Driver & LVGL Init
│   ├── imu.cpp                  # QMI8658 Config & Lift-to-Wake Logic
│   ├── power.cpp                # AXP2101 & Sleep Mode Handler
│   ├── rtc.cpp                  # Time Synchronization
│   ├── watchface_terminal.cpp   # "Hacker" Style System Info Face
│   ├── watchface_analog.cpp     # High-Res Analog Clock
│   ├── watchface_graphical.cpp  # Digital Clock
│   └── processes.cpp            # FreeRTOS Tasks (UI Update Loop)
├── platformio.ini               # Compiler Flags for ESP32-C6
└── README.md

```

## 🚀 Getting Started

### 1. Requirements

* **VS Code** with **PlatformIO**.
* **Libraries** (Managed via `platformio.ini`):
* `lvgl/lvgl`
* `moononournation/GFX Library for Arduino`
* `lewisxhe/XPowersLib`
* `lewisxhe/SensorLib`



### 2. Build Flags

Ensure your `platformio.ini` targets the C6 and enables USB CDC for debugging:

```ini
[env:esp32-c6-amoled]
platform = espressif32
board = esp32-c6-devkitc-1
framework = arduino
build_flags = 
    -D ARDUINO_USB_CDC_ON_BOOT=1 
    -D ARDUINO_USB_MODE=1

```

### 3. Sensitivity Tuning

To adjust how easily the watch wakes up when you raise your wrist, edit `src/imu.cpp`:

```cpp
// Value: 1 (Most Sensitive) to 255 (Least Sensitive)
// Recommended: 150
imu.configWakeOnMotion(150, ...);

```

## 📄 License

MIT License. Feel free to use and modify for your own wearable projects.
