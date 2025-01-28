
#include "lora_communication.h"
#include "buttons.h"
#include "logs.h"
#include <W55RP20lwIP.h> // Include library for the right board
Wiznet55rp20lwIP eth(1 /* chip select */);

#include <LEAmDNS.h>
#include "httpHandlers.h"
#include "servers.h"

#define SERVERS 1
#define DISPLAY_A 1
#define INERCIAL 0


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
  setupServers();
  timeClient.begin(8001);
  timeClient.update();
  Serial.println(timeToString(getRealTime()));
  #endif

  AzimuthElevation ae = {45.98, 30.23};

  writeAEtoLog(&ae);
}

long lastSendTime = 0;        // last send time
int interval = 20000;          // interval between sends

void loop() {
  #if DISPLAY_A
  loopButtons();
  #endif

  #if SERVERS
  server.handleClient();
  MDNS.update();
  websocketLoop();
  timeClient.update();
  #endif

  #if INERCIAL
  if (millis() - lastSendTime > interval) {
    while (restartInertialUnit(132.0) != 0);
    lastSendTime = millis();            // timestamp the message
    interval = 20000;    // 20 seconds
  }

  AzimuthElevation* data = readFromInertialUnit();
    if (data) {
        Serial.println(F("Processed incoming message."));
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