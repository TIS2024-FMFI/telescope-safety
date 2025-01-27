
#include "danger_evaluation.h"

int enteredForbiddenZone(AzimuthElevation* azimutElevation){
    //writeAlarmToLog(azimutElevation);
    //startAlarm();
    //disableMotors();
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
    for (const auto& zone : systemForbiddenZones) {
        if (isPointInPolygon(azimutElevation, zone)) {
            enteredForbiddenZone(azimutElevation);
            return -1;
        }
    }
    return 0;
}
