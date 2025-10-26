#include "fingerprinting.h"
#include <Arduino.h>

String extractHeader(String request, String headerName)
{
  int startPos = request.indexOf(headerName + ": ");
  if (startPos == -1)
    return "N/A";

  startPos += headerName.length() + 2;
  int endPos = request.indexOf('\n', startPos);
  String value = request.substring(startPos, endPos);
  value.trim();
  return value;
}

String determineDeviceType(String userAgent)
{
  userAgent.toLowerCase();
  if (userAgent.indexOf("mobile") != -1 || userAgent.indexOf("iphone") != -1)
  {
    return "Mobile";
  }
  else if (userAgent.indexOf("tablet") != -1 || userAgent.indexOf("ipad") != -1)
  {
    return "Tablet";
  }
  else if (userAgent.indexOf("smart-tv") != -1 || userAgent.indexOf("smarttv") != -1 || userAgent.indexOf("roku") != -1)
  {
    return "Smart TV";
  }
  else if (userAgent.indexOf("bot") != -1 || userAgent.indexOf("crawler") != -1 || userAgent.indexOf("spider") != -1)
  {
    return "Bot";
  }
  else if (userAgent.indexOf("android") != -1 && userAgent.indexOf("mobile") == -1)
  {
    return "Tablet";
  }
  return "Desktop";
}

String determineDeviceBrand(String userAgent)
{
  // Apple devices
  if (userAgent.indexOf("iPhone") != -1)
    return "Apple";
  if (userAgent.indexOf("iPad") != -1)
    return "Apple";
  if (userAgent.indexOf("Macintosh") != -1)
    return "Apple";

  // Samsung
  if (userAgent.indexOf("SM-") != -1 || userAgent.indexOf("Samsung") != -1 || userAgent.indexOf("SAMSUNG") != -1)
    return "Samsung";

  // Google
  if (userAgent.indexOf("Pixel") != -1 || userAgent.indexOf("Nexus") != -1)
    return "Google";

  // Huawei
  if (userAgent.indexOf("Huawei") != -1 || userAgent.indexOf("HUAWEI") != -1 || userAgent.indexOf("HW-") != -1)
    return "Huawei";

  // Xiaomi
  if (userAgent.indexOf("Xiaomi") != -1 || userAgent.indexOf("Mi ") != -1 || userAgent.indexOf("Redmi") != -1)
    return "Xiaomi";

  // OnePlus
  if (userAgent.indexOf("OnePlus") != -1 || userAgent.indexOf("ONEPLUS") != -1)
    return "OnePlus";

  // Oppo
  if (userAgent.indexOf("OPPO") != -1 || userAgent.indexOf("CPH") != -1)
    return "Oppo";

  // Vivo
  if (userAgent.indexOf("vivo") != -1 || userAgent.indexOf("V") != -1)
    return "Vivo";

  // LG
  if (userAgent.indexOf("LG-") != -1 || userAgent.indexOf("LG/") != -1)
    return "LG";

  // Motorola
  if (userAgent.indexOf("Moto") != -1 || userAgent.indexOf("XT") != -1)
    return "Motorola";

  // Sony
  if (userAgent.indexOf("Sony") != -1)
    return "Sony";

  // Windows
  if (userAgent.indexOf("Windows") != -1)
    return "Microsoft";

  return "Unknown";
}

String determineDeviceModel(String userAgent)
{
  // iPhone models
  if (userAgent.indexOf("iPhone") != -1)
  {
    if (userAgent.indexOf("iPhone14") != -1)
      return "iPhone 14";
    if (userAgent.indexOf("iPhone13") != -1)
      return "iPhone 13";
    if (userAgent.indexOf("iPhone12") != -1)
      return "iPhone 12";
    if (userAgent.indexOf("iPhone11") != -1)
      return "iPhone 11";
    if (userAgent.indexOf("iPhone X") != -1)
      return "iPhone X";
    return "iPhone";
  }

  // iPad models
  if (userAgent.indexOf("iPad") != -1)
  {
    if (userAgent.indexOf("iPad Pro") != -1)
      return "iPad Pro";
    if (userAgent.indexOf("iPad Air") != -1)
      return "iPad Air";
    if (userAgent.indexOf("iPad Mini") != -1)
      return "iPad Mini";
    return "iPad";
  }

  // Samsung models (SM-XXXX format)
  int smPos = userAgent.indexOf("SM-");
  if (smPos != -1)
  {
    int endPos = userAgent.indexOf(" ", smPos);
    if (endPos == -1)
      endPos = userAgent.indexOf(";", smPos);
    if (endPos == -1)
      endPos = userAgent.indexOf(")", smPos);
    if (endPos > smPos)
    {
      return userAgent.substring(smPos, endPos);
    }
  }

  // Google Pixel
  if (userAgent.indexOf("Pixel") != -1)
  {
    int pixelPos = userAgent.indexOf("Pixel");
    int endPos = userAgent.indexOf(";", pixelPos);
    if (endPos > pixelPos)
    {
      String model = userAgent.substring(pixelPos, endPos);
      model.trim();
      return model;
    }
  }

  return "N/A";
}

