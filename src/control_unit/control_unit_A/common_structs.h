#ifndef COMMON_STRUCTS
#define COMMON_STRUCTS

struct AzimuthElevation {
    double azimuth;
    double elevation;
};

enum ChangeType{
    FORBIDDEN_ZONE_CHANGED,
    LOG_FREQUENCY_ALARM_CHANGED,
    RESTRT
};
#endif