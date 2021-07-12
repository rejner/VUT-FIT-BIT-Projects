
/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického
** výrazu do postfixového tvaru. Pro převod využijte zásobník (tStack),
** který byl implementován v rámci příkladu c202. Bez správného vyřešení
** příkladu c202 se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix .... konverzní funkce pro převod infixového výrazu 
**                       na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar .... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"
int solved;

/*
** Pomocná funkce untilLeftPar.
** Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka
** bude také odstraněna. Pokud je zásobník prázdný, provádění funkce se ukončí.
**
** Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
** znaků postExpr. Délka převedeného výrazu a též ukazatel na první volné
** místo, na které se má zapisovat, představuje parametr postLen.
**
** Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
** nadeklarovat a používat pomocnou proměnnou typu char.
*/
void untilLeftPar ( tStack* s, char* postExpr, unsigned* postLen ) {

  //Kontrola validního zásobníku
  if (s != NULL){

    //Pomocná proměnná uchovávající znak vrcholu zásobníku
    char top;
    stackTop(s, &top); //Nahrajeme znak do proměnné, ať máme co porovnávat

    //Jestliže na vrcholu zásobníku je jiný znak než levá závorka, budeme znaky sázet ze zásobníku
    //do pole postExp
    while (top != '(' && !stackEmpty(s)){
      
      //Extrahovaný operátir ze zásobníku vložím do výstupního pole postExpr na index daný postLen
      postExpr[*postLen] = top;
      //Inkrementujeme index výstupního pole
      ++*postLen;
      //Posunu zásobník, tím se dostanu na další znak v pořadí pro další iteraci
      stackPop(s);
      //Uložím hodnotu ze zásobníku do pom. proměnné
      stackTop(s, &top);           
    }

    //Odstraním poslední levou závorku na zásobníku, pro kterou jsem právě dokončil převod výrazu
    if (top == '(') stackPop(s);

  }

}

/*
** Pomocná funkce doOperation.
** Zpracuje operátor, který je předán parametrem c po načtení znaku ze
** vstupního pole znaků.
**
** Dle priority předaného operátoru a případně priority operátoru na
** vrcholu zásobníku rozhodneme o dalším postupu. Délka převedeného 
** výrazu a taktéž ukazatel na první volné místo, do kterého se má zapisovat, 
** představuje parametr postLen, výstupním polem znaků je opět postExpr.
*/
void doOperation ( tStack* s, char c, char* postExpr, unsigned* postLen ) {

  //Test zda jsem vůbec dostal správně inicializovaný zásobník
  if (s != NULL){

    //Pomocná proměnná pro znak na vrcholu zásobníku
    char top;

    //Zkontrolujeme zda není zásobník prázdný a vložíme znak na vrcholu do pomocné proměnné 'top'
    if (! stackEmpty(s)){ stackTop(s, &top);}
      
    //Test zda je na vrcholu zásobníku levá závorka, případně jestli
    //se na zásobníku aktuálně nachází operátor nižší priority než nový operátor, který
    //se nachází v proměnné 'c'
    if (stackEmpty(s) || top == '(' || ((c == '*' || c == '/') && (top == '+' || top == '-'))){
      
      stackPush(s,c); //vložíme nový operátor na zásobník

    }else{ //Jestliže se nenáchází na vrcholu levá závorka nebo operátor nižší priority

      //Do výstupního řetězce umístíme operátor na zásobníku
      postExpr[*postLen] = top;
      ++*postLen; //inkrementujeme index pro zápis

      //Posuneme zásobník, abychom se dostali k dalšímu znaku
      stackPop(s);

      //Funkci rekurzivně opakujeme
      doOperation(s, c, postExpr, postLen);
    }
  }
}

/* 
** Konverzní funkce infix2postfix.
** Čte infixový výraz ze vstupního řetězce infExpr a generuje
** odpovídající postfixový výraz do výstupního řetězce (postup převodu
** hledejte v přednáškách nebo ve studijní opoře). Paměť pro výstupní řetězec
** (o velikosti MAX_LEN) je třeba alokovat. Volající funkce, tedy
** příjemce konvertovaného řetězce, zajistí korektní uvolnění zde alokované
** paměti.
**
** Tvar výrazu:
** 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
**    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
**    násobení má stejnou prioritu jako dělení. Priorita násobení je
**    větší než priorita sčítání. Všechny operátory jsou binární
**    (neuvažujte unární mínus).
**
** 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
**    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
**
** 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
**    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
**    chybné zadání výrazu).
**
** 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen 
**    ukončovacím znakem '='.
**
** 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
**
** Poznámky k implementaci
** -----------------------
** Jako zásobník použijte zásobník znaků tStack implementovaný v příkladu c202. 
** Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
**
** Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
**
** Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
** char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
**
** Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
** nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
** by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
** ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
** řetězce konstantu NULL.
*/
char* infix2postfix (const char* infExpr) {

  //Ošetříme možnost chybně předaného pole s infoxovým výrazem
  if (infExpr == NULL) return NULL;

  //Vytvoříme nejdříve index countery pro naše výstupní a vstupní pole
  unsigned int infLen = 0;
  unsigned int postLen = 0;

  //Naalokujeme paměť pro nás post-fixový výraz
  char *postExpr = malloc (sizeof(char) * MAX_LEN);
  if (postExpr == NULL) return NULL; //Test jestli se správně alokovala paměť

  //Vytvoříme si zásobník a inicializujeme jej
  tStack s;
  stackInit(&s);

  char c = infExpr[infLen]; //Proměnná pro načítané znaky
  //Načteme znak po znaku vstup a pomocí switch konstrukce provádíme operace na znaky
  while (c != '='){

    //Pro rozhodování co s kterým znakem dělat skvěle poslouží konstrukce switch
    switch(c){
      
      //Levá závorka - uložíme do zásobníku
      case '(':

        stackPush(&s, c);

        break;

      //Pravá závorka - zavoláme pom. funkci untilLeftPar, která
      //se o ni postará
      case ')':

        untilLeftPar(&s, postExpr, &postLen);

        break;
      
      //Operátory - zavolá funkci doOperation, ta rozhodne co se s daným operátorem stane
      case '+':
      case '-':
      case '*':
      case '/':

        doOperation(&s, c, postExpr, &postLen);

        break;
      
      //Veškeré další znaky se uloží rovnou do pole s postfix výrazem
      default:
        
        postExpr[postLen] = c;
        ++postLen;

        break;

    }

    //Inkrementace počítadla pole infixního výrazu
    ++infLen;
    //Do proměnné c je nahrán další znak ke zpracování
    c = infExpr[infLen];

  }

  //Po načtení znaku '=' je potřeba pro jistotu vyprázdnit zásobník
  while(!stackEmpty(&s)){

    char t;
    stackTop(&s, &t);

    //Operátory bereme pouze po znak levé závorky
    if (t != '('){

      postExpr[postLen] = t;
      ++postLen;
      stackPop(&s);

    }else{

      break;

    }
  }

  //Na konec postfixu doplníme znak '=' a ukončíme string '\0'
  postExpr[postLen] = '=';
  postExpr[postLen+1] = '\0';

  //Vrátíme post-fixový výraz volající funkci
  return postExpr;
  
}

/* Konec c204.c */
