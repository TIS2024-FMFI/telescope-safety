
#ifndef GAIN_ROLL_PITCH_YAW_H
#define GAIN_ROLL_PITCH_YAW_H

#include "ICM_20948.h"
#include <Wire.h>
#include "SparkFun_ISM330DHCX.h"

#define SERIAL_PORT Serial
#define SPI_PORT SPI
#define CS_PIN 2
#define WIRE_PORT Wire
#define SENSOR_POWER_PIN 15

extern uint8_t AD0_VAL;

struct RollPitchYaw {
    double roll;
    double pitch;
    double yaw;
};

extern double yawOffset;

void initializeSensor();

typedef enum {
    SENSOR_ICM20948,
    SENSOR_ISM330DHCX,
    SENSOR_UNKNOWN
} SensorType;

extern SensorType detectedSensor;

// Reads data from sensor and stores them in RollPitchYaw structure
// @return pointer to RollPitchYaw structure
RollPitchYaw* readFromSensor();

void Mahony_update(float ax, float ay, float az, float gx, float gy, float gz, float deltat);

#endif // GAIN_ROLL_PITCH_YAW_H
