
#include "lora_communication.h"

void setup() {
  Serial.begin(9600);                   // initialize serial
  delay(100);
  
  initializeLoRa();  
}

long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

void loop() {
  if (millis() - lastSendTime > interval) {
    while (restartInertialUnit() != 0);
    lastSendTime = millis();            // timestamp the message
    interval = random(2000) + 1000;    // 2-3 seconds
  }

  AzimuthElevation* data = readFromInertialUnit();
    if (data) {
        Serial.println(F("Processed incoming message."));
    }
}
