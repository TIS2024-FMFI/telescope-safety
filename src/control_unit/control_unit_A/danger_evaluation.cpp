
#include "danger_evaluation.h"

// Skontroluje, či bod patrí do jedného polygónu (Ray-Casting algoritmus)
bool isPointInPolygon(const Point& point, const ForbiddenZone& polygon) {
    int intersectCount = 0;
    int n = polygon.size();

    for (int i = 0; i < n; ++i) {
        const Point& v1 = polygon[i];
        const Point& v2 = polygon[(i + 1) % n]; // Posledný bod spája s prvým

        // Kontrola, či priamka pretína lúč
        if (((v1.y > point.y) != (v2.y > point.y)) &&
            (point.x < (v2.x - v1.x) * (point.y - v1.y) / (v2.y - v1.y) + v1.x)) {
            intersectCount++;
        }
    }

    return (intersectCount % 2) == 1; // Nepárny počet prienikov = vo vnútri
}

// Skontroluje, či bod patrí do niektorej zakázanej zóny
int checkForbiddenZone(AzimuthElevation* azimutElevation) {
    for (const auto& zone : systemForbiddenZones) {
        if (isPointInPolygon(azimutElevation, zone)) {
            enteredForibiddenZone(azimutElevation);
            return -1;
        }
    }
    return 0;
}
