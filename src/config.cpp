#include "config.h"

// Set the SSID and password for the honeypot network
const char *ssid = "IoT_Device_123";
const char *password = "12345678";

// Whitelisted MAC addresses
const char *whitelist[] = {"50:DA:D6:06:4B:E6"};
const int whitelist_size = sizeof(whitelist) / sizeof(whitelist[0]);