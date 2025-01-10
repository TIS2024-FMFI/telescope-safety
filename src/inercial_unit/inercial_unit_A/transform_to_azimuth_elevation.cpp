
#include "transform_to_azimuth_elevation.h"

AzimuthElevation* fromRPYtoAzimuthElevation(RollPitchYaw* rollPitchYaw) {
    static AzimuthElevation result = { NAN, NAN };

    // Kontrola na NaN pre vstupné hodnoty
    if (!rollPitchYaw || std::isnan(rollPitchYaw->roll) || std::isnan(rollPitchYaw->pitch) || std::isnan(rollPitchYaw->yaw)) {
        return &result;
    }

    // Prepočet na radiány
    double roll = rollPitchYaw->roll * M_PI / 180.0;
    double pitch = rollPitchYaw->pitch * M_PI / 180.0;
    double yaw = rollPitchYaw->yaw * M_PI / 180.0;

    // Rotačné matice pre yaw, pitch a roll
    double R_yaw[3][3] = {
        {cos(yaw), -sin(yaw), 0},
        {sin(yaw), cos(yaw), 0},
        {0, 0, 1}
    };

    double R_pitch[3][3] = {
        {cos(pitch), 0, sin(pitch)},
        {0, 1, 0},
        {-sin(pitch), 0, cos(pitch)}
    };

    double R_roll[3][3] = {
        {1, 0, 0},
        {0, cos(roll), -sin(roll)},
        {0, sin(roll), cos(roll)}
    };

    // Kombinácia rotačných matíc: R = R_yaw * R_pitch * R_roll
    double R_temp[3][3] = {0};
    double R[3][3] = {0};

    // Najprv R_yaw * R_pitch
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                R_temp[i][j] += R_yaw[i][k] * R_pitch[k][j];
            }
        }
    }

    // Potom výsledok * R_roll
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                R[i][j] += R_temp[i][k] * R_roll[k][j];
            }
        }
    }

    // Aplikácia rotačnej matice na referenčný vektor [1, 0, 0]
    double x = R[0][0];
    double y = R[1][0];
    double z = R[2][0];

    // Výpočet azimutu
    result.azimuth = atan2(y, x) * 180.0 / M_PI;
    if (result.azimuth < 0) result.azimuth += 360.0;

    // Výpočet elevácie
    result.elevation = atan2(z, sqrt(x * x + y * y)) * 180.0 / M_PI;

    return &result;
}
