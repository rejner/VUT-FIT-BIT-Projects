#include "main.h"

using namespace std;

Store SuperMarket("SuperMarket", KAPACITA_OBCHODU); // Obchod
Facility Vchod("Vchod");                            // Vchod
Budova *budova;
Statistiky *statistika;

Histogram VystaveniInfekci("Čas vystavení infekci vůči celkovému času \n  stráveného v obchodě (v %)", 0.0, 0.1, 10);
myHistogram CelkovyCasVystaveniInfekci("Celkový čas vystavení infekci (v sekundách)", 0.0, 20.0, 10);
Histogram PomerInfikovanychAZdravych("Poměř infikovaných a zdravých zákazníků\n  vstupujících do obchodu (0-1 - infikovany, 1-2 - zdravy)", 0.0, 1.0, 2);
Histogram Navstevnost("Počet náštev v jednolivých hodinách otevírací doby", 0.0, 1.0, 13);

// konstruktor budovy
Budova::Budova()
{

    // vytvořit layout
    for (int i = 0; i <= x_max - 1; i++)
    {
        for (int j = 0; j <= y_max - 1; j++)
        {
            rozlozeni[i][j] = new Pole(i, j, VOLNE_POLE);
        }
    }
}

void myHistogram::myOutput(Statistiky *statistika)
{
    long sum = 0;
    unsigned i;
    for (i = 0; i <= count + 1; i++)
        sum += dptr[i];
    if (sum == 0)
        return;

    double from, to;
    long s;
    from = low;
    s = dptr[0];
    for (i = 1; i <= count; i++)
    {
        unsigned x = dptr[i];
        s += x;
        to = from + step;
        from = to;
        x = sum - x;
        statistika->statistika_vic_nez_20s += x;
        statistika->counter += 1;
        statistika->pocet_lidi += sum;
        break;
    }
}

void Budova::print_layout()
{

    // rows
    for (int i = 0; i <= x_max - 1; i++)
    {
        // cols
        for (int j = 0; j <= y_max - 1; j++)
        {

            if (this->rozlozeni[i][j]->obsazeno)
            {
                this->rozlozeni[i][j]->zakaznik->je_nakazeny ? cout << "\xF0\x9F\x98\xA1 " : cout << "\xF0\x9F\x98\x84 ";
            }
            else
            {
                if (this->rozlozeni[i][j]->objekt_zajmu)
                {
                    cout << "\xF0\x9F\x92\x9A ";
                }
                else
                {
                    this->rozlozeni[i][j]->kontaminovano ? cout << "\xF0\x9F\x94\xB4 " : cout << "\xF0\x9F\x94\xB5 ";
                }
            }

            if (j == y_max - 1)
            {
                cout << endl;
            }
        }
    }
}

void Budova::zkontrolovat_pole()
{
    for (int i = 0; i <= x_max - 1; i++)
    {
        for (int j = 0; j <= y_max - 1; j++)
        {
            rozlozeni[i][j]->zkontrolovat_kontaminaci();
        }
    }
}

// konstruktor třídy pole
Pole::Pole(int x, int y, int typ)
{
    this->x = x;
    this->y = y;
    this->typ = typ;
}

