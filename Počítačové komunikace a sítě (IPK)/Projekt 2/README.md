# IPK - Projekt 2
# Sniffer paketů

Program má funkci snifferu paketů, který z rozhraní daným parametrem -i zachycuje jednotlivé požadované pakety, a jejich obsah
(tzv. payload) vypisuje jak v hexadecimálním, tak ASCII formátu. Pakety lze filtrovat pomocí argumentů, jejichž funkce jsou popsány v sekci s argumenty.

Autor: Michal Rein (xreinm00)

## Spuštění

    ./ipk-sniffer -i rozhraní [-p ­­port] [--tcp|-t] [--udp|-u] [-n num]

## Argumenty
 - -h nebo --help (vypíše tuto hlášku a ukončí program)
 - -i <jméno_rozhraní> (rozhraní, na kterém se bude poslouchat. Nebude-li tento parametr uveden, vypíše se seznam aktivních rozhraní)
 - -p <číslo_portu> (bude filtrování paketů na daném rozhraní podle portu. Nebude-li tento parametr uveden, uvažují se všechny porty)
 - -t nebo --tcp (bude zobrazovat pouze tcp pakety)
 - -u nebo --udp (bude zobrazovat pouze udp pakety)
 - -n <počet_paketů> (určuje počet paketů, které se mají zobrazit. Pokud není uvedeno, uvažujte zobrazení pouze 1 paket)

 #### Získáno bodů: 9/20

