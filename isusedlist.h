/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

#include "nodetype.h"

/*
 * Seznam parametrů levé strany přiřazení
 */
typedef struct isUsedList {
    int pos;
    bool isUsed;
    nodeType type;
    char *varName;
    struct isUsedList *next;
} IsUsedList;

/*
 * Přidává prvek do seznamu parametrů levé strany přiřazení
 * @param isUsedList Seznam
 * @param isUsed Bude/nebude hodnota na odpovídající pozici v pravé části přiřazení využita
 * @param type
 * @param varName Jméno proměnné
 */
void addPos(IsUsedList *isUsedList, bool isUsed, nodeType type, char *varName);

/*
 * Nastaví jméno proměnné prvku seznamu
 * @param isUsedList Prvek seznamu
 * @param varName Jméno proměnné
 */
void isUsedListSetVarName(IsUsedList *isUsedList, char *varName);

/*
 * Smaže seznam parametrů levé strany přiřazení
 * @param isUsedList Seznam určený ke smazání
 */
void isUsedListDispose(IsUsedList *isUsedList);
