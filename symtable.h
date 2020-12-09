/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 *        Filip Vágner, xvagne08
 */

#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "stdarg.h"

#include "nodetype.h"
#include "error.h"
#include "str.h"
#include "isusedlist.h"

extern struct tStack stack;
extern struct tNode* currentFuncNode;

/**
 * Uzel binárího stromu - prvek tabulky symbolů
 */
typedef struct tNode {
    char* key;			    // Klíč
    struct tNode* lptr;	    // Levá větev
    struct tNode* rptr;	    // Pravá větev

    nodeType type;          // Typ identifikátoru
    bool isDefined;         // Byl identifikátor definovaný 
    int param;              // Počet parametrů (jde-li o funkci), Pozice parametru (jde-li o parametr funkce)
    struct tNode* localTS;  // Ukazatel na tabulku symbolů lokální úrovně (jde-li o funkci)
    struct retType* retTypes; //Návratové typy (jedná-li se o funkci)
} TNode;

typedef struct tTree {
    struct tNode* root;
    struct tNode* last;
} TTree;

/**
 * Prvek zásobníku tabulek symbolů
 */
typedef struct tStack_elem {
    int scope;
    struct tNode* node;
    struct tStack_elem* next;
} TStack_Elem;

/**
 * Zásobník tabulek symbolů
 */
typedef struct tStack {
    struct tStack_elem* top;
    struct tStack_elem* bottom;
} TStack;

typedef struct retType {
    nodeType type;
    struct retType* next;
} RetType;

/**
 * Inicializuje tabulku symbolů
 * @param root Kořen binárního stromu
 */
void TSInit(TTree *tree);

/**
 * Rekurzivně vloží prvek do tabulky symbolů
 * @param root Ukazatel na kořen stromu
 * @param key Klíč nového uzlu
 * @param type Typ nového uzlu
 * @param isDefined Byl identifikátor nového uzlu definovaný?
 * @param param Počet parametrů (jde-li o funkci), Pozice parametru (jde-li o parametr funkce)
 * @param localTS Ukazatel na tabulku symbolů lokální úrovně (jde-li o funkci)
 */
TNode* TSInsert(TNode** root, char* key, nodeType type, bool isDefined, int param, TNode* localTS);

/**
 * Rekurzivně vyhledá prvek dle klíče
 * @param root Ukazatel na kořen stromu
 * @param key Vyhledávaný klíč
 */
TNode* TSSearch(TNode* root, char* key);

/**
 * Rekurzivně vyhledá prvek dle počtu parametrů, nebo pozice parametru, pokud je předána lokální tabulka funkce
 * @param root Ukazatel na kořen stromu
 * @param paramPos Vyhledávaná hodnota
 */
TNode* TSSearchByParam(TNode* root, int paramPos);

/**
 * Hledá klíč v rámci celého zásobníku a vrací ukazatel na prvek s hledaným klíčem
 * @param stackElem Prvek zásobníku
 * @param key Hledaný klíč
 * @param scope Adresa čísla, kam se má uložit číslo rámce hledaného prvku
 * @return Ukazatel na prvek s daným klíčem, pokud takový prvek neexistuje, vrací NULL
 */
TNode* TSSearchStackAndReturn(TStack_Elem* stackElem, char* key, int* scope);

/**
 * Projde tabulku symbolů a kontroluje, zda splňuje podmínky
 * @param root Ukazatel na kořen stromu
 * @param type Typ, který mají všechny prvky tabulky symbolů mít
 * @param isDefined 
 * @return 1 pokud se všechny hodnoty v tabulce symbolů platí, že node.type == type && node.isDefined = isDefined, 0 jinak
 */
int TSAllMeetsConditions(TNode* root, nodeType type, bool isDefined);

/**
 * Zruší celý strom
 * @param root Ukazatel na kořen stromu
 */
void TSDispose(TNode* root);

/**
 * Hledá klíč v rámci celého zásobníku
 * @param stackElem Prvek zásobníku
 * @param key Hledaný klíč
 * @return 1 v případě nalezení klíče, 0 jinak
 */
int TSSearchStack(TStack_Elem* stackElem, char* key);

