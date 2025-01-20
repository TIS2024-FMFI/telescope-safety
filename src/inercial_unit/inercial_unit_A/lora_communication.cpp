
#include "lora_communication.h"

// Inicializácia LoRa
void initializeLoRa() {
    LoRa.setPins(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN);

    if (!LoRa.begin(LORA_FREQUENCY)) {
        Serial.println("LoRa initialization failed. Check connections!");
        while (true);  // Nekonečný cyklus v prípade zlyhania
    }

    Serial.println("LoRa initialization succeeded.");
}

int sendToControlUnit(AzimuthElevation* azimuthElevation) {
    if (!azimuthElevation || isnan(azimuthElevation->azimuth) || isnan(azimuthElevation->elevation)) {
        Serial.println("Invalid azimuth or elevation data.");
        return -1;
    }

    String outgoing = String("Azimuth: ") + azimuthElevation->azimuth + ", Elevation: " + azimuthElevation->elevation;

    LoRa.beginPacket();               // Začiatok balíka
    LoRa.write(localAddress);
    LoRa.write(outgoing.length());    // Dĺžka správy
    LoRa.print(outgoing);             // Obsah správy
    if (!LoRa.endPacket()) {
        Serial.println("LoRa transmission failed.");
        return -1;  // Chyba pri odosielaní
    }

    Serial.println("Sending to control unit: " + outgoing);
    return 0;
}

int readFromControlUnit() {
    int packetSize = LoRa.parsePacket();
    if (packetSize == 0) {
        return -1;  // Žiadna správa nebola prijatá
    }

    // Čítanie hlavičky
    int sender = LoRa.read();
    byte incomingLength = LoRa.read();   // Dĺžka správy

    String incoming = "";

    while (LoRa.available()) {
        incoming += (char)LoRa.read();
    }

    if (incomingLength != incoming.length()) {  // Overenie správnej dĺžky
        Serial.println("Error: message length mismatch.");
        return -1;
    }

    if (sender != 0xAA) {
        Serial.println("This message is not for me.");
        return -1;
    }

    // Výpis prijatých údajov
    
    Serial.println("Received message from control unit: " + incoming);
    
    // Spracovanie príkazu "RESTART_INERTIAL_UNIT:<azimuth>"
    if (incoming.startsWith("RESTART_INERTIAL_UNIT:")) {
        String azimuthStr = incoming.substring(22);  // Extrahovanie azimutu
        azimuthStr.trim();
        double azimuth = azimuthStr.toDouble();

        if (isnan(azimuth)) {
            Serial.println("Error: Invalid azimuth value.");
            return -1;
        }

        // Resetovanie senzora
        resetSensorPower();

        RollPitchYaw* currentRPY = readFromSensor();
        while (!currentRPY) {
            currentRPY = readFromSensor();
        }
        if (azimuth == -1) {
            Serial.println("Resetting sensor without changing azimuth...");
            yawOffset = currentRPY->yaw; // Uloženie aktuálnej hodnoty yaw
        } else {
            Serial.print("Adjusting yaw to achieve azimuth: ");
            Serial.println(azimuth);

            // Zmena yaw na dosiahnutie požadovaného azimutu
            yawOffset = adjustYawToAzimuth(azimuth, currentRPY);
        }

        // **Send acknowledgment**
        LoRa.beginPacket();
        LoRa.print("ACK:RESTART_INERTIAL_UNIT"); // Acknowledgment message
        LoRa.endPacket();
        Serial.println("Acknowledgment sent to control unit.");
        return 0;
    }

    Serial.println("Unknown command received.");
    return -1;
}
