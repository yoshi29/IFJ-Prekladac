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

#include "error.h"
#include "str.h"

extern struct tStack stack;

/*
 * Typ identifikátoru
 */
typedef enum {
    /*0*/ INT,
    /*1*/ FLOAT,
    /*2*/ STRING,
    /*3*/ FUNC
} nodeType;

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
    struct tNode* node;
    struct tStack_elem* next;
} TStack_Elem;

/**
 * Zásobník tabulek symbolů
 */
typedef struct tStack {
    struct tStack_elem* top;
} TStack;

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
void TSInsert(TNode** root, char* key, nodeType type, bool isDefined, int param, TNode* localTS);

/**
 * Rekurzivně vyhledá prvek dle klíče
 * @param root Ukazatel na kořen stromu
 * @param key Vyhledávaný klíč
 */
TNode* TSSearch(TNode* root, char* key);

/**
 * Hledá klíč v rámci celého zásobníku a vrací ukazatel na prvek s hledaným klíčem
 * @param stackElem Prvek zásobníku
 * @param key Hledaný klíč
 * @return Ukazatel na prvek s daným klíčem, pokud takový prvek neexistuje, vrací NULL
 */
TNode* TSSearchStackAndReturn(TStack_Elem* stackElem, char* key);

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
 * Hledá klíč v rámci lokálních tabulek tabulky symbolů
 * @param node Tabulka symbolů
 * @param key Hledaný klíč
 * @return 1 v případě nalezení klíče, 0 jinak
 */
int TSSearchInLocalTS(TNode* node, char* key);

/**
 * Hledá klíč v rámci celého zásobníku, ovšem ignorují se klíče s typem FUNC.
 * @param node Tabulka symbolů
 * @param key Hledaný klíč
 * @return 1 v případě nalezení klíče, 0 jinak
 */
int TSSearchStackExceptFunc(TStack_Elem* stackElem, char* key);

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