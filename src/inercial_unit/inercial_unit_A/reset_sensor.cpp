
#include "reset_sensor.h"

double adjustYawToAzimuth(double targetAzimuth, RollPitchYaw* currentRPY) {

    // Prepočet roll a pitch na radiány
    double roll = currentRPY->roll * M_PI / 180.0;
    double pitch = currentRPY->pitch * M_PI / 180.0;

    // Prepočet cieľového azimutu na radiány
    double azimuthRad = targetAzimuth * M_PI / 180.0;

    // Prepočet na yaw
    double cosYaw = cos(azimuthRad) / cos(pitch);
    double sinYaw = sin(azimuthRad) / cos(pitch);

    // Výpočet yaw
    double yaw = atan2(sinYaw, cosYaw) * 180.0 / M_PI;

    // Normalizácia yaw na rozsah [-180, 180]
    if (yaw > 180.0) yaw -= 360.0;
    if (yaw < -180.0) yaw += 360.0;

    return yaw;
}

void resetSensorPower() {
    Serial.println("Resetting sensor power...");
    digitalWrite(SENSOR_POWER_PIN, LOW);  // Vypnutie napájania
    delay(5000);                          // Čaká 5 sekúnd
    digitalWrite(SENSOR_POWER_PIN, HIGH); // Zapnutie napájania
    delay(1000);                          // Čas na reštart senzora
    Serial.println("Sensor power reset complete.");
    initializeSensor();
}
