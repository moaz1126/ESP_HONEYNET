#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>
#include <WiFiClient.h>

void logIntruderData(String time, String ip, String mac, int rssi, String request, String userAgent, String acceptLang, String primaryLang, String deviceType, String deviceBrand, String deviceModel, String osType, String osVersion, String browser, String browserVersion, String manufacturer, String screenRes, String host, String referer, String dnt, String connection, String accept, unsigned long joinDate);
String readLogs();
int countLogs(String text);
void displayLogs(WiFiClient &client, String logs);
String getValue(String data, String key);
String sanitizeCSV(String input);
void clearLogs(WiFiClient client);
void exportLogs(WiFiClient client);

#endif
