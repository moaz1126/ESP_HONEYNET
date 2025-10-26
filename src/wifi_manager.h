#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

void handleClient(WiFiServer &server, LiquidCrystal_I2C &lcd);

#endif
