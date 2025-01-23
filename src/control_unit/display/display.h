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
    float azimuth;
    float elevation;
};

extern UTFTGLUE myGLCD;  // Declare as extern
extern SoftwareSerial toPico;
extern bool settingsScreen;
extern AzimuthElevation ae;
extern AzimuthElevation newae;
extern int indexOfCurrentChangingNumber;

void displayAE(AzimuthElevation azimutElevation);
DegreesMinutesSeconds convertToDMS(float decimalDegrees);
void showSettings();
void displayNumbers();
void incrementNumber();
void decrementNumber();
float convertToDecimalDegrees(DegreesMinutesSeconds dms);

#endif // DISPLAY_H