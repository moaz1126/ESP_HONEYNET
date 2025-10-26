#include "logging.h"
#include <SPIFFS.h>
#include <Arduino.h>

void logIntruderData(String time, String ip, String mac, int rssi, String request, String userAgent, String acceptLang, String primaryLang, String deviceType, String deviceBrand, String deviceModel, String osType, String osVersion, String browser, String browserVersion, String manufacturer, String screenRes, String host, String referer, String dnt, String connection, String accept, unsigned long joinDate)
{
  File file = SPIFFS.open("/intruders.txt", FILE_APPEND);
  if (!file)
  {
    Serial.println("There was an error opening the file for appending");
    return;
  }

  String logEntry = "{\"timestamp\":\"" + time + "\",\"ip\":\"" + ip + "\",\"mac\":\"" + mac + "\",\"rssi\":\"" + String(rssi) + "\",\"request\":\"" + request + "\",\"userAgent\":\"" + userAgent + "\",\"acceptLang\":\"" + acceptLang + "\",\"primaryLang\":\"" + primaryLang + "\",\"deviceType\":\"" + deviceType + "\",\"deviceBrand\":\"" + deviceBrand + "\",\"deviceModel\":\"" + deviceModel + "\",\"osType\":\"" + osType + "\",\"osVersion\":\"" + osVersion + "\",\"browser\":\"" + browser + "\",\"browserVersion\":\"" + browserVersion + "\",\"manufacturer\":\"" + manufacturer + "\",\"screenRes\":\"" + screenRes + "\",\"host\":\"" + host + "\",\"referer\":\"" + referer + "\",\"dnt\":\"" + dnt + "\",\"connection\":\"" + connection + "\",\"accept\":\"" + accept + "\",\"joinDate\":" + String(joinDate) + "}";

  if (file.println(logEntry))
  {
    Serial.println("Intruder logged to SPIFFS: " + logEntry);
  }
  else
  {
    Serial.println("File write failed");
  }
  file.close();
}

String readLogs()
{
  File file = SPIFFS.open("/intruders.txt", "r");
  if (!file)
  {
    return "";
  }
  String logs = "";
  while (file.available())
  {
    logs += file.readStringUntil('\n') + "\n";
  }
  file.close();
  return logs;
}

int countLogs(String text)
{
  int count = 0;
  for (int i = 0; i < text.length(); i++)
  {
    if (text.charAt(i) == '\n')
      count++;
  }
  return count;
}

void displayLogs(WiFiClient &client, String logs)
{
  int lineNum = 1;
  int startPos = 0;
  int endPos = logs.indexOf('\n');

  while (endPos != -1 && lineNum <= 100) // Limit to 100 entries for performance
  {
    String line = logs.substring(startPos, endPos);
    line.trim();

    if (line.length() > 0)
    {
      // Simple JSON parsing
      client.println("<tr>");
      client.println("<td>" + String(lineNum) + "</td>");
      client.println("<td class='time-cell'>" + getValue(line, "timestamp") + "s</td>");
      client.println("<td class='ip-cell'>" + getValue(line, "ip") + "</td>");
      client.println("<td class='mac-cell'>" + getValue(line, "mac") + "</td>");
      client.println("<td>" + getValue(line, "rssi") + "</td>");
      client.println("<td class='device-type'>" + getValue(line, "deviceType") + "</td>");
      client.println("<td class='brand-cell'>" + getValue(line, "deviceBrand") + "</td>");
      client.println("<td class='model-cell'>" + getValue(line, "deviceModel") + "</td>");
      client.println("<td class='os-cell'>" + getValue(line, "osType") + "</td>");
      client.println("<td>" + getValue(line, "osVersion") + "</td>");
      client.println("<td class='browser-cell'>" + getValue(line, "browser") + "</td>");
      client.println("<td>" + getValue(line, "browserVersion") + "</td>");
      client.println("<td>" + getValue(line, "manufacturer") + "</td>");
      client.println("<td>" + getValue(line, "primaryLang") + "</td>");
      client.println("<td>" + getValue(line, "screenRes") + "</td>");
      client.println("<td>" + getValue(line, "host") + "</td>");
      client.println("<td>" + getValue(line, "referer") + "</td>");
      client.println("<td>" + getValue(line, "dnt") + "</td>");
      client.println("<td>" + getValue(line, "connection") + "</td>");
      client.println("<td class='truncate'>" + getValue(line, "accept") + "</td>");
      client.println("<td class='truncate'>" + getValue(line, "request") + "</td>");
      
      unsigned long joinDate = getValue(line, "joinDate").toInt();
      unsigned long currentMillis = millis();
      unsigned long timeDiff = (currentMillis - joinDate) / 1000;
      unsigned long seconds = timeDiff;
      unsigned long minutes = seconds / 60;
      unsigned long hours = minutes / 60;
      unsigned long days = hours / 24;

      String formattedTime = "";
      if (days > 0) {
        formattedTime = String(days) + "d and " + String(hours % 24) + "h and " + String(minutes % 60) + "m";
      } else if (hours > 0) {
        formattedTime = String(hours) + "h and " + String(minutes % 60) + "m";
      } else if (minutes > 0) {
        formattedTime = String(minutes) + "m and " + String(seconds % 60) + "s";
      } else {
        formattedTime = String(seconds) + "s";
      }

      client.println("<td class='time-cell'>" + formattedTime + " ago</td>");
      client.println("</tr>");

      lineNum++;
    }

    startPos = endPos + 1;
    endPos = logs.indexOf('\n', startPos);
  }
}

