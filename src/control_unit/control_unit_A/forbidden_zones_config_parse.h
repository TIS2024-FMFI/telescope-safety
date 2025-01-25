
#include "common_structs.h"
#include <vector>
#include <cstring>
#include <cstdio>

// Zakázaná zóna (polygón)
using ForbiddenZone = std::vector<AzimuthElevation>;

// Všetky zakázané zóny
using ForbiddenZones = std::vector<ForbiddenZone>;

extern ForbiddenZones systemForbiddenZones;

int checkFileFormat(char* newConfiguration);
