#ifndef COMMON_STRUCTS_H
#define COMMON_STRUCTS_H

#include <vector>



struct AzimuthElevation {
    double azimuth;
    double elevation;
};

struct DegreesMinutesSeconds {
    int degrees;
    int minutes;
    int seconds;
};

// Zakázaná zóna (polygón)
using ForbiddenZone = std::vector<AzimuthElevation>;

// Všetky zakázané zóny
using ForbiddenZones = std::vector<ForbiddenZone>;

enum ChangeType {
    FORBIDDEN_ZONE_CHANGED,
    LOG_FREQUENCY_AND_ALARM_TYPE_CHANGED,
    RESTART
};

struct Settings {
  bool rele;
  bool alarm;
  bool logging;
  unsigned int log_frequency;
  unsigned int update_frequency;
  ForbiddenZones systemForbiddenZones;
};

extern Settings settings;
extern int TransformMatrix[3][3];

#endif