Pole *Pole::validovat_a_provest_pohyb(int smer)
{

    switch (smer)
    {
    case POHYB_DOPREDU:
        if (this->x == 0)
            return nullptr;
        if (this->x > 0 && !budova->rozlozeni[this->x - 1][this->y]->obsazeno)
        {
            //cout << "pohyb dopredu" << endl;
            budova->rozlozeni[this->x - 1][this->y]->zakaznik = this->zakaznik;
            budova->rozlozeni[this->x - 1][this->y]->obsazeno = true;
            this->obsazeno = false;
            return budova->rozlozeni[this->x - 1][this->y];
        }
        break;

    case POHYB_DOZADU:
        if (this->x == x_max - 1)
            return nullptr;
        if (this->x < x_max - 1 && !budova->rozlozeni[this->x + 1][this->y]->obsazeno)
        {
            //cout << "pohyb dozadu" << endl;
            budova->rozlozeni[this->x + 1][this->y]->zakaznik = this->zakaznik;
            budova->rozlozeni[this->x + 1][this->y]->obsazeno = true;
            this->obsazeno = false;
            return budova->rozlozeni[this->x + 1][this->y];
        }
        break;

    case POHYB_VLEVO:
        if (this->y == 0)
            return nullptr;
        if (this->y > 0 && !budova->rozlozeni[this->x][this->y - 1]->obsazeno)
        {
            //cout << "pohyb doleva" << endl;
            budova->rozlozeni[this->x][this->y - 1]->zakaznik = this->zakaznik;
            budova->rozlozeni[this->x][this->y - 1]->obsazeno = true;
            this->obsazeno = false;
            return budova->rozlozeni[this->x][this->y - 1];
        }
        break;

    case POHYB_VPRAVO:
        if (this->y == y_max - 1)
            return nullptr;
        if (this->y < y_max - 1 && !budova->rozlozeni[this->x][this->y + 1]->obsazeno)
        {
            //cout << "pohyb doprava" << endl;
            budova->rozlozeni[this->x][this->y + 1]->zakaznik = this->zakaznik;
            budova->rozlozeni[this->x][this->y + 1]->obsazeno = true;
            this->obsazeno = false;
            return budova->rozlozeni[this->x][this->y + 1];
        }
        break;

    default:
        //cout << "pohyb zustat" << endl;
        return this;
    }

    return nullptr;
}

void Pole::zkontrolovat_kontaminaci()
{
    if (this->kontaminovano)
    {
        if (--this->zbyvajici_kontaminace <= 0)
        {
            this->kontaminovano = false;
        }
    }
}

// konstruktor třídy zákazník
Zakaznik::Zakaznik()
{
    this->cas_prichodu = Time;
    this->cas_odchodu = Time + (Uniform(1.0, 1620.0)); // 0-1 * 27 minut

    if (Random() <= VYSKYT_INFEKCE)
    {
        this->je_nakazeny = true;
        PomerInfikovanychAZdravych(0.0);
    }
    else
    {
        PomerInfikovanychAZdravych(1.0);
    }

    if (Random() <= NOSENI_ROUSEK)
    {
        this->ma_rousku = true;
    }

    this->cil_x = (int)(Random() * (x_max - 1));
    this->cil_y = (int)(Random() * (y_max - 1));
    this->Activate();
}

