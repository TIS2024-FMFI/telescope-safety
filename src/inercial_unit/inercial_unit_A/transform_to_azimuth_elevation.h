
#ifndef CALCULATE_AZIMUTH_ELEVATION_H
#define CALCULATE_AZIMUTH_ELEVATION_H

#include <cmath>
#include "gain_roll_pitch_yaw.h"

extern double R_correction[4][4];

extern double azimuthOffset;

struct AzimuthElevation {
    double azimuth;
    double elevation;
};

// Converts RollPitchYaw structure to AzimuthElevation structure
// @param rollPitchYaw pointer to RollPitchYaw structure
// @return pointer to AzimuthElevation structure
AzimuthElevation* fromRPYtoAzimuthElevation(RollPitchYaw* rollPitchYaw);

#endif // CALCULATE_AZIMUTH_ELEVATION_H
