
#include "forbidden_zones_config_parse.h"
#include "logs.h"

// Skontroluje, či bod patrí do jednej zakázanej zóny
bool isPointInPolygon(AzimuthElevation* azimutElevation, const ForbiddenZone& polygon);

// Skontroluje, či bod patrí do niektorej zakázanej zóny
int checkForbiddenZone(AzimuthElevation* azimutElevation);

int enteredForbidenZone(AzimuthElevation* azimutElevation){
    //writeAlarmToLog(azimutElevation);
    //startAlarm();
    //disableMotors();
    return 0;
}
