
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2019
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    
	
 L->First = NULL;
 L->Last = NULL;
 L->Act = NULL;

}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	
    /*Inicializace proměnných potřebných pro projití seznamem,
      do aktuální položky currentElement si rovnou nahraju první prvek seznamu*/
    tDLElemPtr currentElement = L->First, nextElement;

    /*Procházím seznam do té doby, dokud nenarazím na neplatný odkaz*/
    while (currentElement != NULL){

        nextElement = currentElement->rptr; //z aktuálně procházeného prvku si nahraju odkaz na další prvek do proměnné nexElement
        free(currentElement); //zruším aktuálně procházený prvek
        currentElement = nextElement; //nahraju další následující prvek do proměnné s aktuálně procházeným prvkem

    }

    /*Až projdu celý list, znovu inicializuji seznam*/
    L->First = NULL;
    L->Last = NULL;
    L->Act = NULL;
 
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
    /*Inicalizace pomocné proměnné pro starý prvek na začátku seznamu (oldFirst), pro
     nový prvek alokuji paměť (newFirst)*/
    tDLElemPtr oldFirst, newFirst = malloc(sizeof(struct tDLElem));

    /*Test jestli se povedla alokace paměti pro nový prvek*/
    if (newFirst == NULL){

        DLError(); 

    }else{ //Alokace se povedla

        

        /*Test jestli se jedná o prázdný seznam*/
        if (L->First == NULL){

            /*připravím si novou položku a nahraji do ní data*/
            newFirst->data = val;      //data
            newFirst->lptr = NULL;     //položka má být první v seznamu, nic před ní být nemůže
            newFirst->rptr = NULL;     //odkaz ná následující položku (tedy ta původní první položka v seznamu)
            
            /*Jestliže je seznam prázdný, nová položka bude jak první, tak i poslední položkou*/
            L->First = newFirst; //nastavím novou položku jako první v celém seznamu
            L->Last = newFirst;  //nastavím novou položku jako poslední v celém seznamu

        }else{

            /*do proměnné si nahraji starý prvek na začátku seznamu*/
            oldFirst = L->First;

            newFirst->data = val;         //data
            newFirst->lptr = NULL;        //položka má být první v seznamu, nic před ní být nemůže
            newFirst->rptr = oldFirst;    //odkaz ná následující položku (tedy ta původní první položka v seznamu)
            
            /*Jestliže již seznam obsahuje nějaké položky, nová položka bude pouze první*/
            oldFirst->lptr = newFirst; //nastavím novou položku jako předchozí prvek staré první položky

            L->First = newFirst; //nastavím novou položku jako první v celém seznamu

        }     

    }
 
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	
	
    /*Funkce je velmi podobná funkci DLInsertFirst*/

   /*Inicalizace pomocné proměnné pro starý prvek na konci seznamu (oldLast), pro
     nový prvek alokuji paměť (newLast)*/
    tDLElemPtr oldLast, newLast = malloc(sizeof(struct tDLElem));

    /*Test jestli se povedla alokace paměti pro nový prvek*/
    if (newLast == NULL){

        DLError(); 

    }else{ //Alokace se povedla     

        /*Test jestli se jedná o prázdný seznam*/
        if (L->Last == NULL){

            /*připravím si novou položku a nahraji do ní data*/
            newLast->data = val;     //data
            newLast->lptr = NULL; //odkaz ná předchozí položku (tedy ta původní poslední položka v seznamu)
            newLast->rptr = NULL;    //položka má být poslední v seznamu, nic před ní být nemůže
            
            /*Jestliže je seznam prázdný, nová položka bude jak první, tak i poslední položkou*/
            L->First = newLast; //nastavím novou položku jako první v celém seznamu
            L->Last = newLast;  //nastavím novou položku jako poslední v celém seznamu

        }else{

            /*do proměnné si nahraji starý prvek na začátku seznamu*/
            oldLast = L->Last;

            /*připravím si novou položku a nahraji do ní data*/
            newLast->data = val;     //data
            newLast->lptr = oldLast; //odkaz ná předchozí položku (tedy ta původní poslední položka v seznamu)
            newLast->rptr = NULL;    //položka má být poslední v seznamu, nic před ní být nemůže
            
            /*Jestliže již seznam obsahuje nějaké položky, nová položka bude pouze poslední*/
            oldLast->rptr = newLast; //nastavím novou položku jako následující prvek staré poslední položky
            L->Last = newLast;  //nastavím novou položku jako poslední v celém seznamu


        }

    }
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
    L->Act = L->First;

}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
    L->Act = L->Last;

}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

    /*Test zda seznam není prázdný (takový seznam neobsahuje první prvek)*/
    if (L->First == NULL){

        DLError();

    }else{ //seznam něco obsahuje

        *val = L->First->data; //přesun dat první položky v seznamu přes odkaz proměnné 'val'

    }
	
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	
	/*Test zda seznam není prázdný (takový seznam neobsahuje první prvek)*/
    if (L->Last == NULL){

        DLError();

    }else{ //seznam něco obsahuje

        *val = L->Last->data; //přesun dat první položky v seznamu přes odkaz proměnné 'val'

    }

}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	
    tDLElemPtr next; //do této proměnné uložím odkaz na položku, na kterou odkazuje prvek první (tedy 2. položku)

    /*Test jestli seznam vůbec obsahuje nějaké prvky*/
    if (L->First != NULL){

        /*Uložím si odkaz na další prvek*/
        next = L->First->rptr;

        /*Test na shodu aktivního prvku s prvním*/
        if (L->First == L->Act){

            L->Act = NULL; //ztráta aktivity
        }

        /*Test zda vůbec existuje nějaký další prvek*/
        if (next != NULL){
            
            //Jestliže existuje, zruší se návaznost na předchozí první prvek
            next->lptr = NULL;

            free(L->First); //uvolnění prvního prvku
            L->First = next; //druhý prvek se stává prvním

        }else{

            L->Act = NULL;
            L->First = NULL;
            L->Last = NULL;
            free(L->First); //uvolnění prvního prvku

        }

        

    }
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	
	
  tDLElemPtr previous; //do této proměnné uložím odkaz na položku, na kterou odkazuje lptr posledního prvku (tedy předposlední položku)

    /*Test jestli seznam vůbec obsahuje nějaké prvky*/
    if (L->Last != NULL){

        /*Uložím si odkaz na předchozí prvek*/
        previous = L->Last->lptr;

        /*Test na shodu aktivního prvku s posledním*/
        if (L->Last == L->Act){

            L->Act = NULL; //ztráta aktivity

        }

        /*Test zda vůbec existuje nějaký předchozí prvek*/
        if (previous != NULL){
            
            //Jestliže existuje, zruší se návaznost na další (poslední) prvek
            previous->rptr = NULL;

            free(L->Last); //uvolnění posledního prvku
            L->Last = previous; //předposlední prvek se stává posledním
            
        }else{

            L->Last = NULL;
            L->First = NULL;
            L->Act = NULL;
            free(L->Last); //uvolnění posledního prvku

        }

        
        
    }
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	
    /*Pomocná proměnná pro uchování odkazu prvku za prvkem, který budeme odstraňovat*/
    tDLElemPtr tmp;

    /*Test zda sesznam něco obsahuje a zda aktivní prvek náhodou není posledním prvkem*/
    if (L->Act != NULL && L->Act != L->Last){

        tmp = L->Act->rptr->rptr; //uložíme si adresu prvku za prvkem, který budeme odstraňovat

        /*Test jestli není další prvek náhodou poslední*/
        if (L->Act->rptr == L->Last){

            L->Last = L->Act; //Jestliže ano, aktivní prvek se stává i posledním

        }else{

            tmp->lptr = L->Act; //Aktivní prvek se stává prvkem předchozím pro náš prvek v tmp
        }

        free (L->Act->rptr); //Uvolnění prvku po aktivním prvku
        L->Act->rptr = tmp; //Prvek po odstraněném prvku se stává následujícím prvkem prvku aktivního
    }
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
			
  /*Pomocná proměnná pro uchování odkazu prvku před prvkem, který budeme odstraňovat*/
    tDLElemPtr tmp;

    /*Test zda sesznam něco obsahuje a zda aktivní prvek náhodou není prvním prvkem*/
    if (L->Act != NULL && L->Act != L->First){

        tmp = L->Act->lptr->lptr; //Uložíme si adresu prvku před prvkem, který budeme odstraňovat

        /*Test jestli není předchozí prvek náhodou první*/
        if (L->Act->lptr == L->First){

            L->First = L->Act; //Jestliže ano, aktivní prvek se stává i prvním

        }else{

            tmp->rptr = L->Act; //Aktivní prvek se stává prvkem následujícím pro náš prvek v tmp
        }

        free (L->Act->lptr); //Uvolnění prvku před aktivním prvkem
        L->Act->lptr = tmp; //Prvek před odstraněným prvkem se stává předchozím prvkem prvku aktivního

    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
    /*Test jestli je seznam aktivní*/
    if (L->Act != NULL){
        
        /*new = nový prvek, next = prvek po aktivním prvku*/
        tDLElemPtr new = malloc(sizeof(struct tDLElem)), next;

        /*Test zda alokace dopadla v pořádku*/
        if (new == NULL){

            DLError();

        }else{

            next = L->Act->rptr; //Uložím si odkaz na prvek po aktuálním prvku

            /*Inicalizace prvku*/
            new->data = val;     //Uložím hodnotu do nového prvku
            new->lptr = L->Act;  //Předám odkaz na předchozí prvek
            new->rptr = next;    //Předám odkaz na následující prvek

            L->Act->rptr = new; //Nový prvek se stává následujícím prvkem prvku aktivního

            /*Test zda aktivní prvek je i posledním prvkem*/
            if (L->Act != L->Last){
                
                next->lptr = new; //Jestliže není, předám odkaz na nově vytvořený prvek prvku následujícímu

            }else{ //Nový prvek je nyní i poslední

                L->Last = new;

            }  
        }
    }
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
	
    /*Test jestli je seznam aktivní*/
    if (L->Act != NULL){
        
        /*new = nový prvek, previous = prvek před aktivním prvkem*/
        tDLElemPtr new = malloc(sizeof(struct tDLElem)), previous;

        /*Test zda alokace dopadla v pořádku*/
        if (new == NULL){

            DLError();

        }else{

            previous = L->Act->lptr; //Uložím si odkaz na prvek před aktuálním prvkem

            /*Inicalizace prvku*/
            new->data = val;     //Uložím hodnotu do nového prvku
            new->lptr = previous;  //Předám odkaz na předchozí prvek
            new->rptr = L->Act;    //Předám odkaz na následující prvek

            L->Act->lptr = new; //Nový prvek se stává předchozím prvkem prvku aktivního

            /*Test zda aktivní prvek je i prvním prvkem*/
            if (L->Act != L->First){
                
                previous->rptr = new; //Jestliže není, předám odkaz na nově vytvořený prvek prvku předchozímu


            }else{ //Nový prvek je nyní i prvním

                L->First = new;

            }

        }

    }
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
		
    /*Test jestli je nějaký aktivní prvek (jinak neaktivní seznam)*/    
    if (L->Act != NULL){

        *val = L->Act->data; //Předání hodnoty přes parametr

    }else{

        DLError();

    }
	
 
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	
	
 if (L->Act != NULL){

        L->Act->data = val; //Nahraju hodnotu do prvku

    }

}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	
	
 if (L->Act != NULL){

        //Jestliže je aktivní prvek poslední
        if (L->Act == L->Last){

            L->Act = NULL; //Deaktivace aktivního prvku -> a tím i seznamu

        }else{

            L->Act = L->Act->rptr; //Posun na další prvek

        }

    }

}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	
	
 if (L->Act != NULL){

        //Jestliže je aktivní prvek první
        if (L->Act == L->First){

            L->Act = NULL; //Deaktivace aktivního prvku -> a tím i seznamu

        }else{

            L->Act = L->Act->lptr; //Posun na další prvek

        }

    }

}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	
	
    return (L->Act != NULL) ? 1 : 0;
}

/* Konec c206.c*/
