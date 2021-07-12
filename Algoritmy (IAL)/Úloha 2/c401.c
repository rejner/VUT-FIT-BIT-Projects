/* c401.c: **********************************************************}
{* Téma: Rekurzivní implementace operací nad BVS
**                                         Vytvořil: Petr Přikryl, listopad 1994
**                                         Úpravy: Andrea Němcová, prosinec 1995
**                                                      Petr Přikryl, duben 1996
**                                                   Petr Přikryl, listopad 1997
**                                  Převod do jazyka C: Martin Tuček, říjen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                                 Karel Masařík, říjen 2013
**                                                 Radek Hranický 2014-2018
**
** Implementujte rekurzivním způsobem operace nad binárním vyhledávacím
** stromem (BVS; v angličtině BST - Binary Search Tree).
**
** Klíčem uzlu stromu je jeden znak (obecně jím může být cokoliv, podle
** čeho se vyhledává). Užitečným (vyhledávaným) obsahem je zde integer.
** Uzly s menším klíčem leží vlevo, uzly s větším klíčem leží ve stromu
** vpravo. Využijte dynamického přidělování paměti.
** Rekurzivním způsobem implementujte následující funkce:
**
**   BSTInit ...... inicializace vyhledávacího stromu
**   BSTSearch .... vyhledávání hodnoty uzlu zadaného klíčem
**   BSTInsert .... vkládání nové hodnoty
**   BSTDelete .... zrušení uzlu se zadaným klíčem
**   BSTDispose ... zrušení celého stromu
**
** ADT BVS je reprezentován kořenovým ukazatelem stromu (typ tBSTNodePtr).
** Uzel stromu (struktura typu tBSTNode) obsahuje klíč (typu char), podle
** kterého se ve stromu vyhledává, vlastní obsah uzlu (pro jednoduchost
** typu int) a ukazatel na levý a pravý podstrom (LPtr a RPtr). Přesnou definici typů
** naleznete v souboru c401.h.
**
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c401.h"
int solved;

void BSTInit (tBSTNodePtr *RootPtr) {
/*   -------
** Funkce provede počáteční inicializaci stromu před jeho prvním použitím.
**
** Ověřit, zda byl již strom předaný přes RootPtr inicializován, nelze,
** protože před první inicializací má ukazatel nedefinovanou (tedy libovolnou)
** hodnotu. Programátor využívající ADT BVS tedy musí zajistit, aby inicializace
** byla volána pouze jednou, a to před vlastní prací s BVS. Provedení
** inicializace nad neprázdným stromem by totiž mohlo vést ke ztrátě přístupu
** k dynamicky alokované paměti (tzv. "memory leak").
**
** Všimněte si, že se v hlavičce objevuje typ ukazatel na ukazatel.
** Proto je třeba při přiřazení přes RootPtr použít dereferenční operátor *.
** Ten bude použit i ve funkcích BSTDelete, BSTInsert a BSTDispose.
**/

	//nastavíme kořen na NULL
	*RootPtr = NULL;

}

int BSTSearch (tBSTNodePtr RootPtr, char K, int *Content)	{
/*  ---------
** Funkce vyhledá uzel v BVS s klíčem K.
**
** Pokud je takový nalezen, vrací funkce hodnotu TRUE a v proměnné Content se
** vrací obsah příslušného uzlu.´Pokud příslušný uzel není nalezen, vrací funkce
** hodnotu FALSE a obsah proměnné Content není definován (nic do ní proto
** nepřiřazujte).
**
** Při vyhledávání v binárním stromu bychom typicky použili cyklus ukončený
** testem dosažení listu nebo nalezení uzlu s klíčem K. V tomto případě ale
** problém řešte rekurzivním volání této funkce, přičemž nedeklarujte žádnou
** pomocnou funkci.
**/

	//otestujeme, jestli není RootPtr nastaven na NULL
	//-> v takovém případě je strom buď prázdný nebo některé
	//   rekurzivní volání funkce došlo na konec stromu
	if (RootPtr != NULL){

		//klíč je menší než klíč v aktuálním uzlu
		if (K < RootPtr->Key){

			//voláme BSTSearch znova pro uzel nalevo od aktuálního uzlu
			return BSTSearch(RootPtr->LPtr, K, Content);
		}
		//klíč je větší než klíč v aktuálním uzlu
		else if (K > RootPtr->Key){

			//voláme BSTSearch znova pro uzel nalevo od aktuálního uzlu
			return BSTSearch(RootPtr->RPtr, K, Content);
		}
		//našli jsme hledaný uzel
		else if (K == RootPtr->Key){

			//předáme hodnotu přes proměnnou Content
			*Content = RootPtr->BSTNodeCont;
			return TRUE;
		}


 	//strom je buď prázdný nebo jsme došli na konec stromu
	}else{

		return FALSE;
	}

	//pouze pro uspokojeni překladače, sem by
	//se program nikdy neměl dostat
	return FALSE;


}