// implementace chování zákazníka
void Zakaznik::Behavior()
{
    while (this->cas_odchodu >= Time)
    {
        if (this->zrovna_prisel)
        {
            if (VERSOBE)
                cout << "+ PŘÍCHOD->ZÁKAZNÍK, čas: " << this->cas_prichodu << " (odchod: " << this->cas_odchodu << ")" << endl;

            Enter(SuperMarket, 1);
            budova->rozlozeni[cil_x][cil_y]->objekt_zajmu = true;
            NUM_OF_CUSTOMERS += 1;

            Seize(Vchod);
            this->umisteni = budova->rozlozeni[x_max - 1][1];
            budova->rozlozeni[x_max - 1][1]->obsazeno = true;
            budova->rozlozeni[x_max - 1][1]->zakaznik = this;
            int pohyb = this->Pohyb();
            Pole *nove_umisteni = budova->rozlozeni[x_max - 1][1]->validovat_a_provest_pohyb(pohyb);
            while (nove_umisteni == nullptr)
            {
                pohyb = this->Pohyb();
                nove_umisteni = budova->rozlozeni[x_max - 1][1]->validovat_a_provest_pohyb(pohyb);
            }

            //cout << "X: " << umisteni->x << " Y: " << umisteni->y << " ---> X: " << nove_umisteni->x << " Y: " << nove_umisteni->y << endl;
            this->umisteni = nove_umisteni;
            this->zrovna_prisel = false;
            Release(Vchod);
            this->vstoupil_do_obchodu = true;
        }
        else
        {

            int pohyb = this->Pohyb();
            Pole *nove_umisteni = this->umisteni->validovat_a_provest_pohyb(pohyb);
            while (nove_umisteni == nullptr)
            {
                pohyb = this->Pohyb();
                nove_umisteni = this->umisteni->validovat_a_provest_pohyb(pohyb);
            }

            //cout << "X: " << umisteni->x << " Y: " << umisteni->y << " ---> X: " << nove_umisteni->x << " Y: " << nove_umisteni->y << endl;
            this->umisteni = nove_umisteni;
        }

        if (this->je_nakazeny)
        {
            pocitadlo_kaslani--;

            // zákazník kašle, infikuje oblask kolem sebe
            if (pocitadlo_kaslani <= 0)
            {

                for (int i = 0; i <= 2; i++)
                {
                    if (this->umisteni->x - i >= 0 && this->umisteni->x + i <= x_max - 1)
                    {
                        if (this->umisteni->y - i >= 0 && this->umisteni->y + i <= y_max - 1)
                        {

                            kontaminovat_pole(this->umisteni->x - i, this->umisteni->y);
                            kontaminovat_pole(this->umisteni->x - i, this->umisteni->y + i);
                            kontaminovat_pole(this->umisteni->x - i, this->umisteni->y - i);

                            kontaminovat_pole(this->umisteni->x + i, this->umisteni->y);
                            kontaminovat_pole(this->umisteni->x + i, this->umisteni->y + i);
                            kontaminovat_pole(this->umisteni->x + i, this->umisteni->y - i);

                            kontaminovat_pole(this->umisteni->x, this->umisteni->y + i);
                            kontaminovat_pole(this->umisteni->x, this->umisteni->y - i);

                            kontaminovat_pole(this->umisteni->x + i, this->umisteni->y);
                            kontaminovat_pole(this->umisteni->x - i, this->umisteni->y);

                            if (i > 0)
                            {
                                kontaminovat_pole(this->umisteni->x + i, this->umisteni->y + i - 1);
                                kontaminovat_pole(this->umisteni->x + i, this->umisteni->y - i + 1);

                                kontaminovat_pole(this->umisteni->x - i, this->umisteni->y + i - 1);
                                kontaminovat_pole(this->umisteni->x - i, this->umisteni->y - i + 1);

                                kontaminovat_pole(this->umisteni->x - i + 1, this->umisteni->y + i);
                                kontaminovat_pole(this->umisteni->x + i - 1, this->umisteni->y + i);

                                kontaminovat_pole(this->umisteni->x - i + 1, this->umisteni->y - i);
                                kontaminovat_pole(this->umisteni->x + i - 1, this->umisteni->y - i);
                            }
                        }
                    }
                    if (this->ma_rousku && i == 1)
                    {
                        break;
                    }
                }
                pocitadlo_kaslani = 15;
            }
        }
        else
        {

            // zdravý člověk se musí ujistit že není v kontaminované oblasti
            if (this->umisteni->kontaminovano)
            {
                this->pocitadlo_rizika++;

                // nebo právě není v bezprostředním kontaktu s nakaženým
            }
            else
            {

                if (this->umisteni->x > 0 && budova->rozlozeni[this->umisteni->x - 1][this->umisteni->y]->obsazeno && budova->rozlozeni[this->umisteni->x - 1][this->umisteni->y]->zakaznik->je_nakazeny)
                {
                    this->pocitadlo_rizika++;
                }
                else if (this->umisteni->x < x_max - 1 && budova->rozlozeni[this->umisteni->x + 1][this->umisteni->y]->obsazeno && budova->rozlozeni[this->umisteni->x + 1][this->umisteni->y]->zakaznik->je_nakazeny)
                {
                    this->pocitadlo_rizika++;
                }

                else if (this->umisteni->y > 0 && budova->rozlozeni[this->umisteni->x][this->umisteni->y - 1]->obsazeno && budova->rozlozeni[this->umisteni->x][this->umisteni->y - 1]->zakaznik->je_nakazeny)
                {
                    this->pocitadlo_rizika++;
                }

                else if (this->umisteni->y < y_max - 1 && budova->rozlozeni[this->umisteni->x][this->umisteni->y + 1]->obsazeno && budova->rozlozeni[this->umisteni->x][this->umisteni->y + 1]->zakaznik->je_nakazeny)
                {
                    this->pocitadlo_rizika++;
                }
            }
        }
        this->Wait(1);
    }

    if (!this->vstoupil_do_obchodu)
    {
        Leave(SuperMarket, 1);
        return;
    }
    if (!this->je_nakazeny)
    {
        if (VERSOBE)
            cout << "- ODCHOD->ZÁKAZNÍK, čas:  " << Time - this->cas_prichodu << "sec  Vystaven kontaminaci: " << this->pocitadlo_rizika << "sec" << endl;
        double podil_vysveni_infekce = this->pocitadlo_rizika / (Time - this->cas_prichodu);
        VystaveniInfekci(podil_vysveni_infekce);
        CelkovyCasVystaveniInfekci(this->pocitadlo_rizika);

        //statistika->statistika_doba_vystaveni_nakaze += this->pocitadlo_rizika;
    }
    else
    {
        if (VERSOBE)
            cout << "- ODCHOD->ZÁKAZNÍK, čas:  " << Time - this->cas_prichodu << "sec (Infikovany)" << endl;
    }

    budova->rozlozeni[cil_x][cil_y]->objekt_zajmu = false;
    umisteni->obsazeno = false;

    NUM_OF_CUSTOMERS -= 1;
    Leave(SuperMarket, 1);
}

