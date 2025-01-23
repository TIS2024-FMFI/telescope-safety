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
      char degrees[30];
      char minutes[10];
      char seconds[10];
      sprintf(degrees, "%d %d %d\n\0", azimuth.degrees, azimuth.minutes, azimuth.seconds);

      //sprintf(minutes, "%d", azimuth.minutes);
      //sprintf(seconds, "%d", azimuth.seconds);
      //toPico.write(degrees);
      //toPico.write(";");
      //toPico.write(minutes);
      //toPico.write(";");
      //toPico.write(seconds);
      String posli = degrees;
      posli += " ";
      posli += minutes;
      posli += " ";
      posli += seconds;
      toPico.write(degrees);
      toPico.flush();
      //Serial.println(newae.azimuth);
      //char azimuth[50];
      //snprintf(azimuth, sizeof(azimuth), "%d", (int)ae.azimuth);
      
      settingsScreen = false;
      displayAE(ae);
      //myGLCD.setColor(255, 255, 255);
      //myGLCD.print(".", CENTER, 200);
    }
  }
}
