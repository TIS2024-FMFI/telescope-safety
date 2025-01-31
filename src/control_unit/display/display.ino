#include "display.h"

extern UTFTGLUE myGLCD;

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  toPico.begin(9600);
    
  myGLCD.InitLCD();

  settingsScreen = false;
  indexOfCurrentChangingNumber = 0;
}

void loop()
{
  if (toPico.available()) {
    String command = toPico.readStringUntil('\n');
    if (command == "BUTTON1" && !settingsScreen) {
      settingsScreen = true;
      indexOfCurrentChangingNumber = 0;
      newAzimuth = azimuth;
      newElevation = elevation;
      showSettings();
    } else if (command == "BUTTON1" && settingsScreen) {
      settingsScreen = false;
      displayAE(azimuth, elevation);
    } else if (command == "BUTTON2" && settingsScreen) {
      indexOfCurrentChangingNumber = (indexOfCurrentChangingNumber + 1) % 3;
      displayNumbers();
    } else if (command == "BUTTON3" && settingsScreen) {
      incrementNumber();
    } else if (command == "BUTTON4" && settingsScreen) {
      decrementNumber();
    } else if (command == "SEND" && settingsScreen) {
      char send[20];
      sprintf(send, "%d %d %d\n", newAzimuth.degrees, newAzimuth.minutes, newAzimuth.seconds);
      toPico.write(send);
      settingsScreen = false;
      displayAE(azimuth, elevation);
    } else if (command != "BUTTON1" && command != "BUTTON2" && command != "BUTTON3" && command != "BUTTON4" && command != "SEND"){
      String elevationStr = toPico.readStringUntil('\n');
      int firstSpace = command.indexOf(' ');
      int secondSpace = command.indexOf(' ', firstSpace + 1);
      azimuth.degrees = command.substring(0, firstSpace).toInt();
      azimuth.minutes = command.substring(firstSpace + 1, secondSpace).toInt();
      azimuth.seconds = command.substring(secondSpace + 1).toInt();

      firstSpace = elevationStr.indexOf(' ');
      secondSpace = elevationStr.indexOf(' ', firstSpace + 1);
      elevation.degrees = elevationStr.substring(0, firstSpace).toInt();
      elevation.minutes = elevationStr.substring(firstSpace + 1, secondSpace).toInt();
      elevation.seconds = elevationStr.substring(secondSpace + 1).toInt();

      azimuth = {azimuth.degrees, azimuth.minutes, azimuth.seconds};
      elevation = {elevation.degrees, elevation.minutes, elevation.seconds};
      if (!settingsScreen){
        displayAE(azimuth, elevation);
      }
    }
  }
}
