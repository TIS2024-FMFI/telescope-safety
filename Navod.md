## Ovladanie systemu
### Webstranka
#### Hlavna stranka
Na hlavnej stranke (teleskop.local) sa nachadza informacia o aktualnej polohe teleskopu. Dole pod polohou sa zobrazuje datum a cas poslednehej prijatej spravy o aktualnej polohe.

Nad touto polohou sa zobrazuje sprava o pripadnom spusteni alarmu alebo pripajani k serveru ak by z nejakeho dovodu nebol dostupny.

Pod zobrazenim polohy sa nachadza tlacidlo, ktorym uzivatel prejde na nastavenie konfiguracie (link: teleskop.local/config)

#### Konfiguracna stranka
Na konfiguracnej stranke sa da cely system nastavovat. Na vrchu v lavo sa nachadza tlacidlo, ktorym uzivatel prejde naspat na halvnu stranku.

Prve nastavenie je velke cervene tlacidlo, ktore vysle spravu pre restart inercialneho senzoru. Restart treba vykonavat za pokojneho stavu teleskopu v polohe zvyslej (inercialny senzor musi byt vodorovne), cize pri elevacii 90.

Dalej si vie uzivatel navolit zakazane zony (poligony) do ktorych teleskop nesmie vojst (z bespecnostnych dovodou). Kazdu zonu musia tvorit aspon 3 body. kazdy bod je tvoreny z azimutu a elevacie a ja v samostatnom riadku. Riadky zacinajuce znakom "#" su ignorovane vnutri systemu. Sluzia pre uzivatela, aby si dokazal pomenovat jednotlive zony. Zony musia byt oddelene jednym prazdnym riadkom.

Priklad takehoto nastavenia 2 zon:

```
# zona braniaca prilis nizkej elevacii
0 45
360 45
0 -90
365 -90

#druha zona popisujuca nohu dalekohladu
58 60
64 60
60 50
``` 

V dalsej casti sa da nastavit, ci ma system spustat alarm pri vstupe do zakazanej zony, ci ma odpojit napajanie motorov, ktore ovladaju teleskop, ako casto sa maju posielat udaje o katualnej polohe z riadiacej jednotky na web, ako casto sa ma aktualna (okamzita) poloha teleskopu zapisovat do suboru s logmi ci sa maju taketo logy vobec zapisoavat podla tohoto nastavenie (ak su vypnute logy nezapisuju sa. Logy o vstupe do zakazanej zony a konfiguraciach sa vzdy zapisu).

V dalsej sekcii si vieme navolit 2 datumy od kedy do kedy si chceme stiahnut logy. Po potvrdeni sa JavaScript na stranke postara o postupne stiahnutie vsetkych existujucich suborov s logmi, ktore vyhovuju intervalu.

V poslednej sekcii si rovnako vieme stiahnut Logy, tentokrat vsak udalosti. Udalostou chapeme vstup do zakazanej zony alebo zmenu konfiguracie. Pre oba tieto druhy udalosti je zvlast subor.
 