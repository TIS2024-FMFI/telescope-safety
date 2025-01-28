#include "common_structs.h"

extern const int _MISO;
extern const int _MOSI;
extern const int _CS;
extern const int _SCK;


int writeHeaderIfNeeded(const char *filename, const char *header);

// Writes AzimuthElevation structure to log file, with timestamp in CSV format
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if success, -1 if error
int writeAEtoLog(AzimuthElevation *azimuthElevation);

int writeChangeToLog(ChangeType changeType, const char *ip);
int writeAlarmToLog(AzimuthElevation *azimuthElevation);
int writeConfigAlarmAndIntervals(const char* data);
int writeConfigZones(const char* zones);
char* loadFile(const char* filePath);
void setupSD();