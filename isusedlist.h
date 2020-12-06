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

typedef struct isUsedList {
    int pos;
    bool isUsed;
    nodeType type;
    char *varName;
    struct isUsedList *next;
} IsUsedList;

void addPos(IsUsedList *isUsedList, bool isUsed, nodeType type, char *varName);
void isUsedListSetVarName(IsUsedList *isUsedList, char *varName);
void isUsedListDispose(IsUsedList *isUsedList);
