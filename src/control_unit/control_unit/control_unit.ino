
#include <W55RP20lwIP.h> // Include library for the right board
Wiznet55rp20lwIP eth(1 /* chip select */);

#include <WiFiClient.h>
#include <WebServer.h> 
#include <LEAmDNS.h>
#include <StreamString.h>
#include "httpHandlers.h"
#include "servers.h"
#include <NTPClient.h>
// #include <EthernetUdp.h>
#include <WiFiUdp.h>
// #include <Udp.h>
#include "allINeedForControlUnit.h"


WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "sk.pool.ntp.org", 3600);

// Functions
void setupEthernet();
String timeToString(Time time);


void setup(void) {
  Serial.begin(115200);
  delay(5000);
  Serial.println("Started Serial");

  setupEthernet();
  setupHTTPServer();

  Serial.println("Starting mDNS!");
  if (MDNS.begin("picow")) {
    Serial.println("MDNS responder started");
  }

  timeClient.begin(8001);
  timeClient.update();
  

}

void loop(void) {
  server.handleClient();
  MDNS.update();
  timeClient.update();
  // Serial.println(timeToString(getRealTime()));
}





// Functions
void setupEthernet(){
  Serial.println("Starting ETH");
  // Start the Ethernet port
  if (!eth.begin()) {
    Serial.println("No wired Ethernet hardware detected. Check pinouts, wiring.");
    while (1) {
      delay(1000);
    }
  }

  // Wait for connection
  while (!eth.connected()) {
    Serial.print("_");
    delay(500);
  }
  while (eth.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(eth.localIP());
}

String timeToString(Time time) {
  String formattedTime = String(time.year) + "-";
  
  // Add leading zero for month, day, hours, minutes, and seconds if necessary
  formattedTime += (time.month < 10 ? "0" : "") + String(time.month) + "-";
  formattedTime += (time.day < 10 ? "0" : "") + String(time.day) + " ";
  formattedTime += (time.hours < 10 ? "0" : "") + String(time.hours) + ":";
  formattedTime += (time.minutes < 10 ? "0" : "") + String(time.minutes) + ":";
  formattedTime += (time.seconds < 10 ? "0" : "") + String(time.seconds);
  
  return formattedTime;
}