String getValue(String data, String key)
{
  String search = "\"" + key + "\":\"";
  int start = data.indexOf(search);
  if (start == -1)
  {
    return "N/A";
  }
  start += search.length();
  int end = data.indexOf("\"", start);
  if (end == -1)
  {
    // Handle case where value is not a string (e.g. joinDate)
    end = data.indexOf("}", start);
    if (end == -1) {
      return "N/A";
    }
  }
  return data.substring(start, end);
}

String sanitizeCSV(String input)
{
  input.replace("\"", "\"\"");
  input.replace(",", ";");
  input.replace("\n", " ");
  input.replace("\r", "");
  return input;
}

void clearLogs(WiFiClient client)
{
  SPIFFS.remove("/intruders.txt");
  Serial.println(">>> Logs cleared");

  client.println("HTTP/1.1 302 Found");
  client.println("Location: /");
  client.println();
}

void exportLogs(WiFiClient client)
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/csv");
  client.println("Content-Disposition: attachment; filename=honeypot_logs.csv");
  client.println("Connection: close");
  client.println();

  client.println("Timestamp,IP,MAC,RSSI,Request,User-Agent,Accept-Language,Primary-Lang,Device-Type,Brand,Model,OS,OS-Version,Browser,Browser-Version,Manufacturer,Screen-Resolution,Host,Referer,DNT,Connection,Accept,JoinDate");

  String logs = readLogs();

  // Convert JSON to CSV
  int startPos = 0;
  int endPos = logs.indexOf('\n');
  while (endPos != -1)
  {
    String line = logs.substring(startPos, endPos);
    line.trim();
    if (line.length() > 0)
    {
      String csv = getValue(line, "timestamp") + "," +
                   getValue(line, "ip") + "," +
                   getValue(line, "mac") + "," +
                   getValue(line, "rssi") + "," +
                   getValue(line, "request") + "," +
                   getValue(line, "userAgent") + "," +
                   getValue(line, "acceptLang") + "," +
                   getValue(line, "primaryLang") + "," +
                   getValue(line, "deviceType") + "," +
                   getValue(line, "deviceBrand") + "," +
                   getValue(line, "deviceModel") + "," +
                   getValue(line, "osType") + "," +
                   getValue(line, "osVersion") + "," +
                   getValue(line, "browser") + "," +
                   getValue(line, "browserVersion") + "," +
                   getValue(line, "manufacturer") + "," +
                   getValue(line, "screenRes") + "," +
                   getValue(line, "host") + "," +
                   getValue(line, "referer") + "," +
                   getValue(line, "dnt") + "," +
                   getValue(line, "connection") + "," +
                   getValue(line, "accept") + "," +
                   getValue(line, "joinDate");
      client.println(csv);
    }
    startPos = endPos + 1;
    endPos = logs.indexOf('\n', startPos);
  }
}