String determineOS(String userAgent)
{
  if (userAgent.indexOf("Windows NT 10") != -1)
    return "Windows 10/11";
  if (userAgent.indexOf("Windows NT 6.3") != -1)
    return "Windows 8.1";
  if (userAgent.indexOf("Windows NT 6.2") != -1)
    return "Windows 8";
  if (userAgent.indexOf("Windows NT 6.1") != -1)
    return "Windows 7";
  if (userAgent.indexOf("Mac OS X") != -1)
    return "macOS";
  if (userAgent.indexOf("Android") != -1)
    return "Android";
  if (userAgent.indexOf("iPhone") != -1 || userAgent.indexOf("iPad") != -1)
    return "iOS";
  if (userAgent.indexOf("Linux") != -1)
    return "Linux";
  if (userAgent.indexOf("Ubuntu") != -1)
    return "Ubuntu";
  return "Unknown";
}

String determineOSVersion(String userAgent)
{
  // Android version
  if (userAgent.indexOf("Android") != -1)
  {
    int start = userAgent.indexOf("Android") + 8;
    int end = userAgent.indexOf(";", start);
    if (end > start)
    {
      String version = userAgent.substring(start, end);
      version.trim();
      return version;
    }
  }

  // iOS version
  if (userAgent.indexOf("OS ") != -1 && (userAgent.indexOf("iPhone") != -1 || userAgent.indexOf("iPad") != -1))
  {
    int start = userAgent.indexOf("OS ") + 3;
    int end = userAgent.indexOf(" ", start);
    if (end > start)
    {
      String version = userAgent.substring(start, end);
      version.replace("_", ".");
      return version;
    }
  }

  return "N/A";
}

String determineBrowser(String userAgent)
{
  if (userAgent.indexOf("Edg/") != -1)
    return "Edge";
  if (userAgent.indexOf("Chrome/") != -1 && userAgent.indexOf("Edg/") == -1)
    return "Chrome";
  if (userAgent.indexOf("Firefox/") != -1)
    return "Firefox";
  if (userAgent.indexOf("Safari/") != -1 && userAgent.indexOf("Chrome") == -1)
    return "Safari";
  if (userAgent.indexOf("Opera") != -1 || userAgent.indexOf("OPR/") != -1)
    return "Opera";
  if (userAgent.indexOf("MSIE") != -1 || userAgent.indexOf("Trident/") != -1)
    return "IE";
  return "Unknown";
}

String determineBrowserVersion(String userAgent)
{
  if (userAgent.indexOf("Chrome/") != -1)
  {
    int start = userAgent.indexOf("Chrome/") + 7;
    int end = userAgent.indexOf(" ", start);
    return userAgent.substring(start, end);
  }
  else if (userAgent.indexOf("Firefox/") != -1)
  {
    int start = userAgent.indexOf("Firefox/") + 8;
    return userAgent.substring(start);
  }
  else if (userAgent.indexOf("Safari/") != -1 && userAgent.indexOf("Version/") != -1)
  {
    int start = userAgent.indexOf("Version/") + 8;
    int end = userAgent.indexOf(" ", start);
    return userAgent.substring(start, end);
  }
  return "N/A";
}

String determineManufacturer(String macAddress)
{
  // This is a very basic lookup. For a real application, you would need a much larger database.
  macAddress.toUpperCase();
  if (macAddress.startsWith("00:0C:29"))
    return "VMware";
  if (macAddress.startsWith("00:05:69"))
    return "VMware";
  if (macAddress.startsWith("00:50:56"))
    return "VMware";
  if (macAddress.startsWith("08:00:27"))
    return "VirtualBox";
  if (macAddress.startsWith("00:1C:42"))
    return "Parallels";
  if (macAddress.startsWith("00:A0:C9"))
    return "Intel";
  if (macAddress.startsWith("00:1B:21"))
    return "Asus";
  if (macAddress.startsWith("00:25:64"))
    return "Dell";
  if (macAddress.startsWith("B8:27:EB"))
    return "Raspberry Pi";
  if (macAddress.startsWith("DC:A6:32"))
    return "Raspberry Pi";
  if (macAddress.startsWith("E4:5F:01"))
    return "Raspberry Pi";

  return "Unknown";
}

String extractScreenResolution(String userAgent)
{
  // This is a very basic heuristic and might not be accurate.
  int start = userAgent.indexOf("Resolution/");
  if (start != -1)
  {
    start += 11;
    int end = userAgent.indexOf(" ", start);
    if (end != -1)
    {
      return userAgent.substring(start, end);
    }
  }
  return "N/A";
}
