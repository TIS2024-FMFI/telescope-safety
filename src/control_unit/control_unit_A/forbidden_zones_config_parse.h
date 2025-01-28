
#include "common_structs.h"
#include <vector>
#include <string_view>
#include <algorithm>
#include <cmath>
#include <cstdio>

// Zakázaná zóna (polygón)
using ForbiddenZone = std::vector<AzimuthElevation>;

// Všetky zakázané zóny
using ForbiddenZones = std::vector<ForbiddenZone>;

extern ForbiddenZones systemForbiddenZones;

// Checks if file format is correct
// also serve for parsing the forbidden zones into data structure
// @param newConfiguration from user
// @return 0 if correct, -1 if zone has fewer than three points, -2 if line has wrong format
// @note Different error codes can be added
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
int checkFileFormat(const char* newConfiguration);

void sortZoneClockwise(ForbiddenZone& zone);

double calculateAngle(const AzimuthElevation& point, const AzimuthElevation& centroid);