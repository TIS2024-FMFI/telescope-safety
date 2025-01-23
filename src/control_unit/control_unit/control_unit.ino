
#include <W55RP20lwIP.h> // Include library for the right board
Wiznet55rp20lwIP eth(1 /* chip select */);

#include <WiFiClient.h>
#include <WebServer.h> 
#include <LEAmDNS.h>
#include <StreamString.h>
#include "httpHandlers.h"
#include "servers.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "time.h"


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
  setupMDNSServer();

  timeClient.begin(8001);
  timeClient.update();
  

}

void loop(void) {
  server.handleClient();
  MDNS.update();
  timeClient.update();
  // Example of usage
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


