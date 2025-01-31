#include "display.h"

UTFTGLUE myGLCD(0,A2,A1,A3,A4,A0);
SoftwareSerial toPico(12,11);  // RX = 11 ZLTY na BIELY, TX =12 ORANZOVY na CIERNY (should connect opposite at PICO2)
bool settingsScreen;
DegreesMinutesSeconds azimuth;
DegreesMinutesSeconds elevation;
DegreesMinutesSeconds newAzimuth;
DegreesMinutesSeconds newElevation;
int indexOfCurrentChangingNumber;

void displayAE(DegreesMinutesSeconds azimuth, DegreesMinutesSeconds elevation) {
  myGLCD.clrScr();

  myGLCD.fillScr(255, 200, 50);
  myGLCD.setColor(100, 50, 0);
  myGLCD.fillRoundRect(70, 110, 410, 210);

  myGLCD.fillRoundRect(430, 20, 480, 50);

  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(255, 200, 50);
  myGLCD.setColor(0, 0, 0);
  myGLCD.print("Aktualna poloha", CENTER, 50);
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(100, 50, 0);

  myGLCD.setFont(SmallFont);
  myGLCD.print("Set.", 445, 30);
  
  char degrees[10];
  char minutes[10];
  char seconds[10];
  sprintf(degrees, "%d~", azimuth.degrees);
  sprintf(minutes, "%d'", azimuth.minutes);
  sprintf(seconds, "%d\"", azimuth.seconds);
  myGLCD.setFont(BigFont);
  myGLCD.print("Azimut: ", 105, 130);
  myGLCD.print(degrees, 225, 130);
  myGLCD.print(minutes, 280, 130);
  myGLCD.print(seconds, 330, 130);

  char elevationText[50];
  sprintf(elevationText, "Elevacia: %d~ %d' %d\"", elevation.degrees, elevation.minutes, elevation.seconds);

  myGLCD.print(elevationText, CENTER, 180);
}


void showSettings() {
  myGLCD.clrScr();

  myGLCD.fillScr(255, 200, 50);
  myGLCD.setColor(100, 50, 0);
  myGLCD.fillRoundRect(70, 110, 410, 210);

  myGLCD.fillRoundRect(430, 20, 480, 50);
  myGLCD.fillRoundRect(430, 100, 480, 130);
  myGLCD.fillRoundRect(430, 180, 480, 210);
  myGLCD.fillRoundRect(430, 260, 480, 290);

  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(255, 200, 50);
  myGLCD.setColor(0, 0, 0);
  myGLCD.print("Nastavenie azimutu", CENTER, 50);
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(100, 50, 0);
  myGLCD.setFont(SmallFont);
  myGLCD.print("Back", 440, 30);
  myGLCD.print(">/Send", 432, 110);
  myGLCD.print("+", 453, 190);
  myGLCD.print("-", 453, 270);

  myGLCD.setFont(BigFont);
  
  char degrees[10];
  char minutes[10];
  char seconds[10];
  sprintf(degrees, "%d~", newAzimuth.degrees);
  sprintf(minutes, "%d'", newAzimuth.minutes);
  sprintf(seconds, "%d\"", newAzimuth.seconds);
  myGLCD.print("Azimut: ", 105, 130);
  if (indexOfCurrentChangingNumber == 0){
    myGLCD.setBackColor(0, 0, 0);
  }
  myGLCD.print(degrees, 225, 130);
  myGLCD.setBackColor(100, 50, 0);
  if (indexOfCurrentChangingNumber == 1){
    myGLCD.setBackColor(0, 0, 0);
  }
  myGLCD.print(minutes, 280, 130);
  myGLCD.setBackColor(100, 50, 0);
  if (indexOfCurrentChangingNumber == 2){
    myGLCD.setBackColor(0, 0, 0);
  }
  myGLCD.print(seconds, 330, 130);
  myGLCD.setBackColor(100, 50, 0);

  char elevationText[50];
  sprintf(elevationText, "Elevacia: %d~ %d' %d\"", newElevation.degrees, newElevation.minutes, newElevation.seconds);

  myGLCD.print(elevationText, CENTER, 180);
}

void displayNumbers() {
  myGLCD.setColor(100, 50, 0);
  myGLCD.fillRoundRect(210, 120, 380, 150);
  char degrees[10];
  char minutes[10];
  char seconds[10];
  sprintf(degrees, "%d~", newAzimuth.degrees);
  sprintf(minutes, "%d'", newAzimuth.minutes);
  sprintf(seconds, "%d\"", newAzimuth.seconds);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(100, 50, 0);
  myGLCD.setFont(BigFont);
  if (indexOfCurrentChangingNumber == 0){
    myGLCD.setBackColor(0, 0, 0);
  }
  myGLCD.print(degrees, 225, 130);
  myGLCD.setBackColor(100, 50, 0);
  if (indexOfCurrentChangingNumber == 1){
    myGLCD.setBackColor(0, 0, 0);
  }
  myGLCD.print(minutes, 280, 130);
  myGLCD.setBackColor(100, 50, 0);
  if (indexOfCurrentChangingNumber == 2){
    myGLCD.setBackColor(0, 0, 0);
  }
  myGLCD.print(seconds, 330, 130);
}

void incrementNumber() {
  if (indexOfCurrentChangingNumber == 0) {
    newAzimuth.degrees++;
  } else if (indexOfCurrentChangingNumber == 1) {
    newAzimuth.minutes++;
  } else if (indexOfCurrentChangingNumber == 2) {
    newAzimuth.seconds++;
  }
  displayNumbers();
}

void decrementNumber() {
  if (indexOfCurrentChangingNumber == 0) {
    newAzimuth.degrees--;
  } else if (indexOfCurrentChangingNumber == 1) {
    newAzimuth.minutes--;
  } else if (indexOfCurrentChangingNumber == 2) {
    newAzimuth.seconds--;
  }
  displayNumbers();
}