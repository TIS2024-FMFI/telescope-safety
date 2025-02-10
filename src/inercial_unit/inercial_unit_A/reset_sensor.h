
#ifndef RESET_SENSOR_H
#define RESET_SENSOR_H

#include "gain_roll_pitch_yaw.h"
#include "transform_to_azimuth_elevation.h"

// resets sensor by interrupting the power supply for 5 seconds
void resetSensorPower();

#endif // RESET_SENSOR_H
