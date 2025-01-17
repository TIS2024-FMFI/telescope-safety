
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
    byte incomingLength = LoRa.read();   // Dĺžka správy

    String incoming = "";

    while (LoRa.available()) {
        incoming += (char)LoRa.read();
    }

    if (incomingLength != incoming.length()) {  // Overenie správnej dĺžky
        Serial.println("Error: message length mismatch.");
        return -1;
    }

    // Výpis prijatých údajov
    Serial.println("Received message from control unit: " + incoming);
    return 0;
}
