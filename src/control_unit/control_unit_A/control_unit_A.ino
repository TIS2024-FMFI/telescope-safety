
#include "lora_communication.h"

void setup() {
  Serial.begin(9600);                   // initialize serial
  delay(100);
  
  initializeLoRa();  
}

long lastSendTime = 0;        // last send time
int interval = 20000;          // interval between sends

void loop() {
  if (millis() - lastSendTime > interval) {
    while (restartInertialUnit(132.0) != 0);
    lastSendTime = millis();            // timestamp the message
    interval = 20000;    // 20 seconds
  }

  AzimuthElevation* data = readFromInertialUnit();
    if (data) {
        Serial.println(F("Processed incoming message."));
    }
}
