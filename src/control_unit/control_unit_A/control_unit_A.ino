
#include "lora_communication.h"
#include "buttons.h"
#include "logs.h"
#include "forbidden_zones_config_parse.h"
#include "danger_evaluation.h"
#include <W55RP20lwIP.h> // Include library for the right board
Wiznet55rp20lwIP eth(1 /* chip select */);

#include <WiFiClient.h>
#include <WebServer.h> 
#include <LEAmDNS.h>
#include <StreamString.h>
#include "httpHandlers.h"
#include "servers.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "time.h"

#define SERVERS 0
#define DISPLAY_A 0
#define INERCIAL 0
#define CONFIGURATION 1
#define DANGER 1

WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "sk.pool.ntp.org", 3600);

// Functions
void setupEthernet();


void setup() {
  Serial.begin(9600);                   // initialize serial
  delay(100);
  Serial.println("Started Serial");
  
  #if INERCIAL
  initializeLoRa();
  #endif

  #if DISPLAY_A
  setupButtons();
  setupSD();
  #endif

  #if SERVERS
  setupEthernet();
  setupHTTPServer();
  setupWebSocketServer();
  setupMDNSServer();
  timeClient.begin(8001);
  timeClient.update();
  #endif
}

long lastSendTime = 0;        // last send time
int interval = 20000;          // interval between sends
boolean reset_flag=true;
boolean file_format_flag=false;

int i = 0;

const char* testConfig =
    "# Zone 1\n"
    "10.0 20.0\n"
    "15.0 25.0\n"
    "20.0 15.0\n"
    "\n"
    "# Zone 2\n"
    "30.0 40.0\n"
    "35.0 45.0\n"
    "40.0 35.0\n";

void loop() {
  #if DISPLAY_A
  loopButtons();
  #endif

  #if SERVERS
  server.handleClient();
  MDNS.update();

  // Testing
  if (i % 5000 == 0){
    AzimuthElevation azel;
    azel.azimuth = i % 1500;
    azel.elevation = i % 2500;
    sendToClients(&azel);
  }
  i++;
  // Serial.println(websocketClients.size());

  timeClient.update();
  // Example of usage
  // Serial.println(timeToString(getRealTime()));
  #endif

  #if INERCIAL
  if (!reset_flag || millis() - lastSendTime > interval) {
    if (restartInertialUnit(132.0) != 0){
      reset_flag=false;
    }
    else{
      lastSendTime = millis();            // timestamp the message
      reset_flag=true;
    }
  }

  AzimuthElevation* data = readFromInertialUnit();
    if (data) {
        Serial.println(F("Processed incoming message."));
    }
  #endif

  #if CONFIGURATION
  // Test the configuration parsing
    int parseResult = checkFileFormat(testConfig);
    if (parseResult == 0) {
        Serial.println("Configuration parsed successfully.");
        file_format_flag=true;
    } else {
        Serial.println("Error parsing configuration.");
        file_format_flag=false;
    }
  #endif

  #if DANGER
  // Test danger
    AzimuthElevation testPoint = {15.0, 20.0};

    // Check if the test point falls in any forbidden zone
    if(file_format_flag){
      int result = checkForbiddenZone(&testPoint);
      if (result == -1) {
          Serial.println("Point is in a forbidden zone!");
      } else {
          Serial.println("Point is safe.");
      }
    }
  #endif
}



// Functions
void setupEthernet(){
  Serial.println("Starting ETH");
  // Start the Ethernet port
  if (!eth.begin()) {
    Serial.println("No wired Ethernet hardware detected. Check pinouts, wiring.");
    while (1) {
      delay(1000);
    }
  }

  // Wait for connection
  while (!eth.connected()) {
    Serial.print("_");
    delay(500);
  }
  while (eth.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(eth.localIP());
}
