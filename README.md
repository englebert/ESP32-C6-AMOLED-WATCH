Based on the code structure and components we have discussed, here is a comprehensive `README.md` for your Git repository.

---

# ESP32-S3 Smartwatch Firmware

A custom, high-performance smartwatch firmware built for ESP32-S3 based wearables. This project utilizes the **Arduino Framework** and integrates **LVGL** for a smooth, gesture-based user interface. It features advanced power management, motion detection (lift-to-wake), and multiple graphical watch faces.

## 📷 Features

* **GUI:** Powered by **LVGL (Light and Versatile Graphics Library)**.
* **Display:** Supports **410x502** high-resolution AMOLED/LCD screens (CO5300 Driver).
* **Input:** Capacitive Touch (FT3168) with gesture support (Swipe Left/Right/Up/Down).
* **Sensors & Peripherals:**
* **6-Axis IMU (QMI8658):** Step counting and **Wake-on-Motion** (Lift-to-wake).
* **RTC (PCF85063):** Accurate timekeeping with automatic system time sync.
* **PMU (AXP2101):** Battery voltage monitoring, charging status, and power rail management.


* **Power Management:**
* Deep/Light Sleep integration.
* Wake sources: Touch, Motion (IMU), and Power Button.
* Auto-screen timeout.



## ⌚ Watch Faces

The system currently includes multiple distinct watch faces:

1. **Graphical:** Digital time, date, and battery status with dynamic coloring.
2. **Analog:** Smoothly sweeping second hand, minute, and hour hands on a 410px circular face.
3. **Terminal:** A "Hacker" style interface that simulates a Linux boot sequence to display system stats (Uptime, I2C scan, Battery mV, Date/Time).
4. **Stats:** (Placeholder) Designed for step counting and health metrics.

## 🛠 Hardware Specifications

This firmware is configured for a board with the following specs:

| Component | Model | Interface |
| --- | --- | --- |
| **MCU** | ESP32-S3 | - |
| **Display** | 410x502 AMOLED/LCD | QSPI / CO5300 |
| **Touch** | FT3168 | I2C |
| **IMU** | QMI8658 | I2C |
| **RTC** | PCF85063 | I2C |
| **PMU** | XPowers AXP2101 | I2C |

## 📂 Project Structure

```text
├── src/
│   ├── main.cpp                 # Setup and Main Loop
│   ├── main.h                   # Global definitions and includes
│   ├── pin_config.h             # GPIO Pin Definitions
│   ├── processes.cpp/.h         # FreeRTOS Tasks (Background Update, Time Sync)
│   ├── display.cpp/.h           # Display Driver (Arduino_GFX) & LVGL Init
│   ├── touch.cpp/.h             # Touch Driver (FT3168) & Input Handling
│   ├── imu.cpp/.h               # QMI8658 Driver & Wake-on-Motion Logic
│   ├── power.cpp/.h             # AXP2101 PMU & Sleep Logic
│   ├── rtc.cpp/.h               # PCF85063 RTC Driver & Time Sync
│   ├── watchface_graphical.cpp  # Digital Watch Face
│   ├── watchface_analog.cpp     # Analog Watch Face
│   ├── watchface_terminal.cpp   # System Info Watch Face
│   └── ...
├── platformio.ini               # Build configuration
└── README.md

```

## 🚀 Getting Started

### Dependencies

Ensure the following libraries are installed (via PlatformIO `lib_deps` or Arduino Library Manager):

* `lvgl`
* `Arduino_GFX_Library`
* `XPowersLib` (for AXP2101)
* `SensorPCF85063` (Lewis He)
* `SensorQMI8658` (Lewis He)
* `Wire`
* `WiFi`

### Installation

1. **Clone the Repo:**
```bash
git clone https://github.com/your-username/esp32-smartwatch.git

```


2. **Open in PlatformIO:**
Open the project folder in VS Code with the PlatformIO extension installed.
3. **Build & Upload:**
Connect your ESP32-S3 watch via USB and click **Upload**.

## ⚙️ Configuration

### Pin Mapping

Edit `src/pin_config.h` to match your specific board layout. Critical pins include:

* `IMU_INT1` / `IMU_INT2`: For motion wakeup.
* `TP_INT`: For touch wakeup.
* `LCD_CS`, `LCD_SCLK`, `LCD_SDIOx`: For display driving.

### Sensitivity Tuning

To adjust the Lift-to-Wake sensitivity, edit `src/imu.cpp`:

```cpp
imu.configWakeOnMotion(
    150, // Threshold (1-255). Lower = More Sensitive.
    SensorQMI8658::ACC_ODR_LOWPOWER_128Hz,
    QMI_INT_LOGIC,
    1
);

```

## 🔋 Power Consumption

The device enters **Light Sleep** automatically after 30 seconds of inactivity.

* **Active Mode:** ~80-120mA (Display ON, WiFi OFF)
* **Sleep Mode:** < 2mA (Display OFF, Touch & IMU monitoring active)

## 🤝 Contributing

Pull requests are welcome! If you create a new cool watch face, feel free to submit it.

## 📄 License

This project is open source. [MIT License](https://www.google.com/search?q=LICENSE).
