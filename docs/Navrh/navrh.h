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

enum ChangeType{
    FORBIDDEN_ZONE_CHANGED,
    LOG_FREQUENCY_CHANGED,
    RESTRT
};


// Inercialna jednotka

// Reads data from sensor and stores them in RollPitchYaw structure
// @return pointer to RollPitchYaw structure
RollPitchYaw* readFromSensor();

// Converts RollPitchYaw structure to AzimuthElevation structure
// @param rollPitchYaw pointer to RollPitchYaw structure
// @return pointer to AzimuthElevation structure
AzimuthElevation* fromRPYtoAzimuthElevation(RollPitchYaw* rollPitchYaw);

// Sends AzimuthElevation structure to control unit
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if success, -1 if error
int sendToControlUnit(AzimuthElevation* azimutElevation);

// Receiving message from control unit to reset
// @return 0 if success, -1 if error
int readFromControlUnit();

// Riadiaca jednotka

// Reads data from inertial unit and stores them in AzimuthElevation structure
// @return pointer to AzimuthElevation structure
AzimuthElevation* readFromInertialUnit();

// Checks if AzimuthElevation structure is in forbidden zone
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if not in forbidden zone, -1 if in forbidden zone
int checkForbiddenZone(AzimuthElevation* azimutElevation);

// Sends AzimuthElevation structure to all conected clients via WS
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if success, -1 if error
int sendToClients(AzimuthElevation* azimutElevation);

// Dispalays AzimuthElevation structure on screen
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if success, -1 if error
int displayAL(AzimuthElevation* azimutElevation);

// Writes AzimuthElevation structure to log file, with timestamp in CSV format
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if success, -1 if error
int writeALtoLog(AzimuthElevation* azimutElevation);

// Checks if file format is correct
// @param newConfiguration from user
// @return 0 if correct, -1 if error
// @note Different error codes can be added
int checkFileFormat(char* newConfiguration);

// Writes new forbidden zones configuration
// @param azimutElevation pointer to AzimuthElevation structure array
// @param numberOfForbiddenZones number of forbidden zones
// @param forbiddenZonesSizes array of sizes of forbidden zones
// @return 0 if success, -1 if error
// The format of the file is as follows:
// 1. line: Azimuth Elevation
// 2. line: Azimuth Elevation
// 3. line: Azimuth Elevation
// ...
// n. line: \n (empty line)
// n+1. line: Azimuth Elevation
// ...
// Azimuth and Elevation are separated by space
// Each Azimuth Elevation is separated by new line
// Each forbidden zone is separated by empty line
// Each forbidden zone contains at least 3 points
int writeNewForbiddenConfig(AzimuthElevation* azimutElevation, int numberOfForbiddenZones, int* forbiddenZonesSizes);


// Reads forbidden zones configuration
// @param azimutElevation pointer to AzimuthElevation structure array to store forbidden zones
// @param forbiddenZonesSizes pointer to array of sizes of forbidden zones
// @return number of forbiden zones, -1 if error
int loadForbiddenZones(AzimuthElevation* azimutElevation, int* forbiddenZonesSizes);

// Writes new log frequency configuration
// @param logFrequency new log frequency
// @param clientUpdateFrequency new fryquency of client updates via WS
// @return 0 if success, -1 if error
int writeNewLogFrequency(int logFrequency, int clientUpdateFrequency);

// Writes change of configuration to log file, with timestamp in CSV format
// @param changeType type of change
// @return 0 if success, -1 if error
int writeChangeToLog(ChangeType changeType);

// Writes in witch position the telescope entered the forbidden zone to log file, with timestamp in CSV format
// @param azimutElevation pointer to AzimuthElevation structure
int writeAlarmToLog(AzimuthElevation* azimutElevation);

// Starts alarm
// @return 0 if success, -1 if error
int startAlarm();

// Stops alarm
// @return 0 if success, -1 if error
int stopAlarm();

// Disables motors
// @return 0 if success, -1 if error
int disableMotors();

// Enables motors
// @return 0 if success, -1 if error
int enableMotors();

// Starts alarm and disables motors
// @return 0 if success, -1 if error
int enteredForbidenZone(AzimuthElevation* azimutElevation);

// Stops alarm and enables motors
// @return 0 if success, -1 if error
int reenable();

// Retrievs real time from Ethernet
// @return time in seconds
time_t getRealTime();

// Retrievs time from system
// @return time in seconds
time_t getTime();

// Restarts system
// @return 0 if success, -1 if error
int restart();              // ?

// Restarts inertial unit
// @return 0 if success, -1 if error
int restartInertialUnit();  // ?




int enteredForbidenZone(AzimuthElevation* azimutElevation){
    writeAlarmToLog(azimutElevation);
    startAlarm();
    disableMotors();
    return 0;
}

int reenable(){
    stopAlarm();
    enableMotors();
    return 0;
}

