
#ifndef FORBIDDEN_ZONES_H
#define FORBIDDEN_ZONES_H

#include "common_structs.h"
#include "logs.h"
#include <vector>

// Zakázaná zóna (polygón)
using ForbiddenZone = std::vector<AzimuthElevation>;

// Všetky zakázané zóny
using ForbiddenZones = std::vector<ForbiddenZone>;

extern ForbiddenZones systemForbiddenZones;

// Skontroluje, či bod patrí do jednej zakázanej zóny
bool isPointInPolygon(AzimuthElevation* azimutElevation, const ForbiddenZone& polygon);

// Skontroluje, či bod patrí do niektorej zakázanej zóny
int checkForbiddenZone(AzimuthElevation* azimutElevation);

int enteredForbidenZone(AzimuthElevation* azimutElevation){
    writeAlarmToLog(azimutElevation);
    startAlarm();
    disableMotors();
    return 0;
}

#endif // FORBIDDEN_ZONES_H
