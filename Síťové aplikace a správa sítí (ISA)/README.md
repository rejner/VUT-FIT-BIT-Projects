# Síťové aplikace a správa sítí - Projekt – Discord bot

Program působí jako bot na komunikační službě Discord. Bot se připojí na Discord server na kanál "#isa-bot" a bude reagovat na všechny zprávy zaslané ostatními uživateli. Bot bude fungovat jako echo pro všechny zprávy, které zachytí. V případě, že bot na daném kanále zachytí jakoukoli zprávu jiného uživatele (tedy jinou než svou vlastní) a zároveň, která není jiného bota (uživatelské jméno neobsahuje podřetězec "bot"), odešle tuto zprávu zpátky na kanál a to ve formátu "echo: <username> - <message>" (kde <username> představuje uživatelské jméno uživatele, který odeslal původní zprávu).

## Spuštění

    ./isabot [-h|--help] [-v|--verbose] -t <bot_access_token>

## Argumenty
 - -h|--help : Vypíše nápovědu na standardní výstup.
 - -v|--verbose : Bude zobrazovat zprávy, na které bot reaguje na standardní výstup ve formátu "<channel> - <username>: <message>".
 - -t <bot_access_token> : Zde je nutno zadat autentizační token pro přístup bota na Discord.

 #### Získáno bodů: 16/20

