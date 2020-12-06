/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "isusedlist.h"

void addPos(IsUsedList *isUsedList, bool isUsed, nodeType type, char *varName) {
    IsUsedList *current = isUsedList;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(struct isUsedList));
    current->next->pos = current->pos + 1;
    current->next->isUsed = isUsed;
    current->next->type = type;
    current->next->varName = malloc(strlen(varName) + 1);
    strcpy(current->next->varName, varName);
    current->next->next = NULL;
}

void isUsedListSetVarName(IsUsedList *isUsedList, char *varName) {
    isUsedList->varName = malloc(strlen(varName) + 1);
    strcpy(isUsedList->varName, varName);
}

void isUsedListDispose(IsUsedList *isUsedList) {
    if (isUsedList != NULL) {
        if (isUsedList->varName != NULL)
            free(isUsedList->varName);
        isUsedListDispose(isUsedList->next);
        free(isUsedList);
    }
}
