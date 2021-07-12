# Zadání

Implementujte v jazyce C modifikovaný synchronizační problém River Crossing Problem (viz kniha
The Little Book of Semaphores).

Na řece je jedna vyhlídková lod’. Lod’ může vézt právě jednu skupinu osob, skupinu tvoří právě čtyři
osoby. Nelze tedy vézt více ani méně osob než právě čtyři. Osoby jsou rozděleny do dvou kategorií -
první kategorie jsou osoby, které pracují v prostředí operačního systému Linux (hackers) a druhou
kategorii tvoří osoby, které pracují v prostředí operačního systému Windows (serfs). Platí, že nelze
vézt skupinu osob, kde je pouze jedna osoba ze stejné kategorie (tři hackers a jeden serf nebo jeden
hacker a tři serfs); v takovém případě se tato osoba necítí bezpečně. Povolené jsou tedy následující tři
kombinace: 4 hackers; 4 serfs; 2 hackers a 2 serfs.

Osoby, které se chtějí přepravit, se shromažd’ují na molu. Molo má omezenou kapacitu, pokud se
nově příchozí osoba již na molo nevejde, odchází pryč a zkusí přijít na molo později. Tento postup
se v případě potřeby opakuje, tj. každá osoba nakonec odpluje. Jakmile je možné na molu vytvořit
vhodnou skupinu, nastoupí tato skupina na lod’ a odpluje. Jedna z osob, která nastoupila na lod’, se
stává kapitánem a řídí lod’. Po určité době se lod’ vrátí zpět, skupina osob vystoupí a odejde pryč. Po
dobu plavby (včetně vylodění skupiny) nevstupuje žádná osoba z mola na lod’, osoby však mohou
přicházet na molo. Pořadí nalodění osob není definováno. Kapitán opouští lod’ jako poslední, pořadí
vylodění ostatních osob není definováno.

# Spuštění

    $ ./proj2 P H S R W C

kde:

 - P je počet osob generovaných v každé kategorii; bude vytvořeno P hackers a P serfs.<br> P >= 2 && (P % 2) == 0
 - H je maximální hodnota doby (v milisekundách), po které je generován nový proces hackers.<br> H >= 0 && H <= 2000
 - S je maximální hodnota doby (v milisekundách), po které je generován nový proces serfs.<br> S >= 0 && S <= 2000
 - R je maximální hodnota doby (v milisekundách) plavby.<br> R >= 0 && R <= 2000
 - W je maximální hodnota doby (v milisekundách), po které se osoba vrací zpět na molo (pokud bylo před tím molo plné).<br> W >= 20 && W <= 2000
 - C je kapacita mola.<br> C >= 5
 - Všechny parametry jsou celá čísla.

 #### Získáno bodů: 12/15