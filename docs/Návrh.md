# Návrh 

## Úvod
(treba doplniť)

## Podrobná špecifikácia vonkajších rozhraní systému na kontrolu polohy teleskopu:  

### Definícia komunikácie medzi jednotlivými komponentmi hardvéru (merač/vysielač a prijímač/zobrazovač):

V moduli s inerciálnou jednotkou bude komunikácia medzi senzorom a mikropočítačom prebiehať pomocou I<sup>2</sup>C rozhrania. Modul s inerciálnou jednotkou bude vybavený rádiovým modemom, ktorý zabezpečuje reálnu komunikáciu so systémom riadiacej jednotky. Na komunikáciu mikropočítača s modemom sa využije SPI - Serial Peripheral Interface. Modem bude poskytovať bezdrátovú komunikáciu medzi modulom s inerciálnou jednotkou a riadiacou jednotkou pomocou protokolu LoRa.

Riadiaca jednotka (prijímač/zobrazovač): Má schopnosť prijímať údaje o polohe teleskopu z modulu s inerciálnou jednotkou prostredníctvom rádiového signálu. Obsahuje jednoduchý displej na zobrazenie aktuálnych súradníc azimutu a elevácia. Riadiaca jednotka vie komunikovať s počítačom cez ethernetové rozhranie.  

Riadiaca jednotka, po prijatí dát o polohe ďalekohľadu z inerciálnej jednotky rádiovým signálom, zaloguje prijaté dáta do súboru a odošle dáta vo formáte JSON web serveru (bežiacom na riadiacej jednotke).  

Cpp program číta prichádzajúce dáta z modemu, zisťuje čo je to za typ správy (dáta o polohe alebo alarm, signál pre spustenie alarmu). Pri prijatí alarmu spustí fyzický alarm (audiovizuálny) a zaloguje spustenie alarmu, aktuálnu polohu , pri ktorej došlo ku kolízii (spusteniu alarmu) a dátum a čas (do špeciálneho logu kolízii, ktoré sa nemažú). Pri prijatí dát sa dáta zalogujú (do zvlášť súboru, pre každý deň je vytvorený nový súbor, po určitom čase (konfigurovateľne) sa logy automaticky mažú). Dáta sa ďalej zabalia do formátu JSON a odošlú sa na web server.  

Web server je v jazyku GO a prijate dáta v JSON len jednoducho preposiela cez WebSockety všetkým aktuálne prihláseným užívateľom.  

Prehliadač po prijatí JSON súboru pomocou JavaScript-u JSON rozbalí a zobrazí (Ak ostane priveľa času, tak aj vykreslí ďalekohľad a jeho aktuálnu polohu).  

Používatelia sa cez prehliadač pripoja na web server cez HTTP[S], obdrzia od servera HTML, CSS a JS subory, následne JavaScript na stránke čaká na prijatie dát cez WebSocket.

(TUTO TO CHCE NEJAKY NADPIS ĎALŠÍ)

## Modul s inerciálnou jednotkou (MIMU):
Inerciálna jednotka umiestnená na teleskope bude monitorovať jeho polohu a pohyb pomocou komponentov Roll-Pitch-Yaw, Eulerových uhlov alebo kvaterniónov. Vstavaný mikropočítač spracováva tieto informácie, preráta ich do uhlov azimut a elevácia. Tieto údaje posiela riadiacej jednotke. V prípade odpojenia systému dostáva správu od riadiacej jednotky, že polohu nie je nutné ďalej merať.

## Riadiaca jednotka (RJ):
* MAC adresa
* TCP/IP nastavenia  
  * IP adresa získaná z DHCP
   
## Všeobecné nastavenia:
* frekvencia logovania (a zobrazovania) - komunikácia medzi MIMU a RJ

## Log súbory:
* priečinok Polohy
  * meno obsahuje "Log_" + MAC riadiacej jednotky, ktorá ho zaznamenala + dátum vzniku súboru, t.j. prvého záznamu v ňom  
  * jeden záznam obsahuje  
    * timestamp = dátum a čas  
    * zdroj merania = identifikátor MIMU  
    * Poloha  
      * AZ – azimut  
      * EL - elevácia (výška)
* jeden subor obsahuje informácie o mimoriadnej udalosti
* jeden subor obsahuje informácie o konfiguračnej udalosti
* Súbory budú vo formáte CSV.

## Konfiguračné súbory:
* riadky začínajúce "#" sú ignorované (slúžia na zlepšenie orientacie v súbore, pomenovanie jednotlivých zón)
* riadok obsahuje dvojicu čísel v tvare "AZIMUTH_SPACE_ELEVATION_NEWLINE"
* prázdny riadok (obsahujúci NEWLINE) oddeluje jednotlivé oblasti
* každú oblasť tvoria minimalne 3 záznamy Azimuthu a Elevacie

## Diagramy:
(treba doplniť stavový, komponent diagram)

## Návrch používateľského rozhrania:
(treba doplniť)

## Plán implementácie:

<b>Modul s inerciálnou jednotkou</b><br>
program v jazyku C vytvorený vo vývojovom prostredí Arduino IDE pre mikropočítač Raspberry Pi Pico 2 na získanie údajov z inerciálnej jednotky 10 DOF IMU Sensor,ICM20948 vo forme Eulerových uhlov, výpočet kvaterniónov, prevod na azimut a eleváciu
program pre vysielač modemu LoRa v jazyku C vytvorený vo vývojovom prostredí Arduino IDE na rádiové zasielanie údajov získaných zo senzoru
program pre prijímač modemu LoRa na prijatie správy ohľadom vypnutia systému

<b>Riadiaca jednotka</b><br>
program v jazyku C vytvorený vo vývojom prostredí Arduino IDE pre spracovanie údajov prijatých u prijímača modemu LoRa
program v jazyku C na vyhodnotenie nebezpečenstva podľa získaných a konfiguračných údajov, spúšťanie alarmu, vypnutie systému
program v jazyku C na spracovanie konfiguračného súboru a jeho naparsovanie do dátovej štruktúry

program v jazyku C pre príjem HTTP requestov a distribúciu statických súborov
program v jazyku C pre príjem HTTP POST requestov na konfiguráciu systému
program v jazyku C pre príjem HTTP POST requestov na zipovanie a stiahnutie logovych dat
program v jazyku C pre upgrade HTTP na WebSocket a naslednu distribuciu aktuálnych dát z inerciálnej jednotky

program pre logovanie dát, posielanie na webový server
program na vypísanie dát na displej

<b>Webové rozhranie</b><br>
program v html, css a javascript jazyku pre stránky používateľského rozhrania(konfiguračná stránka, informačná stránka)

## Návrh implementácie:
(treba doplniť-potom čo si to všetci pozrú v branchi s návrhom implementácie, najlepšie ako kód vsuvvku)

## Záver
(treba doplniť/optional)
