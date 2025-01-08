
#ifndef CALCULATE_AZIMUTH_ELEVATION_H
#define CALCULATE_AZIMUTH_ELEVATION_H

#include <cmath>
#include "gain_roll_pitch_yaw.h"

struct AzimuthElevation {
    double azimuth;
    double elevation;
};

AzimuthElevation* fromRPYtoAzimuthElevation(RollPitchYaw* rollPitchYaw);

#endif // CALCULATE_AZIMUTH_ELEVATION_H
