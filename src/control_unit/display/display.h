#ifndef DISPLAY_H
#define DISPLAY_H
#include <SoftwareSerial.h>
#include <UTFTGLUE.h>

struct DegreesMinutesSeconds {
    int degrees;
    int minutes;
    int seconds;
};

struct AzimuthElevation {
    double azimuth;
    double elevation;
};

extern UTFTGLUE myGLCD;  // Declare as extern
extern SoftwareSerial toPico;
extern bool settingsScreen;
extern DegreesMinutesSeconds azimuth;
extern DegreesMinutesSeconds elevation;
extern DegreesMinutesSeconds newAzimuth;
extern DegreesMinutesSeconds newElevation;
extern int indexOfCurrentChangingNumber;

// Dispalays AzimuthElevation structure on screen
// @param azimutElevation pointer to AzimuthElevation structure
// @return 0 if success, -1 if error
void displayAE(DegreesMinutesSeconds azimuth, DegreesMinutesSeconds elevation);

void showSettings();
void displayNumbers();
void incrementNumber();
void decrementNumber();

#endif // DISPLAY_H