#include "common_structs.h"
#include "time.h"

extern const int _MISO;
extern const int _MOSI;
extern const int _CS;
extern const int _SCK;


int writeHeaderIfNeeded(String filename, const char *header);

// Writes AzimuthElevation structure to log file, with timestamp in CSV format
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if success, -1 if error
int writeAEtoLog(AzimuthElevation *azimuthElevation);

// Writes change of configuration to log file, with timestamp in CSV format
// @param changeType type of change
// @return 0 if success, -1 if error
int writeChangeToLog(ChangeType changeType, const char *ip); //to be changed

// Writes in witch position the telescope entered the forbidden zone to log file, with timestamp in CSV format
// @param azimutElevation pointer to AzimuthElevation structure
int writeAlarmToLog(AzimuthElevation *azimuthElevation);

// Writes new log frequency configuration and alarm settings
// @param data has info on alarm and storing intervals settings
// @return 0 if success, -1 if error
int writeConfigAlarmAndIntervals(const char* data);

// Writes new forbidden zones configuration
// @param zones as written by user as const char* type
// @return 0 if success, -1 if error
int writeNewForbiddenConfig(const char* zones);

// Reads forbidden zones configuration
// stores configuration azimuth and elvation values from user to global variable
// @return number of forbiden zones, -1 if error
int loadForbiddenZones(); //to be done

// Reads log frequency configuration
// Reads alarm type configuration
// @param logFrequency pointer to store log frequency
// @param clientUpdateFrequency pointer to store frequency of client updates via WS
// @param alarm pointer to store if alarm should start audiovisual signal when triggered
// @param motors pointer to store if motors should be disabled when alarm is triggered
// @param log_indicator indicates if logging is turned on or off
// @return 0 if success, -1 if error
int loadConfigAlarmAndIntervals(unsigned int* logFrequency, unsigned int* clientUpdateFrequency,bool* alarm, bool* motors,bool* log_indicator); //to be done

char* loadFile(const char* filePath);

void setupSD();