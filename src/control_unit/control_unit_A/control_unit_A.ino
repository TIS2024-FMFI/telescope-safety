
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
int TransformMatrix[3][3];


#define SERVERS 1
#define DISPLAY_A 1
#define INERCIAL 1

// Functions
void setupEthernet();
void setupSettings();
void displayAE(AzimuthElevation* ae);


void setup() {
  Serial.begin(9600);                   // initialize serial
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
  }
  #endif
  
  #if INERCIAL

  #if INERCIAL
  doOperations();
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
  }
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
    if (millis() - lastTry >= timeOut){
      Serial.println("Connection time out");
      break;
    }
    delay(100);
    Serial.print(".");
  }

  Serial.print("IP address: ");
  Serial.println(eth.localIP());
}

void setupSettings(){
  loadSettings();
  matrix.row0 = {0, 0, 0};
  matrix.row1 = {0, 0, 0};
  matrix.row3 = {0, 0, 0};
  Serial.println("jupi2");
}
