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

/*
 * Typ identifikátoru
 */
typedef enum {
    /*0*/ INT,
    /*1*/ FLOAT,
    /*2*/ STRING,
    /*3*/ NIL,
    /*4*/ FUNC
} nodeType;

/**
 * Uzel binárího stromu
 */
typedef struct tNode {
    char* key;			    // Klíč
    struct tNode* lptr;	    // Levá větev
    struct tNode* rptr;	    // Pravá větev

    nodeType type;          // Typ identifikátoru
    bool isDefined;         // Byl identifikátor definovaný 
    int param;              // Počet parametrů (jde-li o funkci), Pozice parametru (jde-li o parametr funkce)
    struct tNode* localTS;  // Ukazatel na tabulku symbolů lokální úrovně (jde-li o funkci) //TODO: Pravděpodobně bude třeba localTS = NULL
} TNode;

typedef struct tTree {
    struct tNode* root;
    struct tNode* last;
} TTree;

typedef struct tStack_elem {
    struct tNode* node;
    struct tStack_elem* next;
} TStack_Elem;

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
 * Zruší celý strom
 */
void TSDispose(TNode* root);

/**
 * Vloží prvek do tabulky symbolů pouze pokud se v tabulce symbolů doposud nenachází
 * @param root Ukazatel na kořen stromu
 * @param key Klíč nového uzlu
 * @param type Typ nového uzlu
 * @param isDefined Byl identifikátor nového uzlu definovaný?
 * @param param Počet parametrů (jde-li o funkci), Pozice parametru (jde-li o parametr funkce)
 * @param localTS Ukazatel na tabulku symbolů lokální úrovně (jde-li o funkci)
 */
void TSInsertAndExitOnDuplicity(TNode** root, char* key, nodeType type, bool isDefined, int param, TNode* localTS);

void TStackInit(TStack* stack);

void PushFrame(TStack* stack);

void PopFrame(TStack* stack);

void TSPrint(TNode* root);