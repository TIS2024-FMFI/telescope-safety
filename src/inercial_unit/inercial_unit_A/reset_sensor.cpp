
#include "reset_sensor.h"

#include "reset_sensor.h"
#include "transform_to_azimuth_elevation.h"

void resetSensorPower() {
    Serial.println("Resetting sensor power...");
    digitalWrite(SENSOR_POWER_PIN, LOW);  // Vypnutie napájania
    delay(5000);                          // Čaká 5 sekúnd
    digitalWrite(SENSOR_POWER_PIN, HIGH); // Zapnutie napájania
    delay(1000);                          // Čas na reštart senzora
    Serial.println("Sensor power reset complete.");
    initializeSensor();
}
