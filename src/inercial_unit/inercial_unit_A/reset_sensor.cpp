
#include "reset_sensor.h"

#include "reset_sensor.h"
#include "transform_to_azimuth_elevation.h"

double adjustYawToAzimuth(double targetAzimuth, RollPitchYaw* currentRPY) {
    if (!currentRPY) return NAN;

    // Convert roll, pitch, and target azimuth to radians
    double roll = currentRPY->roll * M_PI / 180.0;
    double pitch = currentRPY->pitch * M_PI / 180.0;
    double azimuthRad = targetAzimuth * M_PI / 180.0;

    AzimuthElevation* computedAzEl = fromRPYtoAzimuthElevation(currentRPY);
    double sensorAzimuth = computedAzEl->azimuth;

    Serial.print("Computed sensor azimuth: ");
    Serial.println(sensorAzimuth);

    double deltaAzimuth = targetAzimuth - sensorAzimuth;

    // Normalize deltaAzimuth to range [-180, 180]
    if (deltaAzimuth > 180.0) deltaAzimuth -= 360.0;
    if (deltaAzimuth < -180.0) deltaAzimuth += 360.0;

    double cosYaw = cos(azimuthRad) / cos(pitch);
    double sinYaw = sin(azimuthRad) / cos(pitch);
    double yaw = atan2(sinYaw, cosYaw) * 180.0 / M_PI;

    // Normalize yaw to range [-180, 180]
    if (yaw > 180.0) yaw -= 360.0;
    if (yaw < -180.0) yaw += 360.0;

    Serial.print("Yaw offset adjustment: ");
    Serial.println(yaw);

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
