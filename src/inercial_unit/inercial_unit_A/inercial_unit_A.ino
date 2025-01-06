
#include "gain_roll_pitch_yaw.h"

void setup() {
    Serial.begin(115200);
    delay(100);
    initializeSensor();
}

unsigned long lastUpdate = 0;

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate >= 500) { // Check every 0.5 second
        lastUpdate = currentMillis;

        RollPitchYaw* roll_pitch_yaw = readFromSensor();

        Serial.print(F("Roll: "));
        Serial.print(roll_pitch_yaw->roll, 1);
        Serial.print(F(" Pitch: "));
        Serial.print(roll_pitch_yaw->pitch, 1);
        Serial.print(F(" Yaw: "));
        Serial.println(roll_pitch_yaw->yaw, 1);
    }
}
