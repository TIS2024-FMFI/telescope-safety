
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

    String outgoing = String("ACK:RESTART_INERTIAL_UNIT");

    LoRa.beginPacket();
    LoRa.write(localAddress);
    LoRa.write(outgoing.length());
    LoRa.print(outgoing);
    LoRa.endPacket();
    Serial.println("Acknowledgment sent.");

    // Rozdelenie viacerých príkazov pomocou "|"
    char *command = strtok((char*)incoming.c_str(), "|");

    while (command != nullptr) {
        String cmdStr = String(command);

        if (cmdStr.startsWith("SET_CALIBRATION_MATRIX:")) {
            double newMatrix[3][3];
            sscanf(cmdStr.c_str(), "SET_CALIBRATION_MATRIX:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
                   &newMatrix[0][0], &newMatrix[0][1], &newMatrix[0][2],
                   &newMatrix[1][0], &newMatrix[1][1], &newMatrix[1][2],
                   &newMatrix[2][0], &newMatrix[2][1], &newMatrix[2][2]);

            setCorrectionMatrix(newMatrix);  
            Serial.println("Calibration matrix updated.");
        }

        if (cmdStr.startsWith("RESTART_INERTIAL_UNIT:")) {
            String azimuthStr = cmdStr.substring(22);
            azimuthStr.trim();
            double azimuth = azimuthStr.toDouble();

            if (isnan(azimuth) || azimuth == -1) {
                Serial.println("Resetting sensor without changing azimuth...");
                resetSensorPower();
                RollPitchYaw* currentRPY = readFromSensor();
                while (!currentRPY) {
                    currentRPY = readFromSensor();
                }
                azimuthOffset = fromRPYtoAzimuthElevation(currentRPY)->azimuth; // Uloženie aktuálnej hodnoty yaw
            } else {
                Serial.print("Adjusting yaw to achieve azimuth: ");
                Serial.println(azimuth);

                // Reset senzora a nastavenie azimuthOffset
                resetSensorPower();
                azimuthOffset = azimuth;
            }
        }

        command = strtok(nullptr, "|");  // Prechádzame na ďalší príkaz
    }

    return 0;
}

void setCorrectionMatrix(double newMatrix[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            R_correction[i][j] = newMatrix[i][j];
        }
    }
}
