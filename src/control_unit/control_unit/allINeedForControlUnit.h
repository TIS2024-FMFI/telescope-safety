#include <time.h>
#include <Arduino.h>

struct AzimuthElevation{
    float azimuth;
    float elevation;
};

enum ChangeType{
    FORBIDDEN_ZONE_CHANGED,
    LOG_FREQUENCY_CHANGED,
    ALARM_TYPE_CHANGED,
    RESTART
};


// Sends AzimuthElevation structure to all conected clients via WS
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if success, -1 if error
int sendToClients(AzimuthElevation* azimutElevation);


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
// Lines starting with # are ignored
int writeNewForbiddenConfig(String newConfiguration);



// Writes new log frequency configuration
// @param logFrequency new log frequency
// @param clientUpdateFrequency new fryquency of client updates via WS
// @return 0 if success, -1 if error
int writeNewLogFrequency(int logFrequency, int clientUpdateFrequency);


// Writes new alarm type configuration
// @param alarm indicates if alarm should start audiovisual signal when triggered
// @param motors indicates if motors should be disabled when alarm is triggered
// @return 0 if success, -1 if error
int writeNewAlarmType(bool alarm, bool motors);


// Writes change of configuration to log file, with timestamp in CSV format
// @param changeType type of change
// @return 0 if success, -1 if error
int writeChangeToLog(ChangeType changeType);

// Writes in witch position the telescope entered the forbidden zone to log file, with timestamp in CSV format
// @param azimutElevation pointer to AzimuthElevation structure
int writeAlarmToLog(AzimuthElevation* azimutElevation);


// Retrievs real time from Ethernet
// @return time in seconds
time_t getRealTime();

// Retrievs time from system
// @return time in seconds
time_t getTime();

// Restarts system
// @return 0 if success, -1 if error
int restart();

int downloadEventLogs();
int downloadLogs(String from, String to);
