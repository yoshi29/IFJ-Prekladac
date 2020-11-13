/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
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
    INT,
    FLOAT,
    STRING,
    NIL,
    FUNC
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