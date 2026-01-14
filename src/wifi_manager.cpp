#include "Arduino.h"
#include "wifi_manager.h"

#define WIFI_DB "/wifistore.db"

static bool _was_connected = false;

void init_wifi_manager() {
    // 1. Mount File System
    if (!LittleFS.begin(true)) {
        USBSerial.println("LittleFS Mount Failed");
        return;
    }
    
    // 2. Set WiFi to Station Mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    // 3. Try to auto connect on boot
    auto_connect_wifi();
}

void save_wifi_cred(String ssid, String pass) {
    // Check if already exists to avoid duplicates (Optional optimization)
    // For now, we just append.
    File file = LittleFS.open(WIFI_DB, "a");
    if (!file) {
        USBSerial.println("Failed to open DB for appending");
        return;
    }
    file.println(ssid);
    file.println(pass);
    file.close();
    USBSerial.println("Saved WiFi: " + ssid);
}

std::vector<WifiCred> load_wifi_creds() {
    std::vector<WifiCred> creds;
    
    if (!LittleFS.exists(WIFI_DB)) return creds;

    File file = LittleFS.open(WIFI_DB, "r");
    if (!file) return creds;

    while (file.available()) {
        String s = file.readStringUntil('\n');
        String p = file.readStringUntil('\n');
        s.trim();
        p.trim();
        if (s.length() > 0) {
            creds.push_back({s, p});
        }
    }
    file.close();
    return creds;
}

void auto_connect_wifi() {
    std::vector<WifiCred> stored = load_wifi_creds();
    if (stored.empty()) {
        USBSerial.println("No WiFi credentials stored.");
        return;
    }

    USBSerial.println("Scanning for known networks...");
    int n = WiFi.scanNetworks();
    
    for (int i = 0; i < n; ++i) {
        String scannedSSID = WiFi.SSID(i);
        for (auto &cred : stored) {
            if (scannedSSID == cred.ssid) {
                USBSerial.printf("Found known network: %s. Connecting...\n", cred.ssid.c_str());
                WiFi.begin(cred.ssid.c_str(), cred.pass.c_str());
                return; // Attempting to connect, exit function
            }
        }
    }
    USBSerial.println("No known networks found nearby.");
}

bool is_wifi_connected() {
    return WiFi.status() == WL_CONNECTED;
}

String get_wifi_ip() {
    return WiFi.localIP().toString();
}

void monitor_wifi() {
    bool is_connected = (WiFi.status() == WL_CONNECTED);

    // Detect Rising Edge (Disconnected -> Connected)
    if (is_connected && !_was_connected) {
        USBSerial.println("WiFi: Connected! Triggering NTP Sync...");
        
        // Give it a moment to stabilize network stack
        delay(500); 
        
        // Trigger NTP
        syncNTP();
    }

    _was_connected = is_connected;
}
