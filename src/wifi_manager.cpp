#include "wifi_manager.h"
#include "web_server.h"
#include "logging.h"
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include "esp_wifi.h"

// Blacklisted MAC addresses (temporary)
String blacklist[50];
int blacklist_count = 0;

void handleClient(WiFiServer &server, LiquidCrystal_I2C &lcd)
{
  WiFiClient client = server.available();
  if (client)
  {
    // Read entire HTTP request
    String fullRequest = "";
    String currentLine = "";

    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        fullRequest += c;
        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            break;
          }
          currentLine = "";
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }

    Serial.println("=== Full Request ===");
    Serial.println(fullRequest);
    Serial.println("===================");

    // Get MAC address
    wifi_sta_list_t wifi_sta_list;
    esp_wifi_ap_get_sta_list(&wifi_sta_list);
    String macAddress = "Unknown";
    for (int i = 0; i < wifi_sta_list.num; i++)
    {
      wifi_sta_info_t station = wifi_sta_list.sta[i];
      macAddress = "";
      for (int j = 0; j < 6; j++)
      {
        if (station.mac[j] < 16)
        {
          macAddress += "0";
        }
        macAddress += String(station.mac[j], HEX);
        if (j < 5)
        {
          macAddress += ":";
        }
      }
    }
    macAddress.toUpperCase();

    if (isWhitelisted(macAddress))
    {
      // Serve admin panel or other content for whitelisted users
      if (fullRequest.indexOf("GET / HTTP") != -1)
      {
        serveAdminPanel(client, lcd);
      }
      else if (fullRequest.indexOf("/clear") != -1)
      {
        clearLogs(client);
      }
      else if (fullRequest.indexOf("/export") != -1)
      {
        exportLogs(client);
      }
    }
    else
    {
      // Check if the MAC address is on the blacklist
      bool blocked = false;
      for (int i = 0; i < blacklist_count; i++)
      {
        if (blacklist[i] == macAddress)
        {
          blocked = true;
          break;
        }
      }

      if (blocked)
      {
        Serial.println("Blocked MAC address: " + macAddress);
        client.stop();
      }
      else
      {
        serveHoneypot(client, fullRequest, lcd);
        // Add to blacklist
        if (blacklist_count < 50)
        {
          blacklist[blacklist_count] = macAddress;
          blacklist_count++;
        }
      }
    }

    // Close the connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}
