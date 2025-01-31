
#ifndef GAIN_ROLL_PITCH_YAW_H
#define GAIN_ROLL_PITCH_YAW_H

#include "ICM_20948.h"

#define SERIAL_PORT Serial
#define SPI_PORT SPI
#define CS_PIN 2
#define WIRE_PORT Wire
#define AD0_VAL 0
#define SENSOR_POWER_PIN 15

struct RollPitchYaw {
    double roll;
    double pitch;
    double yaw;
};

extern double yawOffset;

void initializeSensor();

// Reads data from sensor and stores them in RollPitchYaw structure
// @return pointer to RollPitchYaw structure
RollPitchYaw* readFromSensor();

#endif // GAIN_ROLL_PITCH_YAW_H
