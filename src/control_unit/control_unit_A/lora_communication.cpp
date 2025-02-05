
#include "lora_communication.h"

AzimuthElevation inertialData;

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
        return nullptr;  // Žiadna správa nebola prijatá
    }

    // Čítanie dĺžky prichádzajúcej správy
    int sender = LoRa.read();
    byte incomingLength = LoRa.read();  // Dĺžka správy

    String incoming = "";
    while (LoRa.available()) {
        incoming += (char)LoRa.read();  // Čítanie obsahu správy
    }

    if (incomingLength != incoming.length()) {  // Overenie správnej dĺžky
        Serial.println("Error: message length mismatch.");
        return nullptr;
    }

    if (sender != 0xBB) {
        Serial.println("This message is not for me.");
        return nullptr;
    }

    // Výpis prijatých údajov
    //Serial.println("Received data from inertial unit: " + incoming);

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

int restartInertialUnit(double azimuth) {
    String restartCommand = "RESTART_INERTIAL_UNIT:";
    restartCommand += String(azimuth, 2);  // Add azimuth with 2 decimal places

    const unsigned long timeout = 400;   // Timeout for acknowledgment (in milliseconds)

    LoRa.beginPacket();
    LoRa.write(localAddress);
    LoRa.write(restartCommand.length());
    LoRa.print(restartCommand);
    if (!LoRa.endPacket()) {
      Serial.println("Failed to send restart command.");
      return -1;
    }

    Serial.println("Restart command sent successfully. Waiting for acknowledgment...");

    // Wait for acknowledgment
    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
      int packetSize = LoRa.parsePacket();
      if (packetSize > 0) {
        // Read acknowledgment
        String ackMessage = "";
        while (LoRa.available()) {
          ackMessage += (char)LoRa.read();
        }

        if (ackMessage == "ACK:RESTART_INERTIAL_UNIT") {
          Serial.println("Acknowledgment received from inertial unit.");
          return 0; // Success
        }
      }
    }

    Serial.println("Acknowledgment not received.");
    return -1; // Failure after retries
}

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

void doOperations(){
  AzimuthElevation* data = readFromInertialUnit();
    if (data) {
      if(checkForbiddenZone(data)==0){
        displayAE(data);
        sendToClients(data);
        writeAEtoLog(data);
      }
      else(
        writeAlarmtoLog(data);
      )
    }
}
