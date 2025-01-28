
#include "forbidden_zones_config_parse.h"

ForbiddenZones systemForbiddenZones;

int checkFileFormat(const char* newConfiguration) {
    std::string_view config(newConfiguration);
    std::string_view::size_type current = 0;
    std::string_view::size_type end = 0;

    ForbiddenZones temp_zones;
    ForbiddenZone temp_zone;
    int pointCount = 0;
    bool inZone = false;

    while (current < config.size()) {
        // Find the end of the line
        end = config.find('\n', current);
        if (end == std::string_view::npos) {
            end = config.size();
        }

        // Extract the line as `std::string_view`
        std::string_view line = config.substr(current, end - current);
        current = end + 1; // Move to the next line

        // Trim whitespace from the start and end
        while (!line.empty() && (line.front() == ' ' || line.front() == '\t')) {
            line.remove_prefix(1);
        }
        while (!line.empty() && (line.back() == ' ' || line.back() == '\t')) {
            line.remove_suffix(1);
        }

        // Ignore comments and empty lines
        if (line.empty() || line[0] == '#') {
            if (line.empty() && inZone) {
                // Finish the current zone
                if (pointCount < 3) {
                    return -1; // Error: Zone contains fewer than 3 points
                }

                // Sort the current zone clockwise before storing it
                // Ray-casting algorithm in danger evaluation requires this sort
                sortZoneClockwise(temp_zone);

                temp_zones.push_back(std::move(temp_zone));
                temp_zone.clear();
                pointCount = 0;
                inZone = false;
            }
            continue;
        }

        // Parse a point (azimuth, elevation)
        double az, el;
        if (sscanf(line.data(), "%lf %lf", &az, &el) != 2) {
            return -2; // Error: Invalid line format
        }

        // Add the point to the current zone
        temp_zone.push_back({az, el});
        pointCount++;
        inZone = true;
    }

    // Finalize the last zone
    if (inZone) {
        if (pointCount < 3) {
            return -1; // Error: Zone contains fewer than 3 points
        }

        // Sort the current zone clockwise before storing it
        //Ray-casting algorithm in danger evaluation requires this sort
        sortZoneClockwise(temp_zone);

        temp_zones.push_back(std::move(temp_zone));
    }

    // Update the global forbidden zones
    systemForbiddenZones = std::move(temp_zones);
    return 0; // Success
}

// Function to compute the centroid and sort points in clockwise order
void sortZoneClockwise(ForbiddenZone& zone) {
    if (zone.size() < 3) return; // No sorting needed for less than 3 points

    // Step 1: Calculate the centroid
    AzimuthElevation centroid = {0.0, 0.0};
    for (const auto& point : zone) {
        centroid.azimuth += point.azimuth;
        centroid.elevation += point.elevation;
    }
    centroid.azimuth /= zone.size();
    centroid.elevation /= zone.size();

    // Step 2: Sort points in clockwise order based on the centroid
    std::sort(zone.begin(), zone.end(),
              [&centroid](const AzimuthElevation& a, const AzimuthElevation& b) {
                  return calculateAngle(a, centroid) < calculateAngle(b, centroid);
              });
}

// Helper function to calculate the polar angle relative to the centroid
double calculateAngle(const AzimuthElevation& point, const AzimuthElevation& centroid) {
    return atan2(point.elevation - centroid.elevation, point.azimuth - centroid.azimuth);
}
