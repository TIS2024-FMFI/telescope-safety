#include "lora_communication.h"

AzimuthElevation inertialData;

const unsigned long timeout = 5000;   // Timeout for acknowledgment (in milliseconds)
bool flagSend = true;
bool ackFlag = false;
unsigned long startTime = 0;
const int resetTime = 1500;
unsigned long lastRead = 0;

void restartLoRa(){
  Serial.println("Resetting LoRa module...");
  digitalWrite(LORA_RESET_PIN, LOW);
  delay(10);
  digitalWrite(LORA_RESET_PIN, HIGH);
  delay(100);
  LoRa.begin(LORA_FREQUENCY);
}

void initializeLoRa() {
  SPI1.setSCK(10);
  SPI1.setTX(11);
  SPI1.setRX(12);

  LoRa.setSPI(SPI1);

  LoRa.setPins(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("LoRa initialization failed. Check connections!");
    while (true);
  }

  Serial.println("LoRa initialization succeeded.");
}

AzimuthElevation* readFromInertialUnit() {
  int packetSize = LoRa.parsePacket();  // Kontrola prichádzajúcej správy
  if (packetSize == 0) {
    if (millis() - lastRead >= resetTime){
      lastRead = millis();
      Serial.println("Haven't recived any data for too long. Restarting LoRa");
      restartLoRa();
      Serial.println("LoRa has been restarted");
    }
    return nullptr;  // Žiadna správa nebola prijatá
  }
  
  lastRead = millis();

  

  // Čítanie dĺžky prichádzajúcej správy
  int sender = LoRa.read();
  byte incomingLength = LoRa.read();  // Dĺžka správy

  String incoming = "";
  while (LoRa.available()) {
    incoming += (char)LoRa.read();  // Čítanie obsahu správy
  }

  // Výpis prijatých údajov
  // Serial.println("Received data from inertial unit: " + incoming);

  if (incomingLength != incoming.length()) {  // Overenie správnej dĺžky
    Serial.println("Error: message length mismatch.");
    return nullptr;
  }

  if (sender != 0xBB) {
      Serial.println("This message is not for me.");
      return nullptr;
  }

  if (incoming == "ACK:RESTART_INERTIAL_UNIT") {
    ackFlag = true;
    return nullptr;
  }

  // Očakávaný formát: "Azimuth: <value>, Elevation: <value>"
  int azimuthIndex = incoming.indexOf("Azimuth: ");
  int elevationIndex = incoming.indexOf("Elevation: ");
  if (azimuthIndex == -1 || elevationIndex == -1) {
    Serial.println("Error: invalid data format.");
    return nullptr;
  }

  // Extrahovanie azimutu a elevácie
  String azimuthStr = incoming.substring(azimuthIndex + 9, elevationIndex - 2);
  String elevationStr = incoming.substring(elevationIndex + 11);

  azimuthStr.trim();  // Odstránenie bielych znakov
  elevationStr.trim();

  // Konverzia na double
  inertialData.azimuth = azimuthStr.toDouble();
  inertialData.elevation = elevationStr.toDouble();

  // Overenie NaN hodnôt
  if (isnan(inertialData.azimuth) || isnan(inertialData.elevation)) {
    Serial.println("Error: received NaN values.");
    return nullptr;
  }

  // Výpis spracovaných hodnôt
  Serial.print("Parsed Azimuth: ");
  Serial.print(inertialData.azimuth, 2);  // Presnosť na 2 desatinných miest
  Serial.print(", Elevation: ");
  Serial.println(inertialData.elevation, 2);  // Presnosť na 2 desatinných miest

  return &inertialData;
}



int restartInertialUnit(double azimuth, int calibrationMatrix[3][3]) {
  // Serial.printf("restartInertialUnit, flagSend=%s\n", flagSend ? "true" : "false");
  if (flagSend){
    String restartCommand = "RESTART_INERTIAL_UNIT:";
    restartCommand += String(azimuth, 2);  // Add azimuth with 2 decimal places
    LoRa.beginPacket();
    LoRa.write(localAddress);
    LoRa.write(restartCommand.length());
    LoRa.print(restartCommand);
    if (!LoRa.endPacket()) {
      Serial.println("Failed to send restart command.");
      return -1;
    }
    startTime = millis();
    flagSend = false;
    ackFlag=false;
    Serial.println("Restart command sent successfully. Waiting for acknowledgment...");
    return -1;
  }
  else {
    if (millis() - startTime < timeout) {
      if (ackFlag){
        Serial.println("Acknowledgment received from inertial unit.");
        flagSend = true;
        ackFlag = false;
        return 0; // Success
      }
    }
    else {
      Serial.println("Acknowledgment not received.");
      flagSend = true;
      return -1; // Failure after retries
    }
  }
  return -1;
}

int lastUpdateDisplay = 0;
const int secendsToMilis = 1000;
const int displayUpdate = 10;

void displayAE(AzimuthElevation* ae) {
  if ((millis() - lastUpdateDisplay) >= (displayUpdate * secendsToMilis)){
    lastUpdateDisplay = millis();
    DegreesMinutesSeconds azimuth = convertToDMS(ae->azimuth, false);
    DegreesMinutesSeconds elevation = convertToDMS(ae->elevation, true);
    char aeSend[20];
    sprintf(aeSend, "%d %d %d\n%d %d %d\n", azimuth.degrees, azimuth.minutes, azimuth.seconds, elevation.degrees, elevation.minutes, elevation.seconds);
    toNano.write(aeSend);
  }
}

void doOperations(){
  AzimuthElevation* data = readFromInertialUnit();
  if (data) {
    checkForbiddenZone(data);
    displayAE(data);
    sendToClients(data);
    writeAEtoLog(data);
  }
  else {
    // Serial.println("DATA = nullptr");
  }
}
