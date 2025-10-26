#include "web_server.h"
#include "logging.h"
#include "fingerprinting.h"
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include "esp_wifi.h"
#include "config.h"

IPAddress myIP;

void serveHoneypot(WiFiClient client, String fullRequest, LiquidCrystal_I2C &lcd)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Intruder Alert!");
  Serial.println("!!! Intruder Alert: New client connected !!!");

  String clientIP = client.remoteIP().toString();
  Serial.println("Client IP address: " + clientIP);
  lcd.setCursor(0, 1);
  lcd.print(clientIP);

  // Get MAC address and signal strength
  wifi_sta_list_t wifi_sta_list;
  esp_wifi_ap_get_sta_list(&wifi_sta_list);

  String macAddress = "Unknown";
  int rssi = 0;

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
    rssi = station.rssi;
    Serial.println("Client MAC: " + macAddress + " | RSSI: " + String(rssi) + " dBm");
  }
  macAddress.toUpperCase();

  // Parse HTTP headers
  String userAgent = extractHeader(fullRequest, "User-Agent");
  String acceptLang = extractHeader(fullRequest, "Accept-Language");
  String acceptEncoding = extractHeader(fullRequest, "Accept-Encoding");
  String connection = extractHeader(fullRequest, "Connection");
  String host = extractHeader(fullRequest, "Host");
  String referer = extractHeader(fullRequest, "Referer");
  String dnt = extractHeader(fullRequest, "DNT");
  String secFetchSite = extractHeader(fullRequest, "Sec-Fetch-Site");
  String secFetchMode = extractHeader(fullRequest, "Sec-Fetch-Mode");
  String secFetchDest = extractHeader(fullRequest, "Sec-Fetch-Dest");
  String accept = extractHeader(fullRequest, "Accept");

  // Extract request line
  String requestLine = fullRequest.substring(0, fullRequest.indexOf('\n'));
  requestLine.trim();

  // Get timestamp
  unsigned long timestamp = millis();
  String timeStr = String(timestamp / 1000);

  // Get current time for join date
  unsigned long joinDate = millis();

  // Advanced device fingerprinting
  String deviceType = determineDeviceType(userAgent);
  String deviceBrand = determineDeviceBrand(userAgent);
  String deviceModel = determineDeviceModel(userAgent);
  String osType = determineOS(userAgent);
  String osVersion = determineOSVersion(userAgent);
  String browser = determineBrowser(userAgent);
  String browserVersion = determineBrowserVersion(userAgent);
  String manufacturer = determineManufacturer(macAddress);
  String screenRes = extractScreenResolution(userAgent);

  // Determine primary language
  String primaryLang = acceptLang;
  if (primaryLang.indexOf(',') > 0)
  {
    primaryLang = primaryLang.substring(0, primaryLang.indexOf(','));
  }

  // Log comprehensive data
  logIntruderData(timeStr, clientIP, macAddress, rssi, requestLine, userAgent,
                  acceptLang, primaryLang, deviceType, deviceBrand, deviceModel,
                  osType, osVersion, browser, browserVersion, manufacturer,
                  screenRes, host, referer, dnt, connection, accept, joinDate);

  // Send fake IoT device page
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println("Server: IoT-Hub/1.2.5");
  client.println();

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>IoT Smart Home Hub</title><style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:'Segoe UI',Tahoma,Geneva,Verdana,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;display:flex;align-items:center;justify-content:center;padding:20px}.container{background:rgba(255,255,255,0.95);border-radius:20px;box-shadow:0 20px 60px rgba(0,0,0,0.3);max-width:500px;width:100%;padding:40px;backdrop-filter:blur(10px);animation:slideIn 0.5s ease}@keyframes slideIn{from{opacity:0;transform:translateY(-30px)}to{opacity:1;transform:translateY(0)}}.header{text-align:center;margin-bottom:30px}.icon{width:80px;height:80px;margin:0 auto 20px;background:linear-gradient(135deg,#667eea,#764ba2);border-radius:50%;display:flex;align-items:center;justify-content:center;box-shadow:0 10px 25px rgba(102,126,234,0.4);animation:pulse 2s infinite}@keyframes pulse{0%,100%{transform:scale(1)}50%{transform:scale(1.05)}}.icon svg{width:40px;height:40px;fill:white}h1{color:#333;font-size:28px;margin-bottom:10px;font-weight:600}.subtitle{color:#666;font-size:14px}.status-card{background:#f8f9fa;border-radius:12px;padding:20px;margin:20px 0;border-left:4px solid #667eea}.status-item{display:flex;justify-content:space-between;align-items:center;padding:12px 0;border-bottom:1px solid #e0e0e0}.status-item:last-child{border-bottom:none}.label{color:#666;font-size:14px;font-weight:500}.value{color:#333;font-weight:600;font-size:14px}.status-indicator{width:10px;height:10px;border-radius:50%;background:#4caf50;display:inline-block;margin-right:8px;animation:blink 1.5s infinite}@keyframes blink{0%,100%{opacity:1}50%{opacity:0.3}}.warning{background:#fff3cd;border-left:4px solid #ffc107;padding:15px;border-radius:8px;margin:20px 0;display:flex;align-items:center;gap:12px}.warning-icon{color:#ff9800;font-size:24px}.warning-text{color:#856404;font-size:14px}</style></head><body><div class='container'><div class='header'><div class='icon'><svg viewBox='0 0 24 24'><path d='M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm-1 15v-2h2v2h-2zm0-4V7h2v6h-2z'/></svg></div><h1>IoT Smart Home Hub</h1><p class='subtitle'>Device Offline - Requires Attention</p></div><div class='status-card'><div class='status-item'><span class='label'>Status</span><span class='value'><span class='status-indicator'></span>Connected</span></div><div class='status-item'><span class='label'>Firmware</span><span class='value'>v1.2.5 (Latest)</span></div><div class='status-item'><span class='label'>IP Address</span><span class='value'>192.168.4.1</span></div><div class='status-item'><span class='label'>Signal Strength</span><span class='value'>Excellent</span></div></div><div class='warning'><div class='warning-icon'>⚠️</div><div class='warning-text'><strong>Security Alert:</strong> Unrecognized device connection attempt blocked. Please review your network security settings.</div></div></div></body></html>";

  client.println(html);

  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Honeypot Active");
  lcd.setCursor(0, 1);
  lcd.print(myIP.toString());
}

void serveAdminPanel(WiFiClient client, LiquidCrystal_I2C &lcd)
{
  Serial.println(">>> Admin panel accessed");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Admin Access");
  lcd.setCursor(0, 1);
  lcd.print(client.remoteIP().toString());

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();

  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("<meta http-equiv='refresh' content='30'>");
  client.println("<title>HoneyNet Admin Panel</title>");
  client.println("<style>");
  client.println("*{margin:0;padding:0;box-sizing:border-box}");
  client.println("body{font-family:'Segoe UI',Tahoma,Geneva,Verdana,sans-serif;background:#1a1a2e;color:#eee;padding:20px}");
  client.println(".container{max-width:1800px;margin:0 auto}");
  client.println(".header{background:linear-gradient(135deg,#667eea,#764ba2);padding:30px;border-radius:15px;margin-bottom:30px;box-shadow:0 10px 40px rgba(0,0,0,0.3)}");
  client.println("h1{font-size:36px;margin-bottom:5px;display:flex;align-items:center;gap:15px}");
  client.println(".subtitle{opacity:0.9;font-size:14px}");
  client.println(".stats-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(250px,1fr));gap:20px;margin-bottom:30px}");
  client.println(".stat-card{background:#16213e;padding:25px;border-radius:12px;border-left:4px solid #667eea;box-shadow:0 5px 15px rgba(0,0,0,0.2)}");
  client.println(".stat-value{font-size:32px;font-weight:bold;color:#667eea;margin-bottom:5px}");
  client.println(".stat-label{color:#aaa;font-size:14px;text-transform:uppercase;letter-spacing:1px}");
  client.println(".logs-section{background:#16213e;border-radius:12px;padding:30px;box-shadow:0 5px 15px rgba(0,0,0,0.2)}");
  client.println(".section-header{display:flex;justify-content:space-between;align-items:center;margin-bottom:20px;padding-bottom:15px;border-bottom:2px solid #667eea;flex-wrap:wrap;gap:10px}");
  client.println("h2{font-size:24px;color:#667eea}");
  client.println(".btn-group{display:flex;gap:10px}");
  client.println(".btn{color:white;border:none;padding:10px 20px;border-radius:8px;cursor:pointer;font-size:14px;transition:all 0.3s;text-decoration:none;display:inline-block}");
  client.println(".clear-btn{background:#e74c3c}");
  client.println(".clear-btn:hover{background:#c0392b;transform:translateY(-2px)}");
  client.println(".export-btn{background:#27ae60}");
  client.println(".export-btn:hover{background:#229954;transform:translateY(-2px)}");
  client.println(".table-container{overflow-x:auto}");
  client.println("table{width:100%;border-collapse:collapse;font-size:12px}");
  client.println("thead{background:#0f3460;position:sticky;top:0}");
  client.println("th{padding:12px 8px;text-align:left;font-weight:600;color:#667eea;border-bottom:2px solid #667eea;font-size:11px;white-space:nowrap}");
  client.println("td{padding:10px 8px;border-bottom:1px solid #2d3561;font-size:12px}");
  client.println("tr:hover{background:#1a2847}");
  client.println(".ip-cell{color:#3498db;font-family:monospace}");
  client.println(".mac-cell{color:#2ecc71;font-family:monospace;font-size:11px}");
  client.println(".time-cell{color:#f39c12;font-size:11px}");
  client.println(".device-badge{display:inline-block;padding:4px 8px;border-radius:12px;font-size:10px;font-weight:600;white-space:nowrap}");
  client.println(".badge-mobile{background:#e74c3c;color:white}");
  client.println(".badge-desktop{background:#3498db;color:white}");
  client.println(".badge-tablet{background:#9b59b6;color:white}");
  client.println(".badge-tv{background:#e67e22;color:white}");
  client.println(".badge-bot{background:#95a5a6;color:white}");
  client.println(".brand-cell{color:#e67e22;font-weight:600}");
  client.println(".model-cell{color:#95a5a6;font-size:11px}");
  client.println(".os-cell{color:#9b59b6}");
  client.println(".browser-cell{color:#3498db}");
  client.println(".signal-strong{color:#27ae60;font-weight:bold}");
  client.println(".signal-medium{color:#f39c12;font-weight:bold}");
  client.println(".signal-weak{color:#e74c3c;font-weight:bold}");
  client.println(".truncate{max-width:150px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}");
  client.println(".detail-row{background:#0f1929;font-size:11px;color:#95a5a6}");
  client.println(".detail-row td{padding:8px 8px 8px 20px}");
  client.println(".detail-label{color:#667eea;font-weight:600;margin-right:5px}");
  client.println(".no-data{text-align:center;padding:40px;color:#aaa;font-size:16px}");
  client.println(".refresh-info{text-align:center;margin-top:20px;color:#666;font-size:13px}");
  client.println(".blink{animation:blink 1.5s infinite}");
  client.println("@keyframes blink{0%,100%{opacity:1}50%{opacity:0.3}}");
  client.println("</style>");
  client.println("<script>");
  client.println("function formatTime() {");
  client.println("  const timeCells = document.querySelectorAll('.time-cell');");
  client.println("  timeCells.forEach(cell => {");
  client.println("    const joinDate = parseInt(cell.getAttribute('data-join-date'));");
  client.println("    if (!isNaN(joinDate)) {");
  client.println("      const now = new Date().getTime();");
  client.println("      const diff = Math.floor((now - joinDate) / 1000);");
  client.println("      const seconds = diff;");
  client.println("      const minutes = Math.floor(seconds / 60);");
  client.println("      const hours = Math.floor(minutes / 60);");
  client.println("      const days = Math.floor(hours / 24);");

  client.println("      let formattedTime = '';");
  client.println("      if (days > 0) {");
  client.println("        formattedTime = `${days}d and ${hours % 24}h and ${minutes % 60}m`;");
  client.println("      } else if (hours > 0) {");
  client.println("        formattedTime = `${hours}h and ${minutes % 60}m`;");
  client.println("      } else if (minutes > 0) {");
  client.println("        formattedTime = `${minutes}m and ${seconds % 60}s`;");
  client.println("      } else {");
  client.println("        formattedTime = `${seconds}s`;");
  client.println("      }");
  client.println("      cell.textContent = `${formattedTime} ago`;");
  client.println("    }");
  client.println("  });");
  client.println("}");
  client.println("setInterval(formatTime, 1000);");
  client.println("</script>");
  client.println("</head>");
  client.println("<body>");
  client.println("<div class='container'>");
  client.println("<div class='header'>");
  client.println("<h1>🛡️ HoneyNet Admin Panel</h1>");
  client.println("<p class='subtitle'>Advanced Device Fingerprinting & Intelligence Gathering System</p>");
  client.println("</div>");

  // Stats section
  String logs = readLogs();
  int logCount = countLogs(logs);
  int currentConnections = WiFi.softAPgetStationNum();

  client.println("<div class='stats-grid'>");
  client.println("<div class='stat-card'>");
  client.println("<div class='stat-value'>" + String(logCount) + "</div>");
  client.println("<div class='stat-label'>Total Intrusions</div>");
  client.println("</div>");
  client.println("<div class='stat-card'>");
  client.println("<div class='stat-value blink'>" + String(currentConnections) + "</div>");
  client.println("<div class='stat-label'>Active Connections</div>");
  client.println("</div>");
  client.println("<div class='stat-card'>");
  client.println("<div class='stat-value'>" + String(millis() / 1000) + "s</div>");
  client.println("<div class='stat-label'>System Uptime</div>");
  client.println("</div>");
  client.println("<div class='stat-card'>");
  client.println("<div class='stat-value'>ACTIVE</div>");
  client.println("<div class='stat-label'>Honeypot Status</div>");
  client.println("</div>");
  client.println("</div>");

  // Logs table
  client.println("<div class='logs-section'>");
  client.println("<div class='section-header'>");
  client.println("<h2>📊 Device Intelligence Database</h2>");
  client.println("<div class='btn-group'>");
  client.println("<a href='/export' class='btn export-btn'>📥 Export CSV</a>");
  client.println("<a href='/clear' class='btn clear-btn'>🗑️ Clear All</a>");
  client.println("</div>");
  client.println("</div>");

  if (logs.length() > 0)
  {
    client.println("<div class='table-container'>");
    client.println("<table>");
    client.println("<thead>");
    client.println("<tr>");
    client.println("<th>#</th>");
    client.println("<th>Time</th>");
    client.println("<th>IP</th>");
    client.println("<th>MAC</th>");
    client.println("<th>RSSI</th>");
    client.println("<th>Type</th>");
    client.println("<th>Brand</th>");
    client.println("<th>Model</th>");
    client.println("<th>OS</th>");
    client.println("<th>OS Version</th>");
    client.println("<th>Browser</th>");
    client.println("<th>Browser Version</th>");
    client.println("<th>Manufacturer</th>");
    client.println("<th>Lang</th>");
    client.println("<th>Screen Res</th>");
    client.println("<th>Host</th>");
    client.println("<th>Referer</th>");
    client.println("<th>DNT</th>");
    client.println("<th>Connection</th>");
    client.println("<th>Accept</th>");
    client.println("<th>Request</th>");
    client.println("<th>Join Date</th>");
    client.println("</tr>");
    client.println("</thead>");
    client.println("<tbody>");

    displayLogs(client, logs);

    client.println("</tbody>");
    client.println("</table>");
    client.println("</div>");
  }
  else
  {
    client.println("<div class='no-data'>");
    client.println("📭 No intrusions detected yet. System is monitoring...");
    client.println("</div>");
  }

  client.println("<div class='refresh-info'>AP: " + myIP.toString() + " | Storage: SPIFFS</div>");
  client.println("</div>");
  client.println("</body>");
  client.println("</html>");

  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Honeypot Active");
  lcd.setCursor(0, 1);
  lcd.print(myIP.toString());
}

bool isWhitelisted(String mac)
{
  for (int i = 0; i < whitelist_size; i++)
  {
    if (mac == whitelist[i])
    {
      return true;
    }
  }
  return false;
}
