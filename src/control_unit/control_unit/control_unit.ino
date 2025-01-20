/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Example works with either Wired or WiFi Ethernet, define one of these values to 1, other to 0
#define USE_WIFI 0
#define USE_WIRED 1

/* SPI */
#define USE_SPI_PIO
#define PIN_SCK 21
#define PIN_MOSI 23
#define PIN_MISO 22
#define PIN_CS 20
#define PIN_RST 25

#if USE_WIFI
#include <WiFi.h>
#elif USE_WIRED
#include <W5500lwIP.h> // Or W5100lwIP.h or ENC28J60lwIP.h
Wiznet5500lwIP eth(PIN_CS); // or Wiznet5100lwIP or ENC28J60lwIP
#endif

#include <WiFiClient.h>
#include <WebServer.h>
#include <LEAmDNS.h>
#include <StreamString.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

uint8_t macAddress[] = {0x02, 0x34, 0x56, 0x78, 0x9A, 0xBC};

WebServer server(80);

const int led = LED_BUILTIN;

//Functions
void handleFormPage();
void handleMainPage();
void handleCSS();
void handleJSMain();
void handleJSForm();

void handleRoot() {
  static int cnt = 0;
  digitalWrite(led, 1);
  int sec = millis() / 1000;
  int hr = sec / 3600;
  int min = (sec / 60) % 60;
  sec = sec % 60;

  StreamString temp;
  temp.reserve(500); // Preallocate a large chunk to avoid memory fragmentation
  temp.printf("<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>" BOARD_NAME " Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from the " BOARD_NAME "!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>Free Memory: %d</p>\
    <p>Page Count: %d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>", hr, min, sec, rp2040.getFreeHeap(), ++cnt);
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void drawGraph() {
  String out;
  out.reserve(2600);
  char temp[70];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}

void blink(int time){
  digitalWrite(led, 1);
  delay(time);
  digitalWrite(led, 0);
  delay(time);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  blink(100);
  Serial.begin(115200);
  blink(100);
  Serial.println("Started Serial");

#if USE_WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#elif USE_WIRED
  // Set up SPI pinout to match your HW
  SPI.setCS(PIN_CS);
  SPI.setSCK(PIN_SCK);
  SPI.setMOSI(PIN_MOSI);  // = SPI.settX
  SPI.setMISO(PIN_MISO);  // = SPI.setRX

  // eth.setSPISettings(SPI);

  // Start the Ethernet port
  if (!eth.begin(macAddress)) {
    Serial.println("No wired Ethernet hardware detected. Check pinouts, wiring.");
    while (1) {
      blink(1000);
      delay(1000);
    }
  }

  // Wait for connection
  while (eth.status() != WL_CONNECTED) {
    blink(500);
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(eth.localIP());
#endif

  if (MDNS.begin("picow")) {
    blink(3000);
    Serial.println("MDNS responder started");
  }
  blink(5000);

  server.on("/", handleRoot);
  server.on("/test.svg", drawGraph);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}






// Functions

void handleMainPage(){
  // load from SD card index.html
  // for testing purposes:
  String response = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>Hlavná stránka</title>\n    <link rel=\"stylesheet\" href=\"styles.css\">\n</head>\n<body>\n    <header>\n        <h1>Monitorovanie polohy teleskopu</h1>\n    </header>\n    <main>\n        <section class=\"position\">\n            <h2>Aktuálna poloha</h2>\n            <div class=\"coordinates\">\n                <p>Azimut:</p>\n                <span id=\"azimut\" class=\"large-text\">123°</span>\n                <p>Elevácia:</p>\n                <span id=\"elevacia\" class=\"large-text\">45°</span>\n            </div>\n        </section>\n    </main>\n    <div id=\"error-modal\" class=\"modal hidden\">\n        <div class=\"modal-content\">\n            <p id=\"error-message\">Chyba: Nie je pripojenie k serveru</p>\n        </div>\n    </div>\n</body>\n</html>\n";

  server.send(200, "text/html", response);
}

void handleFormPage(){
  // load from SD card form.html
  // for testing purposes:
  String response = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>Konfigurácia</title>\n    <link rel=\"stylesheet\" href=\"styles.css\">\n</head>\n<body>\n    <header>\n        <h1>Konfigurácia teleskopu</h1>\n    </header>\n    <main>\n        <section class=\"shutdown\">\n            <div class=\"center\">\n                <button id=\"shutdown-button\" class=\"large-button\">Vypnúť systém</button>\n            </div>\n        </section>\n\n        <section class=\"settings\">\n            <h2>Zakázané polohy</h2>\n            <textarea id=\"forbidden-positions\" rows=\"5\" placeholder=\"90 45 # Zakázaná oblasť\"></textarea>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"settings\">\n            <h2>Levely alarmu</h2>\n            <div class=\"checkbox-group\">\n                <label>\n                    <input type=\"checkbox\" id=\"audio-visual\"> Audiovizuálne upozornenie\n                </label>\n                <label>\n                    <input type=\"checkbox\" id=\"disconnect\"> Odpojenie systému\n                </label>\n            </div>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"settings\">\n            <h2>Intervaly</h2>\n            <label for=\"update-interval\">Interval aktualizácií (sekundy):</label>\n            <input type=\"number\" id=\"update-interval\" min=\"1\" value=\"5\">\n            <label for=\"log-interval\">Interval logovania (sekundy):</label>\n            <input type=\"number\" id=\"log-interval\" min=\"1\" value=\"60\">\n            <label>\n                <input type=\"checkbox\" id=\"disable-logging\"> Vypnúť logovanie\n            </label>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"downloads\">\n            <h2>Stiahnutie logov</h2>\n            <div>\n                <label for=\"log-from\">Od:</label>\n                <input type=\"date\" id=\"log-from\">\n                <label for=\"log-to\">Do:</label>\n                <input type=\"date\" id=\"log-to\">\n            </div>\n            <div class=\"center\">\n                <button id=\"download-logs\">Stiahnuť logy</button>\n            </div>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"downloads\">\n            <h2>Stiahnutie udalostí</h2>\n            <div class=\"center\">\n                <button id=\"download-events\">Stiahnuť udalosti</button>\n            </div>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"submit-section\">\n            <div class=\"center\">\n                <button id=\"submit-config\">Konfigurovať</button>\n            </div>\n        </section>\n    </main>\n</body>\n</html>\n";

  server.send(200, "text/html", response);
}

void handleCSS(){
  // load from SD card styles.css
  // for testing purposes:
  String response = "body {\n    font-family: Arial, sans-serif;\n    margin: 0;\n    padding: 0;\n    background-color: #f4f4f9;\n    color: #333;\n}\n\nheader {\n    background-color: #0066cc;\n    color: #fff;\n    padding: 1rem;\n    text-align: center;\n}\n\nmain {\n    padding: 2rem;\n    max-width: 800px;\n    margin: 0 auto;\n}\n\nh1, h2 {\n    margin-bottom: 1rem;\n}\n\n/* Hlavná stránka */\n.position {\n    background: #fff;\n    border-radius: 8px;\n    padding: 2rem;\n    margin: 2rem auto;\n    box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);\n    max-width: 500px;\n    text-align: center;\n}\n\n.coordinates p {\n    font-size: 1.5rem;\n    margin: 1rem 0;\n}\n\n.large-text {\n    display: block;\n    font-size: 3rem;\n    font-weight: bold;\n    color: #0066cc;\n}\n\n.modal {\n    position: fixed;\n    top: 0;\n    left: 0;\n    width: 100%;\n    height: 100%;\n    background: rgba(0, 0, 0, 0.7);\n    display: flex;\n    justify-content: center;\n    align-items: center;\n}\n\n.modal.hidden {\n    display: none;\n}\n\n.modal-content {\n    background: #fff;\n    padding: 1.5rem;\n    border-radius: 8px;\n    text-align: center;\n    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.3);\n}\n\n#error-message {\n    font-size: 1.2rem;\n    color: #c0392b;\n}\n\n/* Konfiguračná stránka */\ntextarea {\n    width: 100%;\n    padding: 0.8rem;\n    font-size: 1rem;\n    border: 1px solid #ccc;\n    border-radius: 4px;\n    margin-top: 0.5rem;\n    resize: vertical;\n}\n\nlabel {\n    font-size: 1rem;\n    display: flex;\n    align-items: center;\n    margin-top: 1rem;\n}\n\ninput[type=\"checkbox\"] {\n    margin-right: 0.5rem;\n}\n\ninput[type=\"date\"],\ninput[type=\"number\"] {\n    display: block;\n    width: 100%;\n    padding: 0.5rem;\n    margin-top: 0.5rem;\n    font-size: 1rem;\n    border: 1px solid #ccc;\n    border-radius: 4px;\n}\n\nbutton {\n    background-color: #0066cc;\n    color: #fff;\n    padding: 0.8rem 1.2rem;\n    font-size: 1rem;\n    border: none;\n    border-radius: 4px;\n    cursor: pointer;\n    margin-top: 1rem;\n}\n\nbutton:hover {\n    background-color: #004a99;\n}\n\n.large-button {\n    font-size: 1.5rem;\n    padding: 1rem 2rem;\n    background-color: #cc0000;\n    border-radius: 8px;\n    font-weight: bold;\n}\n\n.large-button:hover {\n    background-color: #ff3333;\n}\n\n.center {\n    display: flex;\n    justify-content: center;\n}\n\n.divider {\n    margin: 2rem 0;\n    border: 0;\n    border-top: 1px solid #ccc;\n}\n\n.shutdown {\n    margin-top: 2rem;\n    text-align: center;\n}\n\n.downloads {\n    margin-top: 2rem;\n}\n\n.checkbox-group {\n    display: flex;\n    flex-direction: column;\n    gap: 0.5rem;\n}\n\n.submit-section {\n    margin-top: 2rem;\n    text-align: center;\n}\n";

  server.send(200, "text/css", response);
}

void handleJSMain(){
  // load from SD card main.js
  // for testing purposes:
  String response = "";

  server.send(200, "text/javascript", response);
}

void handleJSForm(){
  // load from SD card form.js
  // for testing purposes:
  String response = "";

  server.send(200, "text/javascript", response);
}