void Zakaznik::kontaminovat_pole(int x, int y)
{
    budova->rozlozeni[x][y]->kontaminovano = true;
    budova->rozlozeni[x][y]->zbyvajici_kontaminace = 15;
}

bool sortByVal(const pair<int, double> &a, const pair<int, double> &b)
{
    return (a.second < b.second);
}

// implementace pohybu zákazníka
// šance na směr pohybu/stání je ovlivněna bodem zájmu
int Zakaznik::Pohyb()
{
    double vlevo, vpravo, dopredu, dozadu = 0;

    double vzdalenost_vpravo = sqrt(pow(this->umisteni->x - cil_x, 2) + pow((this->umisteni->y + 1) - cil_y, 2) * 1.0);
    double vzdalenost_vlevo = sqrt(pow(this->umisteni->x - cil_x, 2) + pow((this->umisteni->y - 1) - cil_y, 2) * 1.0);
    double vzdalenost_dopredu = sqrt(pow((this->umisteni->x - 1) - cil_x, 2) + pow(this->umisteni->y - cil_y, 2) * 1.0);
    double vzdalenost_dozadu = sqrt(pow((this->umisteni->x + 1) - cil_x, 2) + pow(this->umisteni->y - cil_y, 2) * 1.0);

    vector<pair<int, double>> vzdalenosti;
    vzdalenosti.push_back(make_pair(POHYB_DOPREDU, vzdalenost_dopredu));
    vzdalenosti.push_back(make_pair(POHYB_DOZADU, vzdalenost_dozadu));
    vzdalenosti.push_back(make_pair(POHYB_VPRAVO, vzdalenost_vpravo));
    vzdalenosti.push_back(make_pair(POHYB_VLEVO, vzdalenost_vlevo));

    // srovnat podle velikosti
    sort(vzdalenosti.begin(), vzdalenosti.end(), sortByVal);

    int i = 1;
    for (auto it = vzdalenosti.begin(); it != vzdalenosti.end(); it++)
    {
        switch (it->first)
        {
        case POHYB_DOPREDU:
            if (i == 1)
            {
                dopredu = 0.225;
            }
            else if (i == 2)
            {
                dopredu = 0.225;
            }
            else if (i == 3)
            {
                dopredu = 0.20;
            }
            else
            {
                dopredu = 0.15;
            }
            i++;
            break;

        case POHYB_DOZADU:
            if (i == 1)
            {
                dozadu = 0.225;
            }
            else if (i == 2)
            {
                dozadu = 0.225;
            }
            else if (i == 3)
            {
                dozadu = 0.20;
            }
            else
            {
                dozadu = 0.15;
            }
            i++;
            break;

        case POHYB_VLEVO:
            if (i == 1)
            {
                vlevo = 0.225;
            }
            else if (i == 2)
            {
                vlevo = 0.225;
            }
            else if (i == 3)
            {
                vlevo = 0.20;
            }
            else
            {
                vlevo = 0.15;
            }
            i++;
            break;

        case POHYB_VPRAVO:
            if (i == 1)
            {
                vpravo = 0.225;
            }
            else if (i == 2)
            {
                vpravo = 0.225;
            }
            else if (i == 3)
            {
                vpravo = 0.20;
            }
            else
            {
                vpravo = 0.15;
            }
            i++;
            break;
        }
    }

    double kam = Random();
    if (kam <= dopredu)
    {
        return POHYB_DOPREDU;
    }
    else if (kam <= (dopredu + dozadu))
    {
        return POHYB_DOZADU;
    }
    else if (kam <= (dopredu + dozadu + vlevo))
    {
        return POHYB_VLEVO;
    }
    else if (kam <= (dopredu + dozadu + vlevo + vpravo))
    {
        return POHYB_VPRAVO;
    }
    else
    {
        return POHYB_STAT;
    }
}

