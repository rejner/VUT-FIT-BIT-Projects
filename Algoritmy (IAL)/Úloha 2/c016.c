/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, 2014-2018
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {


	if (*ptrht != NULL){

		unsigned int i = 0;

		while (i < HTSIZE){

			(*ptrht)[i] = NULL;
			i++;

		}
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není,
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {

	//uložíme si index kde by měl prvek být
	unsigned int slot_index = hashCode(key);

	//uložíme si položku do nové proměnné
	tHTItem *item = (*ptrht)[slot_index];

	//jestli na indexu nic není, vrací hodnotu NULL
	if (item == NULL){
		return NULL;
	}

	while( item != NULL){
		
		//zkusíme jestli tento prvek není právě náš hledaný
		if (strcmp(item->key, key) == 0){
			
			//jestliže je, vrátíme na něj ukazatel
			return item;
		}

		//jdeme hledat dál
		item = item->ptrnext;

	}

	//prvek nebyl nalezen, vracíme NULL
	return NULL;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {

	//zavoláme hashovací funkci nad požadovaným klíčem a zjistíme si index kam patří
	unsigned int slot_index = hashCode(key);

	//načteme si prvek na daném indexu do nové položky
	tHTItem *item = htSearch(ptrht, key);

	//jestliže je na požadovaném indexu volné místo, zapíšeme rovnou
	//novou položku na daný index v hashovací tabulce a vrátíme se z funkce
	if (item == NULL){

		//alokujeme si místo pro položku
		item = malloc(sizeof(tHTItem));

		//test správné alokace
		if (item == NULL){
			free(item);
			return;
		}

		//alokujeme místo pro klíč
		item->key = malloc(strlen(key) + 1);

		//test správné alokace
		if (item->key == NULL){
			free(item->key);
			free(item);
			return;
		}

		//vložíme klíč a data na své místo ve struktuře tHTItem
		strcpy(item->key, key);
		item->data = data;

		//ukazatel na další prvek bude zatím nevyužitý -> nastavíme na NULL
		item->ptrnext = NULL;

		//jestliže je na začátku indexu volno
		if ((*ptrht)[slot_index] == NULL){

			//uložíme položku na začátek index v tabulce
			(*ptrht)[slot_index] = item; 

		}else{

			//načtu si první položku na indexu
			tHTItem *first_item = (*ptrht)[slot_index];
			
			//nová položku bude odkazovat na první
			item->ptrnext = first_item;

		}

		//změníme první položku indexu na nový prvek
		(*ptrht)[slot_index] = item;


	}else{

		//updatuji data
		item->data = data;

	}

}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {

	//najdeme se požadovaný klíč
	tHTItem *item = htSearch(ptrht, key);

	if (item == NULL){

		return NULL;
	}

	//vrátíme hodnotu v data
	return &item->data;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {

	unsigned int slot_index = hashCode(key);

	tHTItem *item = (*ptrht)[slot_index];
	

	//jestliže je index prázdný, nic neděláme
	if (item == NULL){

		return;
	}

	//vytvoříme si pomocnou položku pro zpětné provázání seznamu po uvolnění některé z položek
	tHTItem *prev_item = NULL;

	while (item != NULL){

		//otestujeme jestli jsme nenašli náhodou shodný klíč
		if (strcmp(item->key, key) == 0){

			//prvek který jsme našli je první v seznamu, nemá tedy předchozí prvek
			if (prev_item == NULL){

				//začátku indexu předáme ukazatel na prvek za prvním prvkem
				(*ptrht)[slot_index] = item->ptrnext;

				
				//uvolníme klíč a samotnou položku
				free(item->key);
				free(item);
				return;


			}else{

				//převážeme seznam
				prev_item->ptrnext = item->ptrnext;

				//uvolníme klíč a samotnou položku
				free(item->key);
				free(item);

				//vyskočíme z funkce
				return;

			}			

		}

		//uložíme si aktuální prvek do pomocné proměnné a jdeme po ukazateli na další položku
		prev_item = item;
		item = prev_item->ptrnext;

	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {

	tHTItem *item, *next_item;
	unsigned int i = 0;

	//postupně projdeme všechny indexy tabulky a zřetězené prvky na indexech
	//všechny nalezené prvky uvolníme a index nastavíme na NULL
	while (i < HTSIZE){

		item = (*ptrht)[i];

		//opakujeme, dokud nedojdeme na konec zřetězých prvků na indexu
		while(item != NULL){

			//uložíme si odkaz na další prvek a aktuální prvek zrušíme
			next_item = item->ptrnext;
			free(item->key);
			free(item);
			item = next_item;

		}

		//nastavíme index na NULL (idex už je prázdný)
		(*ptrht)[i] = NULL;
		i++;
	}
}
