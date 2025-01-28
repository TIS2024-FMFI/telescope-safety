#include "display.h"
//#include "lora_communication.h"

extern UTFTGLUE myGLCD;

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  toPico.begin(9600);
    
  myGLCD.InitLCD();

  settingsScreen = false;
  indexOfCurrentChangingNumber = 0;

  ae = {45.132, 30.343};
  newae = ae;
  displayAE(ae);
}

void loop()
{
  if (toPico.available()) {
    String command = toPico.readStringUntil('\n');
    if (command == "BUTTON1" && !settingsScreen) {
      settingsScreen = true;
      indexOfCurrentChangingNumber = 0;
      newae = ae;
      showSettings();
    } else if (command == "BUTTON1" && settingsScreen) {
      settingsScreen = false;
      displayAE(ae);
    } else if (command == "BUTTON2" && settingsScreen) {
      indexOfCurrentChangingNumber = (indexOfCurrentChangingNumber + 1) % 3;
      displayNumbers();
    } else if (command == "BUTTON3" && settingsScreen) {
      incrementNumber();
    } else if (command == "BUTTON4" && settingsScreen) {
      decrementNumber();
    } else if (command == "SEND" && settingsScreen) {
      ae = newae; //poslat newae
      DegreesMinutesSeconds azimuth = convertToDMS(newae.azimuth);
      char send[8];
      sprintf(send, "%d %d %d\n", azimuth.degrees, azimuth.minutes, azimuth.seconds);
      toPico.write(send);
      settingsScreen = false;
      displayAE(ae);
    }
  }
}
