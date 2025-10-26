#ifndef FINGERPRINTING_H
#define FINGERPRINTING_H

#include <Arduino.h>

String extractHeader(String request, String headerName);
String determineDeviceType(String userAgent);
String determineDeviceBrand(String userAgent);
String determineDeviceModel(String userAgent);
String determineOS(String userAgent);
String determineOSVersion(String userAgent);
String determineBrowser(String userAgent);
String determineBrowserVersion(String userAgent);
String determineManufacturer(String macAddress);
String extractScreenResolution(String userAgent);

#endif
