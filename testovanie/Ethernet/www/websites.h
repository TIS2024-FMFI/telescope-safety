#define index_page  "<!DOCTYPE html>"\
                    "<html lang=\"en\">"\
                    "<head>"\
                        "<meta charset=\"UTF-8\">"\
                        "<title>HTTP Server Running</title>"\
                    "</head>"\
                    "<body>"\
                        "<h1>This is the test page</h1>"\
                        "<p>It is only a test.</p>"\
                        "<p>It is a test.</p>"\
                    "</body>"


#define config_page   "<!DOCTYPE html>"\
                            "<html lang=\"en\">"\
                            "<head>"\
                                "<meta charset=\"UTF-8\">"\
                                "<title>HTTP Server Running</title>"\
                            "</head>"\
                            "<body>"\
                                "<h1>This is the test page for config</h1>"\
                                "<p>It is only a test.</p>"\
                                "<p>Enter config</p>"\
                                "<form action=\"/config.html\" method=\"post\">"\
                                    "<label for=\"fname\">First name:</label><br>"\
                                    "<input type=\"text\" id=\"fname\" name=\"fname\"><br>"\
                                    "<label for=\"lname\">Last name:</label><br>"\
                                    "<input type=\"text\" id=\"lname\" name=\"lname\"><br><br>"\
                                    "<input type=\"submit\" value=\"Submit\">"\
                            "</body>"

#define zone_setup_page "<!DOCTYPE html>"\
                            "<html lang=\"en\">"\
                            "<head>"\
                                "<meta charset=\"UTF-8\">"\
                                "<title>HTTP Server Running</title>"\
                            "</head>"\
                            "<body>"\
                                "<h1>This is the test zone setup page</h1>"\
                                "<p>It is only a test.</p>"\
                                "<p>Enter zones</p>"\
                            "</body>"


#define styles "body {\n    font-family: Arial, sans-serif;\n    margin: 0;\n    padding: 0;\n    background-color: #f4f4f9;\n    color: #333;\n}\n\nheader {\n    background-color: #0066cc;\n    color: #fff;\n    padding: 1rem;\n    text-align: center;\n}\n\nmain {\n    padding: 2rem;\n    max-width: 800px;\n    margin: 0 auto;\n}\n\nh1, h2 {\n    margin-bottom: 1rem;\n}\n\n/* Hlavná stránka */\n.position {\n    background: #fff;\n    border-radius: 8px;\n    padding: 2rem;\n    margin: 2rem auto;\n    box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);\n    max-width: 500px;\n    text-align: center;\n}\n\n.coordinates p {\n    font-size: 1.5rem;\n    margin: 1rem 0;\n}\n\n.large-text {\n    display: block;\n    font-size: 3rem;\n    font-weight: bold;\n    color: #0066cc;\n}\n\n.modal {\n    position: fixed;\n    top: 0;\n    left: 0;\n    width: 100%;\n    height: 100%;\n    background: rgba(0, 0, 0, 0.7);\n    display: flex;\n    justify-content: center;\n    align-items: center;\n}\n\n.modal.hidden {\n    display: none;\n}\n\n.modal-content {\n    background: #fff;\n    padding: 1.5rem;\n    border-radius: 8px;\n    text-align: center;\n    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.3);\n}\n\n#error-message {\n    font-size: 1.2rem;\n    color: #c0392b;\n}\n\n/* Konfiguračná stránka */\ntextarea {\n    width: 100%;\n    padding: 0.8rem;\n    font-size: 1rem;\n    border: 1px solid #ccc;\n    border-radius: 4px;\n    margin-top: 0.5rem;\n    resize: vertical;\n}\n\nlabel {\n    font-size: 1rem;\n    display: flex;\n    align-items: center;\n    margin-top: 1rem;\n}\n\ninput[type=\"checkbox\"] {\n    margin-right: 0.5rem;\n}\n\ninput[type=\"date\"],\ninput[type=\"number\"] {\n    display: block;\n    width: 100%;\n    padding: 0.5rem;\n    margin-top: 0.5rem;\n    font-size: 1rem;\n    border: 1px solid #ccc;\n    border-radius: 4px;\n}\n\nbutton {\n    background-color: #0066cc;\n    color: #fff;\n    padding: 0.8rem 1.2rem;\n    font-size: 1rem;\n    border: none;\n    border-radius: 4px;\n    cursor: pointer;\n    margin-top: 1rem;\n}\n\nbutton:hover {\n    background-color: #004a99;\n}\n\n.large-button {\n    font-size: 1.5rem;\n    padding: 1rem 2rem;\n    background-color: #cc0000;\n    border-radius: 8px;\n    font-weight: bold;\n}\n\n.large-button:hover {\n    background-color: #ff3333;\n}\n\n.center {\n    display: flex;\n    justify-content: center;\n}\n\n.divider {\n    margin: 2rem 0;\n    border: 0;\n    border-top: 1px solid #ccc;\n}\n\n.shutdown {\n    margin-top: 2rem;\n    text-align: center;\n}\n\n.downloads {\n    margin-top: 2rem;\n}\n\n.checkbox-group {\n    display: flex;\n    flex-direction: column;\n    gap: 0.5rem;\n}\n\n.submit-section {\n    margin-top: 2rem;\n    text-align: center;\n}\n"

