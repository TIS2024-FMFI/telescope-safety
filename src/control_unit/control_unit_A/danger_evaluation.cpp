#include "danger_evaluation.h"

const int RELE = 27;

int enteredForbiddenZone(AzimuthElevation* azimutElevation){
    //writeAlarmToLog(azimutElevation);
    //startAlarm();
    //disableMotors();
    return 0;
}

int reenable(){
    stopAlarm();
    enableMotors();
    return 0;
}

// Skontroluje, či bod patrí do jedného polygónu (Ray-Casting algoritmus)
bool isPointInPolygon(AzimuthElevation* azimutElevation, const ForbiddenZone& polygon) {
    int intersectCount = 0;
    int n = polygon.size();

    for (int i = 0; i < n; ++i) {
        const AzimuthElevation& v1 = polygon[i];
        const AzimuthElevation& v2 = polygon[(i + 1) % n]; // Posledný bod spája s prvým

        // Kontrola, či priamka pretína lúč
        if (((v1.elevation > azimutElevation->elevation) != (v2.elevation > azimutElevation->elevation)) &&
            (azimutElevation->azimuth < (v2.azimuth - v1.azimuth) * (azimutElevation->elevation - v1.elevation) / (v2.elevation - v1.elevation) + v1.azimuth)) {
            intersectCount++;
        }
    }

    return (intersectCount % 2) == 1; // Nepárny počet prienikov = vo vnútri
}

// Skontroluje, či bod patrí do niektorej zakázanej zóny
int checkForbiddenZone(AzimuthElevation* azimutElevation) {
  for (const auto& zone : settings.systemForbiddenZones) {
    if (isPointInPolygon(azimutElevation, zone)) {
      enteredForbiddenZone(azimutElevation);
      return -1;
    }
  }
  return 0;
}

void testing_parsation_and_evaluation() {
    // Test Configuration Parsing
    Serial.print("Testing configuration parsing...");
    bool control_danger_flag = false;
    const char* testConfig =
        "# Zone 1\n"
        "10.0 20.0\n"
        "15.0 25.0\n"
        "20.0 15.0\n"
        "\n"
        "# Zone 2\n"
        "30.0 40.0\n"
        "35.0 45.0\n"
        "40.0 35.0\n";

    int configResult = checkFileFormat(testConfig);
    if (configResult == 0) {
        Serial.print("Configuration parsed successfully!\n");
        control_danger_flag = true;
    } else {
        if (configResult == -1) {
            Serial.print("Configuration parsing failed! Error: one of the zones has fewer than three points.\n");
        } else {
            Serial.print("Configuration parsing failed! Error: one of the lines has bad format. Check if there is just one space between azimuth and elevation and if there are only digits or dot.\n");
        }
    }

    // Test Danger Evaluation
    if (control_danger_flag) {
        Serial.print("Testing danger evaluation...\n");
        AzimuthElevation testPoint = {35, 44};
        int dangerResult = checkForbiddenZone(&testPoint);
        if (dangerResult == -1) {
            Serial.print("Point is in a forbidden zone!\n");
        } else {
            Serial.print("Point is safe.\n");
        }
    }
}

void setupMotors(){
  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, 1);
}

int disableMotors(){
  if (settings.rele){
    digitalWrite(RELE, 0);
    return 0;
  }
  return 1;
}

int enableMotors(){
  digitalWrite(RELE, 1);
  return 0;
}