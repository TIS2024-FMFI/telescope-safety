#include "httpHandlers.h"
#include <Arduino.h>
#include "allINeedForControlUnit.h"
#include "logs.h"
#include <SD.h>

#define DEBUG 1


// Functions
void handleNotFound() {
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
}

void handleFileDownload() {
  String fileName = server.arg("file");
  if (fileName.length() == 0) {
    server.send(400, "text/plain", "File name not specified!");
    return;
  }

  File file = SD.open("/" + fileName);
  if (!file) {
    server.send(404, "text/plain", "File not found!");
    return;
  }

  server.streamFile(file, "text/csv");

  file.close();
}


void handleMainPage() {
  // load from SD card index.html
  // for testing purposes:
  String response = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>Hlavná stránka</title>\n    <link rel=\"stylesheet\" href=\"styles.css\">\n</head>\n<body>\n    <header>\n        <h1>Monitorovanie polohy teleskopu</h1>\n    </header>\n    <main>\n        <section class=\"position\">\n            <h2>Aktuálna poloha</h2>\n            <div class=\"coordinates\">\n                <p>Azimut:</p>\n                <span id=\"azimut\" class=\"large-text\">123°</span>\n                <p>Elevácia:</p>\n                <span id=\"elevacia\" class=\"large-text\">45°</span>\n            </div>\n        </section>\n    </main>\n    <div id=\"error-modal\" class=\"modal hidden\">\n        <div class=\"modal-content\">\n            <p id=\"error-message\">Chyba: Nie je pripojenie k serveru</p>\n        </div>\n    </div>\n</body>\n</html>\n";

  server.send(200, "text/html", response);
}

void handleFormPOST() {
  #if DEBUG
  String message = "";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  Serial.println(message);
  #endif

  if (server.arg(ZONE_CONFIG_BUTTON)){
    if (writeNewForbiddenConfig(server.arg(ZONE_CONFIG_FIELD))){
      writeChangeToLog(FORBIDDEN_ZONE_CHANGED, "");
    }
  }
  else if (server.arg(ALARM_CONFIG_BUTTON)){
    if (server.arg(TURN_OFF_LOGS_CHECKBOX)){
      //
    }
    if (writeNewLogFrequency(server.arg(LOG_INTERVAL_FIELD).toInt(), (server.arg(UPDATE_INTERVAL_FIELD).toInt()))){
      writeChangeToLog(LOG_FREQUENCY_CHANGED, "");
    }
    if (writeNewAlarmType(bool(server.arg(ALARM_CHECKBOX).toInt()), bool(server.arg(MOTORS_CHECKBOX).toInt()))){
      writeChangeToLog(ALARM_TYPE_CHANGED, "");
    }

  }
  else if (server.arg(RESTART_BUTTON)){
    writeChangeToLog(RESTART, "");
    restart();
  }
  else if (server.arg(DOWNLOAD_EVENTS_BUTTON)){
    downloadEventLogs();
  }
  else if (server.arg(DOWNLOAD_LOGS_BUTTON)){
    downloadLogs(server.arg(DOWNLOAD_LOGS_FROM_DATEFIELD), server.arg(DOWNLOAD_LOGS_TO_DATEFIELD));
  }
  handleFormPage();

}

