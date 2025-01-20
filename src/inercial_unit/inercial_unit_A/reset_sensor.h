
#ifndef RESET_SENSOR_H
#define RESET_SENSOR_H

#include "gain_roll_pitch_yaw.h"
#include "transform_to_azimuth_elevation.h"

// Deklarácia funkcií
double adjustYawToAzimuth(double targetAzimuth, RollPitchYaw* currentRPY);
void resetSensorPower();

#endif // RESET_SENSOR_H
