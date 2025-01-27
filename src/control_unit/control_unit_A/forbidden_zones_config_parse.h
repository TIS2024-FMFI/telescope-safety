
#include "common_structs.h"
#include <vector>
#include <string_view>
#include <algorithm>
#include <cmath>

// Zakázaná zóna (polygón)
using ForbiddenZone = std::vector<AzimuthElevation>;

// Všetky zakázané zóny
using ForbiddenZones = std::vector<ForbiddenZone>;

extern ForbiddenZones systemForbiddenZones;

int checkFileFormat(char* newConfiguration);