void BSTInsert (tBSTNodePtr* RootPtr, char K, int Content)	{
/*   ---------
** Vloží do stromu RootPtr hodnotu Content s klíčem K.
**
** Pokud již uzel se zadaným klíčem ve stromu existuje, bude obsah uzlu
** s klíčem K nahrazen novou hodnotou. Pokud bude do stromu vložen nový
** uzel, bude vložen vždy jako list stromu.
**
** Funkci implementujte rekurzivně. Nedeklarujte žádnou pomocnou funkci.
**
** Rekurzivní implementace je méně efektivní, protože se při každém
** rekurzivním zanoření ukládá na zásobník obsah uzlu (zde integer).
** Nerekurzivní varianta by v tomto případě byla efektivnější jak z hlediska
** rychlosti, tak z hlediska paměťových nároků. Zde jde ale o školní
** příklad, na kterém si chceme ukázat eleganci rekurzivního zápisu.
**/

	//jestliže je strom prázdný, vytvoříme první uzel a vložíme do něj hodnoty
	if (*RootPtr == NULL){

		*RootPtr = malloc (sizeof(struct tBSTNode));

		if (*RootPtr == NULL) return; //test zdaření aloakce

		//nastavení hodnot uzlu
		(*RootPtr)->Key = K;
		(*RootPtr)->BSTNodeCont = Content;
		//odkazy na další uzly jsou zatím prázdné
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;
		
	}

	//jestliže je hodnota klíče menší než hodnota klíče v aktuálním uzlu
	else if (K < (*RootPtr)->Key){

		//volám rekurzivně funkci znova pro levý podstrom
		BSTInsert(&(*RootPtr)->LPtr, K, Content);
	}

	//jestliže je hodnota klíče menší než hodnota klíče v aktuálním uzlu
	else if (K > (*RootPtr)->Key){

		//volám rekurzivně funkci znova pro pravý podstrom
		BSTInsert(&(*RootPtr)->RPtr, K, Content);

	}

	//klíče se rovnají, přepíšeme obsah aktuálního uzlu
	else{

		(*RootPtr)->BSTNodeCont = Content;
	}

}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
/*   ------------------
** Pomocná funkce pro vyhledání, přesun a uvolnění nejpravějšího uzlu.
**
** Ukazatel PtrReplaced ukazuje na uzel, do kterého bude přesunuta hodnota
** nejpravějšího uzlu v podstromu, který je určen ukazatelem RootPtr.
** Předpokládá se, že hodnota ukazatele RootPtr nebude NULL (zajistěte to
** testováním před volání této funkce). Tuto funkci implementujte rekurzivně.
**
** Tato pomocná funkce bude použita dále. Než ji začnete implementovat,
** přečtěte si komentář k funkci BSTDelete().
**/

	

	//pro správnou činnost této funkce není přípustné, aby byl buď
	//aktuálně procházený uzel nebo uzel pro uložení hodnoty neinicializovaný
	if (*RootPtr == NULL || PtrReplaced == NULL) return;

	//jestliže RPtr odkazuje ještě na další uzel
	if ((*RootPtr)->RPtr != NULL){

		//funkci voláme rekurzivně znovu
		ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);

	}else{ //našli jsme poslední uzel v pravé části BST

		//před smazáním uzlu přesuneme klíč a hodnotu do uzlu, kam
		//nám odkazuje PtrReplaced
		PtrReplaced->Key = (*RootPtr)->Key;
		PtrReplaced->BSTNodeCont = (*RootPtr)->BSTNodeCont; 

		//smažeme uzel
		BSTDelete(RootPtr, (*RootPtr)->Key);


	}

}

