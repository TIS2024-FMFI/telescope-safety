
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
// @return 0 if correct, -1 if error
// @note Different error codes can be added
int checkFileFormat(char* newConfiguration);

void sortZoneClockwise(ForbiddenZone& zone);

double calculateAngle(const AzimuthElevation& point, const AzimuthElevation& centroid);