/**
 * Hledá klíč v rámci celého zásobníku, ovšem ignorují se klíče s typem FUNC.
 * @param node Tabulka symbolů
 * @param key Hledaný klíč
 * @return 1 v případě nalezení klíče, 0 jinak
 */
TNode* TSSearchStackExceptFunc(TStack_Elem* stackElem, char* key);

/**
 * Hledá klíč v rámci celého zásobníku
 * @param stackElem Prvek zásobníku
 * @param key Hledaný klíč
 * @return Vrátí číslo zásobníku, ve kterém se nachází hledaný prvek
 */
int TSSearchStackAndReturnScope(TStack_Elem* stackElem, char* key);

/**
 * Vloží prvek do tabulky symbolů pouze pokud se v tabulce symbolů doposud nenachází, jinak exit
 * @param root Ukazatel na kořen stromu
 * @param key Klíč nového uzlu
 * @param type Typ nového uzlu
 * @param isDefined Byl identifikátor nového uzlu definovaný?
 * @param param Počet parametrů (jde-li o funkci), Pozice parametru (jde-li o parametr funkce)
 * @param localTS Ukazatel na tabulku symbolů lokální úrovně (jde-li o funkci)
 */
void TSInsertOrExitOnDuplicity(TNode** root, char* key, nodeType type, bool isDefined, int param, TNode* localTS);

/**
 * Ukončí program, pokud není daný klíč definovaný
 * @param stacKElem Ukazatel na prvek zásobníku
 * @param key Klíč, pro který se provádí kontrola
 * @param canBeFunc Mají/Nemají se do kontroly zahrnovat prvky, jejichž typ je FUNC
 */
void TSExitIfNotDefined(TStack_Elem* stackElem, char* key, bool canBeFunc);

/**
 * Vloží funkci do tabulky symbolů, pokud ještě nebyla definována
 * Pokud již v tabulkce symbolů funkce je, tak se provede kontrola, jestli sedí počty/typy parametrů
 * @param stacKElem Ukazatel na prvek zásobníku
 * @param key Klíč
 * @param param Počet parametrů funkce Mají/Nemají se do kontroly zahrnovat prvky, jejichž typ je FUNC
 * @param localTS Lokální tabulka symbolů
 * @param retValCnt Ukazatel, přes který se vrací počet návratových hodnot
 * @param isUsedList Ukazatele na strukturu IsUsedList, kde jsou uloženy identifikátory levé strany
 * @param retType Ukazatele na strukturu retType, kde jsou uloženy návratové typy funkce
 * @param def true, pokud se jedná o definici funkce, jinak false
 */
void TSInsertFuncOrCheck(TStack_Elem* stackElem, char* key, int param, TNode* localTS, int* retValCnt, IsUsedList* isUsedList, RetType* retType, bool def);

/**
 * Porovná dvě tabulky symbolů (param, nodeType)
 * @param firstNode
 * @param secondNode
 */
bool TSCompare(TNode* firstNode, TNode* secondNode);

/**
 * Inicializace zásobníku tabulek symbolů
 * @param stack Ukazatel na zásobník
 */
void TStackInit(TStack* stack);

/**
 * Vložení nového prvku zásobníku - rámce
 * @param stack Ukazatel na zásobník
 */
void PushFrame(TStack* stack);

/**
 * Odstranění prvku zásobníku - rámce
 * @param stack Ukazatel na zásobník
 */
void PopFrame(TStack* stack);

/**
 * Výpis prvků tabulky symbolů, kromě prvků lokálních tabulek symbolů
 * @param root Tabulka symbolů
 */
void TSPrint(TNode* root);

/**
 * Přidá návratový typ 
 * @param retType Seznam návratových typů, do kterého se má přidávat
 * @param type Typ
 */
void addRetType(RetType** retType, nodeType type);

/**
 * Přidá více návratových typů
 * @param retType Seznam návratových typů, do kterého se má přidávat
 * @param num Počet přidávaných návratových typů
 */
void addMultipleRetType(RetType** retType, int num, ...);

/**
 * Spočítá návratové typy daného uzlu
 * @param node Uzel binárního stromu
 */
int countRetTypes(TNode* node);