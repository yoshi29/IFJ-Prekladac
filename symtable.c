/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 *        Filip Vágner, xvagne08
 */

#include "symtable.h"

void TSInit(TTree* tree) {
    tree->root = NULL;
    tree->last = NULL; //TODO: Bude potřeba?
}

void TSInsert(TNode** root, char* key, nodeType type, bool isDefined, int param, TNode* localTS) {
    if (*root == NULL) { // Volné másto pro vložený nového prvku
        TNode *tmp = (TNode*)malloc(sizeof(struct tNode));
        if (tmp == NULL) {
            print_err(ERR_COMPILER);
            exit(ERR_COMPILER);
        }

        tmp->key = key;
        tmp->type = type;
        tmp->isDefined = isDefined;
        tmp->localTS = localTS;
        tmp->param = param;

        tmp->lptr = NULL;
        tmp->rptr = NULL;

        *root = tmp;
    }
    else {
        int cmp = strcmp(key, *(&(*root)->key));

        if (cmp < 0) { // Vkládání do levé větve
            TSInsert(&(*root)->lptr, key, type, isDefined, param, localTS);
        }
        else if (cmp > 0) { // Vkládání do pravé větve
            TSInsert(&(*root)->rptr, key, type, isDefined, param, localTS);
        }
        else {
            //TODO: Co když narazím na prvek, který ve stromu již je?
        }
    }
}

TNode* TSSearch(TNode* root, char* key) {
    if (root == NULL) return NULL;
    int cmp = strcmp(key, root->key);

    if (cmp < 0) {
        return TSSearch(root->lptr, key);
    }
    else if (cmp > 0) {
        return TSSearch(root->rptr, key);
    }
    else { // Prvek nalezen
        return root;
    }
}

void TSPrint(TNode* root) {
    if (root != NULL) {
        printf("KEY: %s\n", root->key);
        TSPrint(root->lptr);
        TSPrint(root->rptr);
    }
}

void TSDispose(TNode* root) {
    if (root != NULL) {
        TSDispose(root->lptr);
        TSDispose(root->rptr);

        if (root->localTS != NULL) {
            TSDispose(root->localTS);
            root->localTS = NULL;
        }

        free(root);
        root = NULL;
    }
}

void TSInsertAndExitOnDuplicity(TNode** root, char* key, nodeType type, bool isDefined, int param, TNode* localTS) {
    if (TSSearch(*root, key) != NULL) { //Klíč již je v tabulce symbolů - redefinice
        print_err(ERR_SEM_DEF);
        exit(ERR_SEM_DEF); //TODO: Hned exit, anebo vracet hodnotu a všude to kontrolovat?
    }
    else TSInsert(root, key, type, isDefined, param, localTS);
}

void TStackInit(TStack* stack) {
    stack->top = NULL;
}

void PushFrame(TStack* stack) {
    TStack_Elem* newStackTop = malloc(sizeof(struct tStack_elem));
    if (newStackTop == NULL) {
        print_err(ERR_COMPILER);
        exit(ERR_COMPILER);
    }

    newStackTop->next = stack->top;
    newStackTop->node = NULL;
    stack->top = newStackTop;
}

void PopFrame(TStack* stack) {
    TStack_Elem* remove = stack->top;
    stack->top = stack->top->next;
    if (remove->node != NULL)
        TSDispose(remove->node);
    free(remove);
    remove = NULL;
}