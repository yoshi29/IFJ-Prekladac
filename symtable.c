/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 *        Filip Vágner, xvagne08
 */

#include "symtable.h"

TStack stack;
TNode* currentFuncNode = NULL;
int scopeCnt = 0;

void TSInit(TTree* tree) {
    tree->root = NULL;
    tree->last = NULL;
}

TNode* TSInsert(TNode** root, char* key, nodeType type, bool isDefined, int param, TNode* localTS) {
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

        tmp->retTypes = NULL;
        tmp->lptr = NULL;
        tmp->rptr = NULL;

        *root = tmp;
        return tmp;
    }
    else {
        int cmp = strcmp(key, (*root)->key);

        if (cmp < 0) { // Vkládání do levé větve
            return TSInsert(&(*root)->lptr, key, type, isDefined, param, localTS);
        }
        else if (cmp > 0) { // Vkládání do pravé větve
            return TSInsert(&(*root)->rptr, key, type, isDefined, param, localTS);
        }
        else { // Aktualizují se data
            (*root)->type = type;
            (*root)->isDefined = isDefined;
            (*root)->localTS = localTS;
            (*root)->param = param;
            return (*root);
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

TNode* TSSearchByNameAndParam(TNode* root, char* funcName, int paramPos) {
    TNode* node = TSSearch(root, funcName);
    if (node == NULL) return NULL;
    return TSSearchByParam(node->localTS, paramPos);
}

TNode* TSSearchByParam(TNode* root, int paramPos) {
    if (root == NULL) return NULL;

    if (root->param != paramPos) {
        TNode* tmp = TSSearchByParam(root->lptr, paramPos);
        if (tmp == NULL) return TSSearchByParam(root->rptr, paramPos);
        else return tmp;
    }
    else return root;
}

TNode* TSSearchStackAndReturn(TStack_Elem* stackElem, char* key, int* scope) {
    TNode* node;
    if (stackElem != NULL) {
        if ((node = TSSearch(stackElem->node, key)) == NULL)
            node = TSSearchStackAndReturn(stackElem->next, key, scope);
        else
            *scope = stackElem->scope; // Prvek byl nazelen, uložíme číslo rámce

        return node;
    }
    else if (currentFuncNode != NULL) {
        if ((node = TSSearch(currentFuncNode->localTS, key)) != NULL)
            *scope = 0;
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

        RetType* retType = root->retTypes;
        RetType* next;
        while (retType != NULL) {
            next = retType->next;
            free(retType);
            retType = next;
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
    else return 0; 
}

TNode* TSSearchStackExceptFunc(TStack_Elem* stackElem, char* key) {
    TNode* node;

    if (stackElem != NULL) {
        if ((node = TSSearch(stackElem->node, key)) != NULL && node->isDefined == true) {
            if (node->type != FUNC) return node; //Klíč byl nalezen a zároveň se nejedná o funkci
            else return TSSearchStackExceptFunc(stackElem->next, key);
        }
        else if ((node = TSSearch(currentFuncNode->localTS, key)) != NULL) return node; //Klíč nebyl nalezen, prohledáme ještě lokální tabulky
        else return TSSearchStackExceptFunc(stackElem->next, key);
    }
    else return NULL;
}

int TSSearchStackAndReturnScope(TStack_Elem* stackElem, char* key) {
    TStack_Elem* elem = stackElem;
    TNode* node;
    int scope = -1;

    while (elem != NULL) {
        if ((node = TSSearch(elem->node, key)) != NULL && node->isDefined == true && node->type != FUNC) {
            scope = elem->scope;
            break;
        }
        elem = elem->next;
    }
    if (scope == -1) { //Nebylo nalezeno, zkusíme hledat mezi formálními parametry aktuální funkce
        if (TSSearch(currentFuncNode->localTS, key) != NULL) scope = 0;
    }
    return scope;
}

/*
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
*/

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
        if (TSSearchStackExceptFunc(stackElem, key) == NULL) isDefined = 0;
        else isDefined = 1;
    }
    if (isDefined == 0) {
        print_err(ERR_SEM_DEF);
        exit(ERR_SEM_DEF);
    }
}

void TSInsertFuncOrCheck(TStack_Elem* stackElem, char* key, int param, TNode* localTS, int* rParamCnt, IsUsedList* isUsedList, RetType* retType, bool def) {
    TNode* funcNode = TSSearch(stackElem->node, key);

    if (funcNode == NULL) { //Funkce zatím nebyla definována
        TNode *funcNode = TSInsert(&(stackElem->node), key, FUNC, def, param, localTS);
        //TODO: Ještě uložit, že zatím neznáme počet návratových parametrů, anebo pak nějak přiřadit lParamCnt a porovnávat při definici funkce?

        // Pokud se jedná o definici funkce
        if (def && retType != NULL) { 
            funcNode->retTypes = retType; // Přiřazení návratových typů
        }

        // Pokud se nejedná o definici funkce
        while (!def && isUsedList != NULL) { 
            addRetType(&(funcNode->retTypes), isUsedList->type); // Přidáme do TS očekávané návratové hodnoty
            isUsedList = isUsedList->next;
        }
        *rParamCnt = countRetTypes(funcNode);
    }
    else { //Funkce již byla definována, provede se kontrola počtů a typů parametrů
        if (funcNode->param == -1) *rParamCnt = 0; //Je volána funkce, která nemá pevně daný počet parametrů
        else if (funcNode->param != param || TSCompare(funcNode->localTS, localTS) != 0) {
            fprintf(stderr, "---------------------- RETVAL: \n");

            //printf("--- ERR_SEMFUNC: funcNode->para: %i, param: %i\n", funcNode->param, param);
            print_err(ERR_SEM_FUNC);
            exit(ERR_SEM_FUNC);
        }
        else {
            if (!def) { // Pokud se nejedná o definici funkce
                RetType *current = funcNode->retTypes;
                while (isUsedList != NULL && current != NULL) {
                    if (isUsedList->type != current->type) {
                        print_err(ERR_SEM_FUNC);
                        exit(ERR_SEM_FUNC);
                    }
                    isUsedList = isUsedList->next;
                    current = current->next;
                }
                if (isUsedList != NULL || current != NULL) { // Musí sedět počet návratových hodnot
                    print_err(ERR_SEM_FUNC);
                    exit(ERR_SEM_FUNC);
                }
            }
            else { // Pokud se jedná o definici funkce
                RetType* current1 = retType;
                RetType* current2 = funcNode->retTypes;
                while (current1 != NULL && current2 != NULL) {
                    if (current1->type != current2->type) {
                        print_err(ERR_SEM_FUNC);
                        exit(ERR_SEM_FUNC);
                    }
                    current1 = current1->next;
                    current2 = current2->next;
                }
                if (current1 != NULL || current2 != NULL) { // Musí sedět počet návratových hodnot
                    print_err(ERR_SEM_FUNC);
                    exit(ERR_SEM_FUNC);
                }

                funcNode->isDefined = true;
            }
            *rParamCnt = countRetTypes(funcNode); //Vrací počet návratových hodnot dané funkce
            //printf("-------- funcRetParamCnt %i\n", *rParamCnt);
        }
    }
}

int TSCompare(TNode* firstNode, TNode* secondNode) {
    if (firstNode != NULL && secondNode != NULL) {
        //printf("1) %s: par: %i, type: %i | 2) %s: par: %i, type: %i\n", firstNode->key, firstNode->param, firstNode->type, secondNode->key, secondNode->param, secondNode->type);
   
        if (firstNode->param == secondNode->param && firstNode->type == secondNode->type) {
            return TSCompare(firstNode->lptr, secondNode->lptr) || TSCompare(firstNode->rptr, secondNode->rptr);
        }
        else return 1;
    }
    else return 0;
}

void TStackInit(TStack* stack) {
    stack->top = NULL;
    stack->bottom = NULL;
}

void PushFrame(TStack* stack) {
    TStack_Elem* newStackTop = malloc(sizeof(struct tStack_elem));
    if (newStackTop == NULL) {
        print_err(ERR_COMPILER);
        exit(ERR_COMPILER);
    }
    newStackTop->scope = scopeCnt++;
    newStackTop->next = stack->top;
    newStackTop->node = NULL;
    stack->top = newStackTop;
}

void PopFrame(TStack* stack) {
    scopeCnt--;
    TStack_Elem* remove = stack->top;
    stack->top = stack->top->next;
    if (remove->node != NULL)
        TSDispose(remove->node);
    free(remove);
    remove = NULL;
}

void addMultipleRetType(RetType** retType, int num,...) {
    va_list args;
    va_start(args, num);
    for (int i = 0; i < num; i++) {
        nodeType type = va_arg(args, int);
        addRetType(retType, type);
    }
    va_end(args);
}

void addRetType(RetType** retType, nodeType type) { 
    if (*retType == NULL) {
        RetType* tmp = malloc(sizeof(struct retType));

        if (tmp == NULL) {
            print_err(ERR_COMPILER);
            exit(ERR_COMPILER);
        }

        tmp->type = type;
        tmp->next = NULL;

        *retType = tmp;
    }
    else {
        addRetType(&(*retType)->next, type);
    }
}

int countRetTypes(TNode* node) {
    int cnt = 0;
    RetType* tmp = node->retTypes;
    while (tmp != NULL) {
        tmp = tmp->next;
        cnt = cnt + 1;
    }
    return cnt;
}