void BSTDelete (tBSTNodePtr *RootPtr, char K) 		{
/*   ---------
** Zruší uzel stromu, který obsahuje klíč K.
**
** Pokud uzel se zadaným klíčem neexistuje, nedělá funkce nic.
** Pokud má rušený uzel jen jeden podstrom, pak jej zdědí otec rušeného uzlu.
** Pokud má rušený uzel oba podstromy, pak je rušený uzel nahrazen nejpravějším
** uzlem levého podstromu. Pozor! Nejpravější uzel nemusí být listem.
**
** Tuto funkci implementujte rekurzivně s využitím dříve deklarované
** pomocné funkce ReplaceByRightmost.
**/

	//strom je buď prázdný nebo uzel s hledaným klíčem neexistuje
	if (*RootPtr == NULL) return;

	//hledaný klíč je menší než klíč v aktuálním uzlu
	if (K < (*RootPtr)->Key){

		//hledáme dál v levém podstromu
		BSTDelete(&(*RootPtr)->LPtr, K);

	}
	//hledáný klíč je větší než klíč v aktuálním uzlu
	else if (K > (*RootPtr)->Key){

		//hledáme dál v pravém podstromu
		BSTDelete(&(*RootPtr)->RPtr, K);

	}
	//hledaný klíč je totožný s klíčem uzlu
	else if (K == (*RootPtr)->Key){

		/*Nyní mohou nastat 4 případy:

		  1. Uzel je koncový a nemá žádné další potomky
		  2. Uzel má pouze levé potomky
		  3. Uzel má pouze pravé potomky
		  4. Uzel má levé i pravé potomky

		(jednotlivé části kodu pro daný případ budou takhle očíslovány)*/

		// 1.
		if ((*RootPtr)->LPtr == NULL && (*RootPtr)->RPtr == NULL){

			//uvolním uzel, mám jistotu, že žádné další uzly nezůstanou viset
			free(*RootPtr);
			*RootPtr = NULL;

		}
		// 2.
		else if ((*RootPtr)->LPtr != NULL && (*RootPtr)->RPtr == NULL){

			//uložíme odkaz na levý uzel do pomocné proměnné a uvolníme uzel
			tBSTNodePtr left_node = (*RootPtr)->LPtr;
			free(*RootPtr);
			//vymazaný uzel můžeme bez problému nahradit jeho levým potomkem
			*RootPtr = left_node;

		}
		// 3.
		else if ((*RootPtr)->RPtr != NULL && (*RootPtr)->LPtr == NULL){

			//uložíme odkaz na pravý uzel do pomocné proměnné a uvolníme uzel
			tBSTNodePtr right_node = (*RootPtr)->RPtr;
			free(*RootPtr);
			//vymazaný uzel můžeme bez problému nahradit jeho levým potomkem
			*RootPtr = right_node;

		}
		// 4.
		else if (((*RootPtr)->RPtr != NULL && (*RootPtr)->LPtr != NULL)){

			/*Existují 2 metody jak se s tímto problémem vypořádat:
			
				1. Námi odstraňovaný uzel nahradím uzlem s maximální hodnotou v levém podstromu
				2. -||- s minimální hodnotou v pravém podstromu
			
				Funkce ReplaceByRightMost nám dokáže najít uzel pro případ 1.
				-> použijeme tedy řešení 1.
			*/

			//ošetříme případ, kdy levý potomem odstraňovaného uzlu
			//bezprostředně nemá žádný pravý podstrom, v takovím případě pouze
			//nahradím smazaný uzel jeho pravým potomkem
			if ((*RootPtr)->LPtr->RPtr == NULL){

				tBSTNodePtr left_node = (*RootPtr)->LPtr;
				tBSTNodePtr right_node = (*RootPtr)->RPtr;

				free(*RootPtr);

				//nahradíme smazaný uzel jeho levým potomkem a ještě mu přidáme
				//odkaz na pravého potomka smazaného uzlu
				*RootPtr = left_node;
				(*RootPtr)->RPtr = right_node; 

			//musíme vyhledat maximum v levém podstromu a nahradit jím smazaný uzel
			}else{
				
				//na místo smazaného uzlu nahradí nejpravější uzel pravého podstromu levého potomka
				ReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr->RPtr);
			}

		}
		
	}

}

void BSTDispose (tBSTNodePtr *RootPtr) {
/*   ----------
** Zruší celý binární vyhledávací strom a korektně uvolní paměť.
**
** Po zrušení se bude BVS nacházet ve stejném stavu, jako se nacházel po
** inicializaci. Tuto funkci implementujte rekurzivně bez deklarování pomocné
** funkce.
**/
	
	//prázdný strom už je prázdný, nic neděláme
	if (*RootPtr == NULL) return;

	//zavoláme funkci Dispose pro levý podstrom
	BSTDispose(&(*RootPtr)->LPtr);

	//zavoláme funkci Dispose pro pravý podstrom
	BSTDispose(&(*RootPtr)->RPtr);

	//uvolníme uzel (podstrom)
	free(*RootPtr);
	*RootPtr = NULL;

}

/* konec c401.c */