void handleFormPage(){
  // load from SD card form.html
  // for testing purposes:
  // String response = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>Konfigurácia</title>\n    <link rel=\"stylesheet\" href=\"styles.css\">\n</head>\n<body>\n    <header>\n        <h1>Konfigurácia teleskopu</h1>\n    </header>\n    <main>\n        <section class=\"shutdown\">\n            <div class=\"center\">\n                <button id=\"shutdown-button\" class=\"large-button\">Vypnúť systém</button>\n            </div>\n        </section>\n\n        <section class=\"settings\">\n            <h2>Zakázané polohy</h2>\n            <textarea id=\"forbidden-positions\" rows=\"5\" placeholder=\"90 45 # Zakázaná oblasť\"></textarea>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"settings\">\n            <h2>Levely alarmu</h2>\n            <div class=\"checkbox-group\">\n                <label>\n                    <input type=\"checkbox\" id=\"audio-visual\"> Audiovizuálne upozornenie\n                </label>\n                <label>\n                    <input type=\"checkbox\" id=\"disconnect\"> Odpojenie systému\n                </label>\n            </div>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"settings\">\n            <h2>Intervaly</h2>\n            <label for=\"update-interval\">Interval aktualizácií (sekundy):</label>\n            <input type=\"number\" id=\"update-interval\" min=\"1\" value=\"5\">\n            <label for=\"log-interval\">Interval logovania (sekundy):</label>\n            <input type=\"number\" id=\"log-interval\" min=\"1\" value=\"60\">\n            <label>\n                <input type=\"checkbox\" id=\"disable-logging\"> Vypnúť logovanie\n            </label>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"downloads\">\n            <h2>Stiahnutie logov</h2>\n            <div>\n                <label for=\"log-from\">Od:</label>\n                <input type=\"date\" id=\"log-from\">\n                <label for=\"log-to\">Do:</label>\n                <input type=\"date\" id=\"log-to\">\n            </div>\n            <div class=\"center\">\n                <button id=\"download-logs\">Stiahnuť logy</button>\n            </div>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"downloads\">\n            <h2>Stiahnutie udalostí</h2>\n            <div class=\"center\">\n                <button id=\"download-events\">Stiahnuť udalosti</button>\n            </div>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"submit-section\">\n            <div class=\"center\">\n                <button id=\"submit-config\">Konfigurovať</button>\n            </div>\n        </section>\n    </main>\n</body>\n</html>\n";
  String response = "<!DOCTYPE html><html lang=\"en\"><head>    <meta charset=\"UTF-8\">    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">    <title>Konfigurácia</title>    <link rel=\"stylesheet\" href=\"styles.css\">    <!--<script src=\"scripts.js\" defer></script> --></head><body>    <header>        <h1>Konfigurácia teleskopu</h1>    </header>    <main>        <div class=\"center\">            <button type=\"button\" onclick=\"window.location.href='index.html'\" class=\"back-button\">Späť na hlavnú stránku</button>        </div>        <form action=\"#\" method=\"POST\" onsubmit=\"confirmSubmit(event)\">            <section>                <div class=\"center\">                    <button type=\"submit\" id=\"shutdown-button\" class=\"large-button\" name=\"restart\">Reštartovať systém</button>                </div>            </section>        </form>        <form action=\"#\" method=\"POST\" onsubmit=\"confirmSubmit(event)\">            <section class=\"settings\">                <h2>Zakázané polohy</h2>                <textarea id=\"forbidden-positions\" name=\"config_zones_field\" rows=\"5\"></textarea>                <div class=\"center\">                    <button type=\"submit\" class=\"small-button\" name=\"config_zones\">Uložiť zakázané oblasti</button>                </div>            </section>        </form>        <hr class=\"divider\">        <form action=\"#\" method=\"POST\" onsubmit=\"confirmSubmit(event)\">            <section class=\"settings\">                <h2>Levely alarmu</h2>                <div class=\"checkbox-group\">                    <label>                        <input type=\"checkbox\" id=\"audio-visual\" name=\"alarm_conf\" value=\"1\"> Audiovizuálne upozornenie                    </label>                    <label>                        <input type=\"checkbox\" id=\"disconnect\" name=\"motors_conf\" value=\"1\"> Odpojenie systému                    </label>                </div>                <h2>Intervaly</h2>                <label for=\"update-interval\">Interval aktualizácií (sekundy):</label>                <input type=\"number\" id=\"update-interval\" name=\"update_interval\" min=\"1\" value=\"5\">                <label for=\"log-interval\">Interval logovania (sekundy):</label>                <input type=\"number\" id=\"log-interval\" name=\"log_interval\" min=\"1\" value=\"60\">                <label>                    <input type=\"checkbox\" id=\"disable-logging\" name=\"disable_logging\" value=\"1\"> Vypnúť logovanie                </label>                <div class=\"center\">                    <button type=\"submit\" class=\"small-button\" name=\"config_alarm\">Uložiť alarmy a intervaly</button>                </div>            </section>        </form>        <hr class=\"divider\">        <form action=\"#\" method=\"POST\" onsubmit=\"confirmSubmit(event)\">            <section class=\"downloads\">                <h2>Stiahnutie logov</h2>                <div>                    <label for=\"log-from\">Od:</label>                    <input type=\"date\" id=\"log-from\" name=\"logs_from_date\">                    <label for=\"log-to\">Do:</label>                    <input type=\"date\" id=\"log-to\" name=\"logs_to_date\">                </div>                <div class=\"center\">                    <button type=\"submit\" id=\"download-logs\" class=\"small-button\" name=\"download_logs\">Stiahnuť logy</button>                </div>            </section>        </form>        <hr class=\"divider\">        <form action=\"#\" method=\"POST\" onsubmit=\"confirmSubmit(event)\">            <section class=\"downloads\">                <h2>Stiahnutie udalostí</h2>                <div class=\"center\">                    <button type=\"submit\" id=\"download-events\" class=\"small-button\" name=\"download_events\">Stiahnuť udalosti</button>                </div>            </section>        </form>    </main></body></html>";

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
