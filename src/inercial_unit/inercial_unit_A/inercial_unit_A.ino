
#include "gain_roll_pitch_yaw.h"
#include "transform_to_azimuth_elevation.h"
#include "lora_communication.h"

void setup() {
    Serial.begin(115200);
    pinMode(SENSOR_POWER_PIN, OUTPUT);
    digitalWrite(SENSOR_POWER_PIN, HIGH);
    delay(100);
    initializeSensor();  // Inicializácia senzora
    initializeLoRa();
}

unsigned long lastUpdate = 0;

void loop() {
    unsigned long currentMillis = millis();

    RollPitchYaw* roll_pitch_yaw = readFromSensor();
    AzimuthElevation* azimuth_elevation = fromRPYtoAzimuthElevation(roll_pitch_yaw);

    if (currentMillis - lastUpdate >= 500) { // Check every 0.5 second
        lastUpdate = currentMillis;

        // Odoslanie azimutu a elevácie cez LoRa
        while (sendToControlUnit(azimuth_elevation) != 0);
        }

    // Prijímanie dát cez LoRa
    if (readFromControlUnit() == 0) {
        Serial.println(F("Processed incoming message."));
    }
}