class ZakaznikGenerator : public Event
{
    void Behavior()
    {

        int time_index = (((int)Time / 3600) % (END_TIME / 3600)); // vypočítej index časové doby
        double zakazniku_dany_index = (prumerny_pocet_zakazniku_za_den)*frekvence_navstevnosti[time_index];
        double pravdepodobnost_prichodu = (1.0 / (3600.0 * (1.0 / zakazniku_dany_index)));
        if (Random() <= pravdepodobnost_prichodu)
        {
            if (!SuperMarket.Full())
            {
                Navstevnost(time_index); // zapsat příchozího zákazníka do statistik
                new Zakaznik;            // aktivuj zákazníka
            }
        }
        Activate(Time + 1);
        //Activate(Time + (Random() * 60)); // naplánuj nové vygenerování zákazníka
    }
};

class TimeCheck : public Event
{
    int cnt = 0;

    void Behavior()
    {
        budova->zkontrolovat_pole();
        //cout << "Zákazníků celkem v obchodě: " << NUM_OF_CUSTOMERS << endl
        //     << endl;
        if (VIZUALIZOVAT)
        {
            cout << "-----------------------------------------------------------" << endl;
            budova->print_layout();
        }

        // Výpis průběhu simulace v procentech
        if (!VERSOBE)
        {
            if (cnt == 0)
            {
                cout << "0%" << endl;
            }
            else if (cnt == (int)END_TIME * 0.25)
            {
                cout << "25%" << endl;
            }
            else if (cnt == (int)END_TIME * 0.50)
            {
                cout << "50%" << endl;
            }
            else if (cnt == (int)END_TIME * 0.75)
            {
                cout << "75%" << endl;
            }
            else if (cnt == (int)END_TIME * 1.0 - 1)
            {
                cout << "100%" << endl;
            }
            cnt++;
        }

        this->Activate(Time + 1);
    }
};

Statistiky::Statistiky() {}
void Statistiky::Calc_and_print()
{
    Print("%d %d\n", (this->statistika_doba_vystaveni_nakaze / 60) / this->counter, (this->statistika_vic_nez_20s) / this->counter);
}

int main()
{

    // příkazy, které se netýkají přímo modelu:
    RandomSeed(time(NULL));
    // Pro vypisování do .dat souboru
    //SetOutput("");
    //statistika = new Statistiky();

    budova = new Budova();

    Init(START_TIME, END_TIME);

    // příkazy, které se týkají přímo modelu (tvorba objektů systému):

    cout << "--- Start of simulation ---" << endl;

    // počítač zákazníků nachazejících se v obchodě
    TimeCheck *check = new TimeCheck();
    check->Activate(1);

    // generátor zákazníků
    (new ZakaznikGenerator)->Activate();

    /* Slouží pro více iterací
    SuperMarket.Clear();
    VystaveniInfekci.Clear();
    CelkovyCasVystaveniInfekci.Clear();
    PomerInfikovanychAZdravych.Clear();
    Navstevnost.Clear();
    Vchod.Clear();
    */

    Run();

    // příkazy, které se provedou po dokončení simulace (zápis statistik apod.):
    cout << "--- End of simulation ---" << endl;
    SuperMarket.Output();
    VystaveniInfekci.Output();
    CelkovyCasVystaveniInfekci.Output();
    PomerInfikovanychAZdravych.Output();
    Navstevnost.Output();

    //CelkovyCasVystaveniInfekci.myOutput(statistika);
    //statistika->Calc_and_print();

    return 0;
}