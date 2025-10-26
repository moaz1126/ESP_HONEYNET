#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SPIFFS.h"
#include "esp_wifi.h"

#include "config.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "logging.h"
#include "fingerprinting.h"

// Create a WiFiServer object
WiFiServer server(80);

// Initialize the LCD
// Define the I2C address of the LCD. Common addresses are 0x27 or 0x3F.
// If your LCD does not work, try changing this address.
const byte LCD_ADDR = 0x27;
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Honeypot Starting");

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Create the Access Point
  WiFi.softAP(ssid, password);

  myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Start the server
  server.begin();
  Serial.println("Server started");
  Serial.println("Honeypot is running!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Honeypot Active");
  lcd.setCursor(0, 1);
  lcd.print(myIP.toString());
}

void loop()
{
  handleClient(server, lcd);
}