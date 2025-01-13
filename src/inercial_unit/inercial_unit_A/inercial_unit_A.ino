
#include "gain_roll_pitch_yaw.h"
#include "transform_to_azimuth_elevation.h"
#include "lora_communication.h"  // Pridané: Podpora LoRa

void setup() {
    Serial.begin(115200);
    delay(100);
    initializeSensor();  // Inicializácia senzora
    initializeLoRa();    // Pridané: Inicializácia LoRa
}

unsigned long lastUpdate = 0;

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate >= 500) { // Check every 0.5 second
        lastUpdate = currentMillis;

        RollPitchYaw* roll_pitch_yaw = readFromSensor();
        AzimuthElevation* azimuth_elevation = fromRPYtoAzimuthElevation(roll_pitch_yaw);

        Serial.print(F("Roll: "));
        Serial.print(roll_pitch_yaw->roll, 1);
        Serial.print(F(" Pitch: "));
        Serial.print(roll_pitch_yaw->pitch, 1);
        Serial.print(F(" Yaw: "));
        Serial.print(roll_pitch_yaw->yaw, 1);

        if (std::isnan(azimuth_elevation->azimuth) || std::isnan(azimuth_elevation->elevation)) {
            Serial.println(F(" Azimuth/Elevation: NaN"));
        } else {
            Serial.print(F(" Azimuth: "));
            Serial.print(azimuth_elevation->azimuth, 1);
            Serial.print(F(" Elevation: "));
            Serial.println(azimuth_elevation->elevation, 1);

            // Odoslanie azimutu a elevácie cez LoRa
            while (sendToControlUnit(azimuth_elevation) != 0);
        }
    }

    // Prijímanie dát cez LoRa
    if (readFromControlUnit() == 0) {
        Serial.println(F("Processed incoming message."));
    }
}
