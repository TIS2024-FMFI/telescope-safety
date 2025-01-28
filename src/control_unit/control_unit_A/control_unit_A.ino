
#include "lora_communication.h"
#include "buttons.h"
#include "danger_evaluation.h"
#include <W55RP20lwIP.h> // Include library for the right board
Wiznet55rp20lwIP eth(1 /* chip select */);

#include <LEAmDNS.h>
#include "httpHandlers.h"
#include "servers.h"

Settings settings;

#define SERVERS 0
#define DISPLAY_A 0
#define INERCIAL 0

// Functions
void setupEthernet();
void setupSettings();


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
}

long lastSendTime = 0;        // last send time
int interval = 20000;          // interval between sends
boolean reset_flag=true;



void loop() {
  #if DISPLAY_A
  setupSettings();
  loopButtons();
  #endif

  #if SERVERS
  server.handleClient();
  MDNS.update();
  websocketLoop();
  timeClient.update();
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

  //testing_parsation_and_evaluation();

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

void setupSettings(){
}