#define hlavna_stranka "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>Hlavná stránka</title>\n    <link rel=\"stylesheet\" href=\"styles.css\">\n</head>\n<body>\n    <header>\n        <h1>Monitorovanie polohy teleskopu</h1>\n    </header>\n    <main>\n        <section class=\"position\">\n            <h2>Aktuálna poloha</h2>\n            <div class=\"coordinates\">\n                <p>Azimut:</p>\n                <span id=\"azimut\" class=\"large-text\">123°</span>\n                <p>Elevácia:</p>\n                <span id=\"elevacia\" class=\"large-text\">45°</span>\n            </div>\n        </section>\n    </main>\n    <div id=\"error-modal\" class=\"modal hidden\">\n        <div class=\"modal-content\">\n            <p id=\"error-message\">Chyba: Nie je pripojenie k serveru</p>\n        </div>\n    </div>\n</body>\n</html>\n"

#define konfiguracna_stranka "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>Konfigurácia</title>\n    <link rel=\"stylesheet\" href=\"styles.css\">\n</head>\n<body>\n    <header>\n        <h1>Konfigurácia teleskopu</h1>\n    </header>\n    <main>\n        <section class=\"shutdown\">\n            <div class=\"center\">\n                <button id=\"shutdown-button\" class=\"large-button\">Vypnúť systém</button>\n            </div>\n        </section>\n\n        <section class=\"settings\">\n            <h2>Zakázané polohy</h2>\n            <textarea id=\"forbidden-positions\" rows=\"5\" placeholder=\"90 45 # Zakázaná oblasť\"></textarea>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"settings\">\n            <h2>Levely alarmu</h2>\n            <div class=\"checkbox-group\">\n                <label>\n                    <input type=\"checkbox\" id=\"audio-visual\"> Audiovizuálne upozornenie\n                </label>\n                <label>\n                    <input type=\"checkbox\" id=\"disconnect\"> Odpojenie systému\n                </label>\n            </div>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"settings\">\n            <h2>Intervaly</h2>\n            <label for=\"update-interval\">Interval aktualizácií (sekundy):</label>\n            <input type=\"number\" id=\"update-interval\" min=\"1\" value=\"5\">\n            <label for=\"log-interval\">Interval logovania (sekundy):</label>\n            <input type=\"number\" id=\"log-interval\" min=\"1\" value=\"60\">\n            <label>\n                <input type=\"checkbox\" id=\"disable-logging\"> Vypnúť logovanie\n            </label>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"downloads\">\n            <h2>Stiahnutie logov</h2>\n            <div>\n                <label for=\"log-from\">Od:</label>\n                <input type=\"date\" id=\"log-from\">\n                <label for=\"log-to\">Do:</label>\n                <input type=\"date\" id=\"log-to\">\n            </div>\n            <div class=\"center\">\n                <button id=\"download-logs\">Stiahnuť logy</button>\n            </div>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"downloads\">\n            <h2>Stiahnutie udalostí</h2>\n            <div class=\"center\">\n                <button id=\"download-events\">Stiahnuť udalosti</button>\n            </div>\n        </section>\n\n        <hr class=\"divider\">\n\n        <section class=\"submit-section\">\n            <div class=\"center\">\n                <button id=\"submit-config\">Konfigurovať</button>\n            </div>\n        </section>\n    </main>\n</body>\n</html>\n"