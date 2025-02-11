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
int lastManageDay = -1;


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

bool resetflag=false;
double azimuth;
unsigned long lastSend = 0;
int sendTimeOut = 60000; // 60s



void loop() {

  if (millis() - lastSend >= sendTimeOut){
    lastSend = millis();
    Serial.println("Sending random message");
    LoRa.beginPacket();
    LoRa.write(localAddress);
    LoRa.write(1);
    LoRa.write(1);
    if (!LoRa.endPacket()){
      Serial.println("Packet wasn't sent.");
    }
    else {
      Serial.println("Packet sent successfuly.");
    }
  }

  #if DISPLAY_A
  // Serial.println("Looping.... Buttons");
  loopButtons();
  #endif


  #if SERVERS
  if (eth.status() == WL_CONNECTED){
    //Serial.println("Looping.... Clients HTTP");
    //Serial.println(eth.localIP());
    server.handleClient();
    // Serial.println("Looping.... MDNS");
    MDNS.update();
    // Serial.println("Looping.... WS");
    websocketLoop();
    // Serial.println("Looping.... Time");
    timeClient.update();
  }
  #endif

  #if INERCIAL
  // Serial.println("Looping.... Inertial");
  doOperations();
  #endif

  if(resetflag){
    // Serial.println("Looping.... In reset");
    if (restartInertialUnit(azimuth) == 0){
      resetflag=false;
    }   
  }

  if (toNano.available()) {
    // Serial.println("Looping.... In NANO send");
    String azimuthStr = toNano.readStringUntil('\n');
    int firstSpace = azimuthStr.indexOf(' ');
    int secondSpace = azimuthStr.indexOf(' ', firstSpace + 1);
    DegreesMinutesSeconds azimuthDMS;
    azimuthDMS.degrees = azimuthStr.substring(0, firstSpace).toInt();
    azimuthDMS.minutes = azimuthStr.substring(firstSpace + 1, secondSpace).toInt();
    azimuthDMS.seconds = azimuthStr.substring(secondSpace + 1).toInt();
    azimuth = convertToDecimalDegrees(azimuthDMS);
    // azimuth = -1;
    resetflag = true;
  }

  Time now = getRealTime();
  if (now.hours == 9 && now.day != lastManageDay) {
    Serial.println("Idem manageSD");
    manageSDSpace();
    lastManageDay = now.day;
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
  char* file = loadFile(matrixFilePath);
  setUpMatrix(file);
  free(file);
  Serial.println("jupi2");
}
