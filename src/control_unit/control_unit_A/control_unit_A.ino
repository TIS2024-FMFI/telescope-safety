
#include "lora_communication.h"
#include "buttons.h"
#include "danger_evaluation.h"
#include <W55RP20lwIP.h> // Include library for the right board
Wiznet55rp20lwIP eth(1 /* chip select */);

#include <LEAmDNS.h>
#include "httpHandlers.h"
#include "servers.h"
#include "logs.h"

Settings settings;

#define SERVERS 1
#define DISPLAY_A 1
#define INERCIAL 1

// Functions
void setupEthernet();
void setupSettings();


void setup() {
  Serial.begin(9600);                   // initialize serial
  //delay(100);
  while (!Serial);
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
int send = 0;


void loop() {
  #if DISPLAY_A
  //setupSettings();
  loopButtons();
  #endif
  #if SERVERS
  server.handleClient();
  MDNS.update();
  websocketLoop();
  timeClient.update();
  #endif
  #if INERCIAL
  // if (!reset_flag || millis() - lastSendTime > interval) {
  //   if (restartInertialUnit(132.0) != 0){
  //     reset_flag=false;
  //   }
  //   else{
  //     lastSendTime = millis();            // timestamp the message
  //     reset_flag=true;
  //   }
  // }

  AzimuthElevation* data = readFromInertialUnit();
    if (data) {
      if (send == 0 || send%10 == 0) {
        DegreesMinutesSeconds azimuth = convertToDMS(data->azimuth, false);
        DegreesMinutesSeconds elevation = convertToDMS(data->elevation, true);
        char ae[20];
        sprintf(ae, "%d %d %d\n%d %d %d\n", azimuth.degrees, azimuth.minutes, azimuth.seconds, elevation.degrees, elevation.minutes, elevation.seconds);
        toNano.write(ae);
        send = 0;
      }
      send++;
        //Serial.println(F("Processed incoming message."));
    }
  #endif

  if (toNano.available()) {
    String azimuthStr = toNano.readStringUntil('\n');
    int firstSpace = azimuthStr.indexOf(' ');
    int secondSpace = azimuthStr.indexOf(' ', firstSpace + 1);
    DegreesMinutesSeconds azimuthDMS;
    azimuthDMS.degrees = azimuthStr.substring(0, firstSpace).toInt();
    azimuthDMS.minutes = azimuthStr.substring(firstSpace + 1, secondSpace).toInt();
    azimuthDMS.seconds = azimuthStr.substring(secondSpace + 1).toInt();
    double azimuth = convertToDecimalDegrees(azimuthDMS);
    Serial.println("Posielam");
    Serial.println(azimuthStr);
    Serial.println(azimuthDMS.degrees);
    Serial.println(azimuth);
    while (restartInertialUnit(azimuth) != 0);
    send = 99;
  }

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
