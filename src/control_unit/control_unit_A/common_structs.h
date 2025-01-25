#ifndef COMMON_STRUCTS
#define COMMON_STRUCTS

struct AzimuthElevation {
    float azimuth;
    float elevation;
};

enum ChangeType {
    FORBIDDEN_ZONE_CHANGED,
    LOG_FREQUENCY_CHANGED,
    ALARM_TYPE_CHANGED,
    RESTART
};
#endif