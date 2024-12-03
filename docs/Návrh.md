# Návrh 

## Podrobná špecifikácia vonkajších rozhraní systému na kontrolu polohy teleskopu:  

### Definícia komunikácie medzi jednotlivými komponentmi hardvéru (merač/vysielač a prijímač/zobrazovač):  

Modul s inerciálnou jednotkou (merač/vysielač): Inerciálna jednotka umiestnená na teleskope bude monitorovať jeho polohu a pohyb a sledovať údaje ako azimut, deklináciu, ako aj komponenty Roll-Pitch-Yaw, Eulerove uhly alebo kvaterniony. Vstavaný mikropočítač spracováva tieto informácie a prostredníctvom rádiovej komunikácie na frekvencii 868 MHz ich prenáša do riadiacej jednotky. Tento modul bude vybavený rádiovým modemom, ktorý zabezpečuje reálnu komunikáciu so systémom riadiacej jednotky.  

Riadiaca jednotka (prijímač/zobrazovač): Má schopnosť prijímať údaje o polohe teleskopu z modulu s inerciálnou jednotkou prostredníctvom rádiového signálu. Obsahuje jednoduchý displej na zobrazenie aktuálnych súradníc azimutu a elevácia. Riadiaca jednotka vie komunikovať s počítačom cez ethernetové rozhranie.  

Riadiaca jednotka, po prijatí dát o polohe ďalekohľadu z inerciálnej jednotky rádiovým signálom, zaloguje prijaté dáta do súboru a odošle dáta vo formáte JSON web serveru (bežiacom na riadiacej jednotke).  

Cpp program číta prichádzajúce dáta z modemu, zisťuje čo je to za typ správy (dáta o polohe alebo alarm, signál pre spustenie alarmu). Pri prijatí alarmu spustí fyzický alarm (audiovizuálny) a zaloguje spustenie alarmu, aktuálnu polohu , pri ktorej došlo ku kolízii (spusteniu alarmu) a dátum a čas (do špeciálneho logu kolízii, ktoré sa nemažú). Pri prijatí dát sa dáta zalogujú (do zvlášť súboru, pre každý deň je vytvorený nový súbor, po určitom čase (konfigurovateľne) sa logy automaticky mažú). Dáta sa ďalej zabalia do formátu JSON a odošlú sa na web server.  

Web server je v jazyku GO a prijate dáta v JSON len jednoducho preposiela cez WebSockety všetkým aktuálne prihláseným užívateľom.  

Prehliadač po prijatí JSON súboru pomocou JavaScript-u JSON rozbalí a zobrazí (Ak ostane priveľa času, tak aj vykreslí ďalekohľad a jeho aktuálnu polohu).  

Používatelia sa cez prehliadač pripoja na web server cez HTTP[S], obdrzia od servera HTML, CSS a JS subory, následne JavaScript na stránke čaká na prijatie dát cez WebSocket.

## Konfiguračné súbory:  

### Modul s inerciálnou jednotkou (MIMU):  
* identifikátor  
* typ modulu  
* nulová poloha - AZ a EL  

### Riadiaca jednotka (RJ):  
* identifikátor  
* TCP/IP nastavenia  
  * IP adresa (Musí byť statická, môže byt zabezpečené cez DHCP, riziko nefunkčnosti pri výpadku) 
  * port, na ktorom bude dostupná služba pre riadiaci softvér na PC  

### PC:   
Všeobecné nastavenia:  
* počet modulov s iniciálnymi meracími jednotkami  
* frekvencia logovania (a zobrazovania) - komunikácia medzi MIMU a RJ  
* súradnicový systém  
* počet zakázaných oblasti  
* zakázaná oblasť 1  
  * počet bodov  
  * bod 1 - AZ a EL  
  * ....  
* alarm distance  

## Dátové súbory:  
* meno obsahuje "Data" + ID riadiacej jednotky, ktorá ho zaznamenala + dátum a čas vzniku súboru, t.j. prvého záznamu v ňom  
* jeden záznam obsahuje  
  * timestamp = dátum a čas  
  * zdroj merania = identifikátor MIMU  
  * Poloha  
    * AZ – azimut  
    * EL - elevácia (výška)  

## Log súbory:  
* meno obsahuje "Log_" + dátum a čas vzniku súboru, t.j. prvého záznamu v ňom  
* jeden záznam obsahuje informácie o mimoriadnej udalosti alebo  konfiguračnej udalosti   

Súbory budú vo formáte CSV.
