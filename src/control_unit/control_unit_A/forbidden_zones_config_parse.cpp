
#include "forbidden_zones_config_parse.h"

ForbiddenZones systemForbiddenZones;

// Checks if file format is correct
// also serve for parsing the forbidden zones into data structure
// @param newConfiguration from user
// @return 0 if correct, -1 if error
// @note Different error codes can be added
int checkFileFormat(const char* newConfiguration) {
    std::string_view config(newConfiguration);
    std::string_view::size_type current = 0;
    std::string_view::size_type end = 0;

    ForbiddenZones temp_zones;
    ForbiddenZone temp_zone;
    int pointCount = 0;
    bool inZone = false;

    while (current < config.size()) {
        // Nájdite koniec riadku
        end = config.find('\n', current);
        if (end == std::string_view::npos) {
            end = config.size();
        }

        // Extrahujte riadok ako `std::string_view`
        std::string_view line = config.substr(current, end - current);
        current = end + 1; // Posuňte sa na ďalší riadok

        // Odstránenie medzier na začiatku a na konci
        while (!line.empty() && (line.front() == ' ' || line.front() == '\t')) {
            line.remove_prefix(1);
        }
        while (!line.empty() && (line.back() == ' ' || line.back() == '\t')) {
            line.remove_suffix(1);
        }

        // Ignorujte komentáre a prázdne riadky
        if (line.empty() || line[0] == '#') {
            if (line.empty() && inZone) {
                // Ukončujeme aktuálnu zónu
                if (pointCount < 3) {
                    return -1; // Chyba: Zóna obsahuje menej ako 3 body
                }
                temp_zones.push_back(std::move(temp_zone));
                temp_zone.clear();
                pointCount = 0;
                inZone = false;
            }
            continue;
        }

        // Parsovanie bodu (azimuth, elevation)
        double az, el;
        if (sscanf(line.data(), "%lf %lf", &az, &el) != 2) {
            return -1; // Chyba: Neplatný formát riadku
        }

        // Pridanie bodu do aktuálnej zóny
        temp_zone.push_back({az, el});
        pointCount++;
        inZone = true;
    }

    // Ukončenie poslednej zóny
    if (inZone) {
        if (pointCount < 3) {
            return -1; // Chyba: Zóna obsahuje menej ako 3 body
        }
        temp_zones.push_back(std::move(temp_zone));
    }

    // Aktualizácia globálnych zakázaných zón
    systemForbiddenZones = std::move(temp_zones);
    return 0; // Success
}
