
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

#define SERVERS 0
#define DISPLAY_A 1
#define INERCIAL 1

// Functions
void setupEthernet();
void setupSettings();
void displayAE(AzimuthElevation* ae);


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
  setupSettings();
  setupStaticFiles();
  #endif

  #if SERVERS
  setupEthernet();
  setupServers();
  timeClient.begin(8001);
  timeClient.update();
  Serial.println(timeToString(getRealTime()));
  #endif

  setupAlarm();
  setupMotors();
}

int send = 0;


void loop() {
  #if DISPLAY_A
  loopButtons();
  #endif
  #if SERVERS
  if (eth.status() == WL_CONNECTED){
    server.handleClient();
    MDNS.update();
    websocketLoop();
    timeClient.update();
  }       // neets to be tested, if the connection can be established automaticaly or has to be call setup again
  // else {
  //   setupEthernet();
  // }
  #endif


  #if INERCIAL
  AzimuthElevation* data = readFromInertialUnit();
    if (data) {
      if (send == 0 || send%100 == 0) {
        displayAE(data);
        send = 0;
      }
      send++;
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
    restartInertialUnit(azimuth);
    send = 99;
  }

  //testing_parsation_and_evaluation();

}



// Functions


int timeOut = 400;

void setupEthernet(){
  Serial.println("Starting ETH");
  // Start the Ethernet port
  if (!eth.begin()) {
    Serial.println("No wired Ethernet hardware detected. Check pinouts, wiring.");
    while (1) {
      delay(1000);
    }
  }
  unsigned long lastTry = millis();

  // Wait for connection
  while (eth.status() != WL_CONNECTED) {
    if (lastTry - millis() >= timeOut){
      Serial.println("Connection time out");
      break
    }
    delay(100);
    Serial.print(".");
  }

  // while (!eth.connected()) {
  //   Serial.print("_");
  //   delay(100);
  // }
  Serial.print("IP address: ");
  Serial.println(eth.localIP());
}

void setupSettings(){
  loadSettings();
}

int lastUpdateDisplay = 0;
const int secendsToMilis = 1000;

void displayAE(AzimuthElevation* ae) {
  if ((millis() - lastUpdateDisplay) >= (settings.update_frequency * secendsToMilis)){
    lastUpdateDisplay = millis();
    DegreesMinutesSeconds azimuth = convertToDMS(ae->azimuth, false);
    DegreesMinutesSeconds elevation = convertToDMS(ae->elevation, true);
    char aeSend[20];
    sprintf(aeSend, "%d %d %d\n%d %d %d\n", azimuth.degrees, azimuth.minutes, azimuth.seconds, elevation.degrees, elevation.minutes, elevation.seconds);
    toNano.write(aeSend);
  }
}