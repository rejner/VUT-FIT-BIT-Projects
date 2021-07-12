/*
*   Soubor:     main.h
*   Popis:      Hlavičkový soubor zdrojového kódu main.cpp, obsahující deklaraci 
*               tříd a globálních proměnných/konstant.
*
*               Zdrojové kódy jsou součástí projektu do předmětu IMS na VUT FIT,
*               jehož téma zahrnuje vytvoření a simulaci epidemiologického modelu na mikro úrovni.
*               Samotný model poté znázorňuje vývoj epidemie (konkrétně studované na onemocnění Covid-19)
*               v prostorách supermárketu.
*
*   Autoři:     Michal Rein (xreinm00), Ondřej Motyčka (xmotyc07)
*   Komentář:   Implementovaný model vychází ze studie, kterou lze nalézt
*               na tomto url: 'https://arxiv.org/ftp/arxiv/papers/2006/2006.10666.pdf'.
*   Datum:      04.12.2020
*/

#include "simlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <math.h>
#include <map>
#include <iterator>
#include <algorithm>

/*
    Všechny níže uvedené globální proměnné lze přepisovat, a tak měnit parametry simulace.
*/

bool VIZUALIZOVAT = false;      // pro každou sekundu vypíše vizualizaci prostoru budovy se zákazníky v reálném čase
bool VERSOBE = false;           // povoluje výpis příchodu/odchodu zákazníků na stdout

int START_TIME = 1; 
int END_TIME = 46800;           // 46800 sekund = 13 hodin
int NUM_OF_CUSTOMERS = 0;       // počítadlo počtu zákazníků aktuálně se nacházejících v obchodě
int KAPACITA_OBCHODU = 60;      // maximální počet připuštěných zákazníků do prostor obchodu - 15 m2 pro jednu osobu
double VYSKYT_INFEKCE = 0.005;  // procentuální vyjádření šance na vstup nemocného zákazníka do obchodu - 0.05 % infikovaných v populaci
double NOSENI_ROUSEK = 1.0;     // podíl lidí v populaci, kteří nosí roušku - 100 %

const int x_max = 40;           // Délka obchodu (v metrech)
const int y_max = 40;           // Šířka obchodu (v metrech)

// frekvence návštěnosti pro otevírací dobu od 7:00 do 20:00 (13 hodin)
double frekvence_navstevnosti[13] = {0.029, 0.053, 0.077, 0.104, 0.084, 0.080, 0.113, 0.110, 0.127, 0.096, 0.065, 0.044, 0.018};
double prumerny_pocet_zakazniku_za_den = 4000;

enum
{
    VOLNE_POLE,  // sem mohou lidi stoupnout
    ZABRANE_POLE // regály atd.
};

// Enum pro jednotlivé směry pohybu
enum
{
    POHYB_VLEVO,
    POHYB_VPRAVO,
    POHYB_DOPREDU,
    POHYB_DOZADU,
    POHYB_STAT
};

class Pole;

// Zákazník
class Zakaznik : public Process
{
public:
    // vlastnosti zákazníka
    double cas_prichodu = 0.0;
    double cas_odchodu = 0.0;
    bool je_nakazeny = false;
    bool zrovna_prisel = true;
    bool vstoupil_do_obchodu = false;

    int cil_x;                      // index řádku na kterém se nachází objekt zájmu zákazníka
    int cil_y;                      // index sloupce na kterém se nachází objekt zájmu zákazníka
    int pocitadlo_rizika = 0;       // čas vystavení se infekci (kontakt s infikovaným/kontaminovanou oblastí)
    int pocitadlo_kaslani = 15;     // interval mezi kašláním nakaženého (ze studie cca 15 sekund/zakašlání)

    /* --- Zatím neimplementováno --- */
    bool ma_rousku = false;
    int vekova_skupina; // mladi-> více bezohlední, starsi-> dodržují pravidla ve větší míře, důchodci-> dodržují pravidla nejvíc
    /*--------------------------------*/

    Pole *umisteni = nullptr;

    Zakaznik(); // konstruktor
    void Behavior();

private:
    int Pohyb();                             // Vypočítá a vrací směr pohybu pro další krok simulace
    void kontaminovat_pole(int x, int y);    // Provede kontaminaci polí v oblasti 5x5 metrů kolem infokovaného
};

// Pole reprezentuje prostor o velikosti 1x1 metr v budově
class Pole
{
public:
    int x;
    int y;
    int typ;
    bool kontaminovano = false;
    int zbyvajici_kontaminace = 0;
    bool obsazeno = false;
    bool objekt_zajmu = false;
    Zakaznik *zakaznik;

    Pole(int x, int y, int typ);

    Pole *validovat_a_provest_pohyb(int smer);
    void zkontrolovat_kontaminaci();
};

// Reprezentace budovy
class Budova
{
public:
    Pole *rozlozeni[x_max][y_max];  // Rozložení budovy složené z instancí třídy Pole
    Budova();                       
    void print_layout();            // Vypíše vizualizaci aktuálního stavu budovy na stdout
    void zkontrolovat_pole();       // Spravuje úrovně kontaminací polí
};

class Statistiky{
    public:
    int statistika_doba_vystaveni_nakaze = 0;
    int statistika_vic_nez_20s = 0;
    int counter = 0;
    int pocet_lidi=0;
    Statistiky();
    void Calc_and_print();
};

class myHistogram : public Histogram {
    public:

        using Histogram::Histogram;
        void myOutput(Statistiky *statistika);
};

