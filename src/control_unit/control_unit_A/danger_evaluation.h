#include "forbidden_zones_config_parse.h"
#include "logs.h"
#include <Arduino.h>

// Skontroluje, či bod patrí do jednej zakázanej zóny
bool isPointInPolygon(AzimuthElevation* azimutElevation, const ForbiddenZone& polygon);

// Checks if AzimuthElevation structure is in forbidden zone
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if not in forbidden zone, -1 if in forbidden zone
int checkForbiddenZone(AzimuthElevation* azimutElevation);

// Starts alarm and disables motors
// @return 0 if success, -1 if error
int enteredForbiddenZone(AzimuthElevation* azimutElevation);

void testing_parsation_and_evaluation();

// Starts alarm
// @return 0 if success, -1 if error
int startAlarm(); //to be done

// Stops alarm
// @return 0 if success, -1 if error
int stopAlarm(); //to be done

// Disables motors
// @return 0 if success, -1 if error
int disableMotors(); //to be done

// Enables motors
// @return 0 if success, -1 if error
int enableMotors(); //to be done

// Stops alarm and enables motors
// @return 0 if success, -1 if error
int reenable(); //to be done

void setupMotors();

void setupAlarm();
