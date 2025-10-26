#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>

extern IPAddress myIP;

void serveHoneypot(WiFiClient client, String fullRequest, LiquidCrystal_I2C &lcd);
void serveAdminPanel(WiFiClient client, LiquidCrystal_I2C &lcd);
bool isWhitelisted(String mac);

#endif
