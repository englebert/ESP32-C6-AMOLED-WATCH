#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include "main.h"
#include <vector>

struct WifiCred {
    String ssid;
    String pass;
};

// Initialize File System
void init_wifi_manager();

// Save a new network (appends to file)
void save_wifi_cred(String ssid, String pass);

// Read all stored networks
std::vector<WifiCred> load_wifi_creds();

// Automatically scan and connect to a known network
void auto_connect_wifi();

// Check if currently connected
bool is_wifi_connected();
String get_wifi_ip();
void monitor_wifi();

#endif
