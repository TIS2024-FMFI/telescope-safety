#include <cstdint>
#include <time.h>

struct RollPitchYaw{
    int8_t roll;
    int8_t pitch;
    int8_t yaw;
};

struct AzimuthElevation{
    int8_t azimuth;
    int8_t elevation;
};


// Inercialna jednotka
RollPitchYaw* readFromSensor();
AzimuthElevation* fromRPYtoAzimuthElevation(RollPitchYaw* rollPitchYaw);
int sendToControlUnit(AzimuthElevation* azimutElevation);


// Riadiaca jednotka
AzimuthElevation* readFromInertialUnit();
int checkForbiddenZone(AzimuthElevation* azimutElevation);
int sendToClients(AzimuthElevation* azimutElevation);
int displayAL(AzimuthElevation* azimutElevation);
int writeALtoLog(AzimuthElevation* azimutElevation);
int writeChangeToLog();
int writeAlarmToLog(AzimuthElevation* azimutElevation);
int startAlarm();
int disableAlarm();
int disableMotors();
int enableMotors();
int enteredForbidenZone(AzimuthElevation* azimutElevation);
int reenable();
time_t getTime();
int restart();              // ?
int restartInertialUnit();  // ?




int enteredForbidenZone(AzimuthElevation* azimutElevation){
    writeAlarmToLog(azimutElevation);
    startAlarm();
    disableMotors();
    return 0;
}

int reenable(){
    disableAlarm();
    enableMotors();
    return 0;
}

