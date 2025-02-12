#ifndef LOGS_H
#define LOGS_H

#include "common_structs.h"
#include "config_parse.h"
#include "time.h"

extern const int _MISO;
extern const int _MOSI;
extern const int _CS;
extern const int _SCK;

extern const char *logConfigFilePath;
extern const char *logCollisionFilePath;
extern const char *logFilePathPrefix;
extern const char* ConfigFilePath;
extern const char* forbiddenConfigFilePath;
extern const char* matrixFilePath;


// Writes header to csv file if its start of the file
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
int writeConfigAlarmAndIntervals(Settings settings);

// Writes new forbidden zones configuration
// @param zones as written by user as const char* type
// @return 0 if success, -1 if error
int writeNewForbiddenConfig(const char* zones);

// Reads forbidden zones configuration
// stores forbidden zones to global variable "settings.systemForbiddenZones"
// Reads log frequency configuration
// stores log frequency to global variable "settings.log_frequency"
// Reads alarm type configuration
// stores alarm type configuration to global variables "settings.alarm" and "settings.rele"
// Reads logging configuration
// stores logging configuration to global variable "settings.logging"
// Reads update frequency configuration
// stores update frequency configuration to global variable "settings.update_frequency"
// @return 0 if success, -1 if error
int loadSettings(); //to be done

// Loads file from SD into memory
// @param filePath path to the file to be loaded
// @return pointer to loaded file (file ist dinamicaly alocated)
char* loadFile(const char* filePath);

// sets up pins for SD
void setupSD();

// Save transform matrix to file
// @return 0 if success, -1 if error
int saveMatrix();

uint32_t getFreeSpace();

bool parseLogFileDate(const String &filename, int &year, int &month, int &day);

unsigned long dateToTimestamp(int year, int month, int day);

void deleteOldLogs();

void manageSDSpace();

unsigned long convertTimeToEpoch(const Time &t);
#endif