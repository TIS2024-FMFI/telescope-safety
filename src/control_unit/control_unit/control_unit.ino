// Example works with either Wired or WiFi Ethernet, define one of these values to 1, other to 0
#define USE_WIFI 0
#define USE_WIRED 1


#if USE_WIFI
#include <WiFi.h>
#elif USE_WIRED
#include <W55RP20lwIP.h> // Or W5100lwIP.h or ENC28J60lwIP.h
Wiznet55rp20lwIP eth(1 /* chip select */); // or Wiznet5100lwIP or ENC28J60lwIP
#endif

#include <WiFiClient.h>
#include <WebServer.h> 
#include <LEAmDNS.h>
#include <StreamString.h>
#include "httpHandlers.h"

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

// Define form names:
#define ZONE_CONFIG_BUTTON "config_zones"
#define ZONE_CONFIG_FIELD "config_zones_field"
#define ALARM_CONFIG_BUTTON "config_alarm"
#define LOG_INTERVAL_FIELD "log_interval"
#define UPDATE_INTERVAL_FIELD "update_interval"
#define ALARM_CHECKBOX "alarm_conf"
#define MOTORS_CHECKBOX "motors_conf"
#define TURN_OFF_LOGS_CHECKBOX "turn_off_logs"
#define RESTART_BUTTON "restart"
#define DOWNLOAD_EVENTS_BUTTON "download_events"
#define DOWNLOAD_LOGS_BUTTON "download_logs"
#define DOWNLOAD_LOGS_FROM_DATEFIELD "logs_from_date"
#define DOWNLOAD_LOGS_TO_DATEFIELD "logs_to_date"


const char *ssid = STASSID;
const char *password = STAPSK;

WebServer server(80);


void setup(void) {
  Serial.begin(115200);
  delay(5000);
  Serial.println("Started Serial");

#if USE_WIFI
  Serial.println("Starting Wi-Fi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#elif USE_WIRED
  Serial.println("Starting ETH");
  // Start the Ethernet port
  if (!eth.begin()) {
    Serial.println("No wired Ethernet hardware detected. Check pinouts, wiring.");
    while (1) {
      delay(1000);
    }
  }

  while (!eth.connected()) {
    Serial.print("_");
    delay(500);
  }

  // Wait for connection
  while (eth.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(eth.localIP());
#endif

  Serial.println("Starting mDNS!");
  if (MDNS.begin("picow")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleMainPage);
  server.on("/config", HTTP_GET, handleFormPage);
  server.on("/config", HTTP_POST, handleFormPOST);
  server.on("/styles.css", handleCSS);
  server.on("/form.js", handleJSForm);
  server.on("/main.js", handleJSMain);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
