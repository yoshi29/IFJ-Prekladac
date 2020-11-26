/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 *        Filip Vágner, xvagne08
 */

#include "symtable.h"

TStack stack;

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

        tmp->key = malloc(strlen(key) + 1);
        strcpy(tmp->key, key);
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

TNode* TSSearchStackAndReturn(TStack_Elem* stackElem, char* key) {
    if (stackElem != NULL) {
        TNode *node = TSSearch(stackElem->node, key);
        if (node == NULL)
            node = TSSearchStackAndReturn(stackElem->next, key);
        return node;
    }
    else return NULL;
}

int TSAllMeetsConditions(TNode* root, nodeType type, bool isDefined) {
    if (root != NULL) {
        if (root->type == type) {
            if (root->isDefined == isDefined)
                return (TSAllMeetsConditions(root->rptr, type, isDefined) && TSAllMeetsConditions(root->lptr, type, isDefined)) ;
            else return 0;
        }
        else return 1; //Pokud by nebyla nalezena žádná funkce, pak hodnota 1 je správným výsledkem
    }
    else return 1; //Pokud by nebylo nalezeno nic, pak hodnota 1 je správným výsledkem
}

void TSPrint(TNode* root) {
    if (root != NULL) {
        printf("--- Printing: %s\n", root->key);
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

        free(root->key);
        free(root);
        root = NULL;
    }
}

int TSSearchStack(TStack_Elem* stackElem, char* key) {
    if (stackElem != NULL) {
        TNode *node = TSSearch(stackElem->node, key);
        if (node != NULL && node->isDefined == true) return 1;
        else return TSSearchStack(stackElem->next, key);
    }
    else return 0; //TODO: OK řešení?
}

int TSSearchStackExceptFunc(TStack_Elem* stackElem, char* key) {
    TNode* node;

    if (stackElem != NULL) {
        if ((node = TSSearch(stackElem->node, key)) != NULL && node->isDefined == true) {
            if (node->type != FUNC) return 1; //Klíč byl nalezen a zároveň se nejedná o funkci
            else return TSSearchStackExceptFunc(stackElem->next, key);
        }
        else if (TSSearchInLocalTS(stackElem->node, key) == 1) return 1; //Klíč nebyl nalezen, prohledáme ještě lokální tabulky
        else return TSSearchStackExceptFunc(stackElem->next, key);
    }
    else return 0; //TODO: OK řešení?
}

int TSSearchInLocalTS(TNode* node, char* key) {
    if (node != NULL) {
        if (node->localTS != NULL) {
            if (TSSearch(node->localTS, key) != NULL) return 1;
            else return (TSSearchInLocalTS(node->lptr, key) || TSSearchInLocalTS(node->rptr, key)); //Zkusí najít v lokálních tabulkách levého a pravého syna
        }
        else {
            return (TSSearchInLocalTS(node->lptr, key) || TSSearchInLocalTS(node->rptr, key)); //Zkusí najít v lokálních tabulkách levého a pravého syna
        }
    }
    else return 0;
}

void TSInsertOrExitOnDuplicity(TNode** root, char* key, nodeType type, bool isDefined, int param, TNode* localTS) {
    if (TSSearch(*root, key) != NULL) { //Klíč již je v tabulce symbolů - redefinice
        print_err(ERR_SEM_DEF);
        exit(ERR_SEM_DEF); //TODO: Hned exit, anebo vracet hodnotu a všude to kontrolovat?
    }
    else TSInsert(root, key, type, isDefined, param, localTS);
}

void TSExitIfNotDefined(TStack_Elem* stackElem, char* key, bool canBeFunc) {
    int isDefined;
    if (canBeFunc) {
        isDefined = TSSearchStack(stackElem, key);
    }
    else {
        isDefined = TSSearchStackExceptFunc(stackElem, key);
    }
    if (isDefined == 0) {
        print_err(ERR_SEM_DEF);
        exit(ERR_SEM_DEF);
    }
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