# IJA Projekt - Aplikace pro simulaci hromadné dopravy
Autoři: Michal Rein (xreinm00), Ondřej Motyčka (xmotyc07)

POZNÁMKA KE SPUŠTĚNÍ:
 *při testování na serveru Merlin dochází při spuštění aplikace pomocí příkazu 'ant run' k chybě "java.lang.OutOfMemoryError". Je možné, že jde pouze o omezení zdrojů pro jednotlivé studenty, jelikož na lokálních strojích lze program bez problému spustit pomocí tohoto příkazu. Jestliže však narazíte na podobný problém, lze aplikaci spustit z kořenového adresáře projektu pomocí příkazu 'java -jar dest/java-app.jar'.

## Spuštění:

1. spustit skript lib/get-libs.sh (stažení externí knihovny)
2. spustit překlad aplikace příkazem:
    
        $ ant compile

3. spustit samotnou aplikaci příkazem:
        
        $ ant run

Aplikace simuluje reálný provoz městské hromadné dopravy.
Po spuštění aplikace se zobrazí úvodní menu, s těmito tlačítky:
 - Load reference (načte přednastavenou mapu, kterou lze nalézt v data/examples/example.json)
 - Load file      (lze vybrat vlastní mapu z file systému)
 - Help           (vypíše nápovědu v novém okně)
 - Close          (ukončí aplikaci)

Program se dá logicky rozdělit do několika částí. Základem je mapa, ve které jsou uložené ulice, zastávky a linky. Všechny entity jsou interaktivní a lze je vybrat levým tlačítkem myši. Po vybrání se zobrazí informace o daném objektu v levé části obrazovky, kde se nachází hlavní informační panel. Na něm lze najít mnoho informací, včetně některých interaktivních prvků, jako zobrazení trasy vybraného spoje, nebo možnost volby ztížené dopravní situace na vybrané ulici.

#### Získáno bodů: 60/80


