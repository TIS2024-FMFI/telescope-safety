
#ifndef GAIN_ROLL_PITCH_YAW_H
#define GAIN_ROLL_PITCH_YAW_H

#include "ICM_20948.h"

#define SERIAL_PORT Serial
#define SPI_PORT SPI
#define CS_PIN 2
#define WIRE_PORT Wire
#define AD0_VAL 0

struct RollPitchYaw {
    double roll;
    double pitch;
    double yaw;
};

void initializeSensor();
RollPitchYaw* readFromSensor();

#endif // GAIN_ROLL_PITCH_YAW_H
