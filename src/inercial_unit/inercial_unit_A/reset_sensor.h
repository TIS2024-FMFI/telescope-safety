
#ifndef RESET_SENSOR_H
#define RESET_SENSOR_H

#include "gain_roll_pitch_yaw.h"
#include "transform_to_azimuth_elevation.h"

// adjusting current yaw to such value that given calibration azimuth from user is given
double adjustYawToAzimuth(double targetAzimuth, RollPitchYaw* currentRPY);

// resets sensor by interrupting the power supply for 5 seconds
void resetSensorPower();

#endif // RESET_SENSOR_H
