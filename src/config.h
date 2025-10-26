#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Set the SSID and password for the honeypot network
extern const char *ssid;
extern const char *password;

// Whitelisted MAC addresses
extern const char *whitelist[];
extern const int whitelist_size;

#endif
