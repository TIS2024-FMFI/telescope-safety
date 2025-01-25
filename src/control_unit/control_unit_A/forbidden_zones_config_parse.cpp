
#include "forbidden_zones_config_parse.h"

ForbiddenZones systemForbiddenZones;

// Checks if file format is correct
// also serve for parsing the forbidden zones into data structure
// @param newConfiguration from user
// @return 0 if correct, -1 if error
// @note Different error codes can be added
int checkFileFormat(char* newConfiguration) {
  char* current = newConfiguration;
  char* end = newConfiguration;
  int pointCount = 0;
  bool inZone = false;
  ForbiddenZones temp_zones;
  ForbiddenZone temp_zone;

  while (*current != '\0') {
    while (*end != '\0' && *end != '\n') {
      end++;
    }

  size_t length = end - current;
  char line[length + 1];
  strncpy(line, current, length);
  line[length] = '\0';

  if (*end == '\n') {
    end++;
  }
  current = end;

  if (line[0] == '#') {
    continue;
  }

  if (strlen(line) == 0) {
    if (inZone && pointCount < 3) {
      return -1;
    }
    if(inZone){
      temp_zones.push_back(temp_zone);
      temp_zone.clear();
    }
    pointCount = 0;
    inZone = false;
  } else {
    inZone = true;

    double az, el;
    if (sscanf(line, "%f %f", &az, &el) != 2) {
      return -1;
    }

    AzimuthElevation temp_az_el;
    temp_az_el.azimuth=az;
    temp_az_el.elevation=el;
    temp_zone.push_back(temp_az_el);

    pointCount++;
    }
  }

  if (inZone && pointCount < 3) {
    return -1;
  }

  if(inZone){
    temp_zones.push_back(temp_zone);
  }

  systemForbiddenZones=temp_zones;

  return 0;
}
