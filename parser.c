/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "parser.h"

TTree* symTable;
int returnedSth;

Token* getToken() { 
    int retVal = getNextToken();
    if (retVal != SUCCESS) {
        print_err(retVal);
        exit(retVal);
    }
    return token;
}

Token* getNonEolToken() {
    while (getToken()->type == EOL_T);
    return token;
}

void addPos(IsUsedList *isUsedList, bool isUsed, char* varName) {
    IsUsedList* current = isUsedList;
    while (current->next != NULL){
        current = current->next;
    }

    current->next = malloc(sizeof(struct isUsedList));
    current->next->pos = current->pos + 1;
    current->next->isUsed = isUsed;
    current->next->varName = varName;
    current->next->next = NULL;
}

void isUsedListDispose(IsUsedList* isUsedList) {
    if (isUsedList != NULL) {
        isUsedListDispose(isUsedList->next);
        free(isUsedList);
    }
}

/* ------------------------------------------------------------------------------- */

int parse(FILE* file) {
    //symTable = (TTree*)malloc(sizeof(struct tTree)); //TTree je vlastně teď redundantní, když z něj aktuálně nepoužíváme last
    //TSInit(symTable);

    TStackInit(&stack);
    PushFrame(&stack); // Vloží se nový prvek do zásobníku a nastaví se mu hodnoty
    //stack.top->node = symTable->root;
    //stack.top->next = NULL;
    insert_built_in_funcs();

    token = malloc(sizeof(Token));
    strInit(&(token->string));
    currentFuncNode = malloc(sizeof(TNode));

    getSourceCode(file);

    int retVal = program();
    if (retVal == SUCCESS) checkFunctionDefinition();
    PopFrame(&stack);

    return retVal;
}

void insert_built_in_funcs() { //TODO: Ještě stále nejisté, jak to přesně v tabulkce symbolů bude
    TStack_Elem* stackBottom = malloc(sizeof(struct tStack_elem));
    stack.bottom = stackBottom;
    stack.bottom->node = TSInsert(&(stack.top->node), "print", FUNC, true, -1, NULL); //TODO: -1 by mohlo značit, že má n parametrů ??
    stack.bottom->node->retTypes = NULL;
    
    addMultipleRetType(&TSInsert(&(stack.top->node), "inputs", FUNC, true, 0, NULL)->retTypes, 2, STRING, INT);
    addMultipleRetType(&TSInsert(&(stack.top->node), "inputi", FUNC, true, 0, NULL)->retTypes, 2, INT, INT);
    addMultipleRetType(&TSInsert(&(stack.top->node), "inputf", FUNC, true, 0, NULL)->retTypes, 2, FLOAT, INT);
    addMultipleRetType(&TSInsert(&(stack.top->node), "int2float", FUNC, true, 1, NULL)->retTypes, 1, FLOAT);
    addMultipleRetType(&TSInsert(&(stack.top->node), "float2int", FUNC, true, 1, NULL)->retTypes, 1, INT);
    addMultipleRetType(&TSInsert(&(stack.top->node), "len", FUNC, true, 1, NULL)->retTypes, 1, INT);
    addMultipleRetType(&TSInsert(&(stack.top->node), "substr", FUNC, true, 3, NULL)->retTypes, 2, STRING, INT);
    addMultipleRetType(&TSInsert(&(stack.top->node), "ord", FUNC, true, 2, NULL)->retTypes, 2, INT, INT);
    addMultipleRetType(&TSInsert(&(stack.top->node), "chr", FUNC, true, 1, NULL)->retTypes, 2, STRING, INT);    
}

void checkFunctionDefinition() { //TODO: Možná ještě bude potřeba otestovat, až se budou do tabulky symbolů na nejvyšší úroveň přidávat volané funkce
    if (TSAllMeetsConditions(stack.top->node, FUNC, true) == 0) { //Kontrola, zda jsou všechny volané funkce definované
        print_err(ERR_SEM_DEF);
        exit(ERR_SEM_DEF);
    }
    TSExitIfNotDefined(stack.top, "main", true);
}

nodeType nodeTypeFromTokenType(tokenType type) {
    switch (type) {
        case DATA_TYPE_INT: case INT_T:
            return INT;
        case DATA_TYPE_FLOAT: case FLOAT_T:
            return FLOAT;
        case DATA_TYPE_STRING: case STRING_T:
            return STRING;
        default:
            print_err(ERR_COMPILER);
            exit(ERR_COMPILER);
    }
}

/* ------------------------------------------------------------------------------- */

int program() { //DONE ^^
    int retVal = ERR_SYNTAX;

    if (strCmpConstStr(&(getNonEolToken()->string), "package") == 0
        && strCmpConstStr(&(getToken()->string), "main") == 0
        && getToken()->type == EOL_T) {
            generateHeader(); //-- Generování hlavičky
            getNonEolToken();
            retVal = def_func_opt();
            if (retVal != SUCCESS) return retVal;

            if (token->type == EOF_T) {
                retVal = SUCCESS;
            }
            else return ERR_SYNTAX;
    }

    //printf("PROGRAM: %i\n", retVal);
    return retVal;
}

int def_func_opt() { //DONE ^^
    int retVal = SUCCESS;

    if (strCmpConstStr(&(token->string), "func") == 0) {
        retVal = def_func();
        if (retVal != SUCCESS) return retVal;

        if (token->type == EOL_T) {
            getNonEolToken();
            retVal = def_func_opt();
        }
    }

    //printf("DEF_FUNC_OPT: %i\n", retVal);
    return retVal;
}

int def_func() { 
    int retVal = ERR_SYNTAX;
    int isMain = 0;

    if (strCmpConstStr(&(token->string), "func") == 0 && getToken()->type == ID) {
        string funcName;
        strInit(&funcName);
        strCopyString(&funcName, &(token->string));
        generateFuncStart(funcName.str); //-- Generování začátku funkce

        if (strCmpConstStr(&funcName, "main") == 0) isMain = 1;

        if (getToken()->type == L_BRACKET) { 
            getToken();

            int paramCount = 0; //Počítadlo formálních parametrů funkce
            TNode* localTS = NULL;

            retVal = formal_params(&paramCount, &localTS, isMain);
            if (retVal != SUCCESS) return retVal;

            if (token->type == R_BRACKET) {
                TSInsertOrExitOnDuplicity(&(stack.top->node), funcName.str, FUNC, true, paramCount, localTS); //vložení informací o funkci do tabulky symbolů
                currentFuncNode = TSSearch(stack.top->node, funcName.str);
                strFree(&funcName);
                getToken();
                retVal = func_ret_types(isMain);
                if (retVal != SUCCESS) return retVal;

                if (token->type == LC_BRACKET && getToken()->type == EOL_T) {
                    PushFrame(&stack); //Začátek těla funkce
                    getNonEolToken();
                    returnedSth = 0;
                    retVal = body(&returnedSth);

                    if (retVal != SUCCESS) return retVal;

                    if (token->type == RC_BRACKET) {
                        generateFuncEnd(currentFuncNode->key);
                        PopFrame(&stack); //Konec těla funkce
                        if (countRetTypes(currentFuncNode) != 0 && returnedSth != 1) return ERR_SEM_FUNC;
                        getToken();
                        retVal = SUCCESS;
                    }
                    else retVal = ERR_SYNTAX;
                }
                else retVal = ERR_SYNTAX;
            }
            else retVal = ERR_SYNTAX;
        }
        else retVal = ERR_SYNTAX;
    }

    //printf("DEF_FUNC: %i\n", retVal);
    return retVal;
}

int formal_params(int *paramCount, TNode** localTS, int isMain) { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == ID) {
        if (isMain == 1) return ERR_SEM_FUNC; //Main nesmí mít parametry
        string paramName;
        strInit(&paramName);
        strCopyString(&paramName, &(token->string));
        getToken();
        if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING) {
            TSInsertOrExitOnDuplicity(localTS, paramName.str, nodeTypeFromTokenType(token->type), true, *paramCount, NULL);
            (*paramCount)++;

            getToken();
            generateVarFromParam(*paramCount); //-- Vygenerování proměnné, do které bude načtena předaná hodnota
            retVal = formal_params_opt(paramCount, localTS);
        }
        else retVal = ERR_SYNTAX;
        strFree(&paramName);
    }

    //printf("FORMAL_PARAMS: %i\n", retVal);
    return retVal;
}

int formal_params_opt(int *paramCount, TNode** localTS) { //DONE ^^
    int retVal = SUCCESS;
    if (token->type == COMMA) {
        if (getNonEolToken()->type == ID) { //za čárkou možný EOL
            string paramName;
            strInit(&paramName);
            strCopyString(&paramName, &(token->string));
            getToken();

            if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING) {
                TSInsertOrExitOnDuplicity(localTS, paramName.str, nodeTypeFromTokenType(token->type), true, *paramCount, NULL);
                (*paramCount)++;

                getToken(); //Načte další token
                generateVarFromParam(*paramCount); //-- Vygenerování proměnné, do které bude načtena předaná hodnota
                retVal = formal_params_opt(paramCount, localTS);
            }
            else retVal = ERR_SYNTAX;

            strFree(&paramName);
        }
        else retVal = ERR_SYNTAX;
    }

    //printf("FORMAL_PARAMS_OPT: %i\n", retVal);
    return retVal;
}

int func_ret_types(int isMain) { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == L_BRACKET) {
        getToken();

        retVal = types(isMain);
        if (retVal != SUCCESS && token->type != R_BRACKET) return ERR_SYNTAX;
        getToken();
    }

    //printf("FUNC_RET_TYPES: %i\n", retVal);
    return retVal;
}

int types(int isMain) { //DONE ^^
    int retVal = SUCCESS;
    int cnt = 0; //Počítadlo parametrů;

    if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING) {
        if (isMain == 1) return ERR_SEM_FUNC; //Funkce main nesmí mít žádné návratové typy
        addRetType(&currentFuncNode->retTypes, nodeTypeFromTokenType(token->type));
        generateRetValDef(++cnt, token->type); //-- Generování návratového typu
        //printf("--- current func: %s\n", currentFuncNode->key);
        getToken();

        retVal = types_opt(&cnt);
    }

    //printf("TYPES: %i\n", retVal);
    return retVal;
}

int types_opt(int* cnt) { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) { 
        getToken();
        if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING) {
            addRetType(&currentFuncNode->retTypes, nodeTypeFromTokenType(token->type));
            generateRetValDef(++(*cnt), token->type); //-- Generování návratového typu
            getToken();
            retVal = types_opt(cnt);
        }
        else retVal = ERR_SYNTAX;
    }

    //printf("TYPES_OPT: %i\n", retVal);
    return retVal;
}

int body() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == KEYWORD && strCmpConstStr(&(token->string), "return") == 0) { //<body> → <return> <body>
        returnedSth = 1;
        getToken();
        retVal = return_f();
        if (retVal != SUCCESS) return retVal;
        retVal = body();
    }
    else if (token->type == KEYWORD && strCmpConstStr(&(token->string), "if") == 0) { //<body> → <if> EOL <body>
        getToken();
        retVal = if_f();
        if (retVal != SUCCESS) return retVal;
        if (token->type != EOL_T) return ERR_SYNTAX;
        getNonEolToken();
        retVal = body();
    }
    else if (token->type == KEYWORD && strCmpConstStr(&(token->string), "for") == 0) { //<body> → <for> EOL <body>
        getToken();
        retVal = for_f();
        if (retVal != SUCCESS) return retVal;
        if (token->type != EOL_T) return ERR_SYNTAX;
        getNonEolToken();
        retVal = body();
    }
    else if (token->type == ID || token->type == UNDERSCORE) {
        int lParamCnt = 1; //Byl načten první prvek levé strany přiřazení
        IsUsedList* isUsedList = malloc(sizeof(struct isUsedList));
        isUsedList->next = NULL;
        isUsedList->pos = 1;

        if (token->type == ID) { //<body> → id <after_id> EOL <body>
            string idName;
            strInit(&idName);
            strCopyString(&idName, &(token->string));

            isUsedList->isUsed = true;
            isUsedList->varName = idName.str;

            getToken();
            retVal = after_id(idName.str, &lParamCnt, isUsedList);
            strFree(&idName);
            if (retVal != SUCCESS) return retVal;
            if (token->type != EOL_T) return ERR_SYNTAX;
            getNonEolToken();
            retVal = body();
        }
        else { //<body> → _ <ids_lo> = <assign_r> EOL <body>
            isUsedList->isUsed = false;
            isUsedList->varName = "";

            getToken();
            retVal = ids_l_opt(&lParamCnt, isUsedList, "");
            if (retVal != SUCCESS) return retVal;
            if (token->type != EQUALS) return ERR_SYNTAX;
            getToken();
            retVal = assign_r(&lParamCnt, isUsedList);
            if (retVal != SUCCESS) return retVal;
            if (token->type != EOL_T) return ERR_SYNTAX;
            getNonEolToken();
            retVal = body();
        }
    }

    //printf("BODY: %i\n", retVal);
    return retVal;
}

int return_f() { //DONE ^^
    int rParamCnt = 0; //Počítadlo návratových hodnot
    if (return_val(&rParamCnt) == SUCCESS) {
        if (token->type == EOL_T) {
            getNonEolToken();
            //printf("RETURN_F: %i\n", 0);
            if (rParamCnt != countRetTypes(currentFuncNode)) return ERR_SEM_FUNC;
            return SUCCESS;
        }
        else return ERR_SYNTAX; 
    }
    else {
        //printf("RETURN_F: %i\n", 2);
        return ERR_SYNTAX;
    }
}

int return_val(int* retParamCnt) { 
    int data_type, paramCnt; 
    int retVal = psa(&data_type, &paramCnt, retParamCnt, false); //Psa může zvýšit paramCnt
    if (retVal != SUCCESS) return SUCCESS;
    *retParamCnt = *retParamCnt + 1;
    generateRetVal(*retParamCnt, psa_var_cnt);
    retVal = ids_exprs_opt(retParamCnt, true, NULL);
    if (retVal == -1) retVal = SUCCESS;
    return retVal;
}

int if_f() {
    //Minulý token byl if
    int data_type, paramCnt, rParamCnt; //Zde nevyužito
    int retVal = psa(&data_type, &paramCnt, &rParamCnt, true);
    if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;

    if (token->type == LC_BRACKET && getToken()->type == EOL_T) {
        PushFrame(&stack); //Začátek těla if
        getNonEolToken();
        retVal = body();
        if (retVal != SUCCESS) return retVal;

        if (token->type != RC_BRACKET) return ERR_SYNTAX;
        getToken();
        PopFrame(&stack); //Konec těla if

        retVal = else_f();
    }
    else retVal = ERR_SYNTAX;

    return retVal;
}

int for_f() {
    int retVal = def();
    if (retVal != SUCCESS) return retVal;

    if (token->type == SEMICOLON) {
        getToken();
        int data_type, paramCnt, rParamCnt;
        retVal = psa(&data_type, &paramCnt, &rParamCnt, true);
        if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;
        if (token->type == SEMICOLON) {
            getToken();
            retVal = assign();
            if (retVal != SUCCESS) return retVal;
            if (token->type == LC_BRACKET && getToken()->type == EOL_T) {
                PushFrame(&stack); //Začátek těla for
                getNonEolToken();
                retVal = body();
                if (retVal != SUCCESS) return SUCCESS;
                if (token->type != RC_BRACKET) return ERR_SYNTAX;
                PopFrame(&stack); //Konec těla for
                PopFrame(&stack); //Konec definiční části for cyklu
                getToken();
            }
            else return ERR_SYNTAX;
        }
        else return ERR_SYNTAX;
    }
    else return ERR_SYNTAX;
    return retVal;
}

int def() {
    int retVal = SUCCESS;
    PushFrame(&stack); //Začátek definiční části for cyklu

    if (token->type == ID) {
        string idName;
        strInit(&idName);
        strCopyString(&idName, &(token->string));

        getToken();
        retVal = def_var(idName.str);
        strFree(&idName);
    }

    return retVal;
}

int def_var(char* idName) {
    int retVal = ERR_SYNTAX;

    if (token->type == DEF) {
        getToken();
        int data_type, paramCnt, rParamCnt;
        retVal = psa(&data_type, &paramCnt, &rParamCnt, false);
        if (retVal == -1) retVal = ERR_SYNTAX;
        TSInsertOrExitOnDuplicity(&(stack.top->node), idName, data_type, true, 0, NULL);

        generateVariable(idName, stack.top->scope, psa_var_cnt); //-- Vygenerování definice proměnné
    }

    return retVal;
}

int after_id(char* idName, int* lParamCnt, IsUsedList *isUsedList) { //DONE ^^
    int retVal = ERR_SYNTAX;
    int paramCnt;

    if (token->type == L_BRACKET) { //<after_id> → <func>
        int retParamCnt = 0;
        *lParamCnt = 0; //Přečtené ID nebude přiřazováno, jelikož se jedná o název funkce
        retVal = func(&retParamCnt, &paramCnt, idName);

        if (*lParamCnt != retParamCnt) return ERR_SEM_FUNC; //Kontrola počtu parametrů na levé a pravé straně
    }
    else if (token->type == DEF) { //<after_id> → <def_var>
        retVal = def_var(idName);
    }
    else { //<after_id> → <ids_lo>
        TSExitIfNotDefined(stack.top, idName, false); //Kontrola, jestli nepřiřazujeme do nedefinované proměnné
        if (token->type == ID) addPos(isUsedList, true, idName);
        else if (token->type == UNDERSCORE) addPos(isUsedList, false, "");
        retVal = ids_l_opt(lParamCnt, isUsedList, idName);
        if (retVal != SUCCESS && token->type != EQUALS) return ERR_SYNTAX;
        getToken();
        retVal = assign_r(lParamCnt, isUsedList);
    }

    //printf("AFTER_ID: %i\n", retVal);
    return retVal;
}

int ids_l(IsUsedList *isUsedList, char* idName) {
    if (token->type == ID) {
        addPos(isUsedList, true, idName);
        getToken();
        TSExitIfNotDefined(stack.top, token->string.str, false); //Kontrola, jestli nepřiřazujeme do nedefinované proměnné
        return SUCCESS;
    }
    else if (token->type == UNDERSCORE) {
        addPos(isUsedList, false, "");
        getToken();
        return SUCCESS;
    }
    else return ERR_SYNTAX;
}

int ids_l_opt(int* lParamCnt, IsUsedList *isUsedList, char* idName) {
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        getToken();
        retVal = ids_l(isUsedList, idName);
        *lParamCnt = *lParamCnt + 1;
        if (retVal != SUCCESS) return retVal;
        retVal = ids_l_opt(lParamCnt, isUsedList, idName);
    }

    //printf("IDS_L_OPT: %i\n", retVal);
    return retVal;
}

int func(int* retParamCnt, int* paramCnt, char* funcName) { //DONE ^^
    int retVal = ERR_SYNTAX;
    *paramCnt = 0; //Bude načítán první parametr funkce
    TNode* localTS = NULL;

    if (token->type == L_BRACKET) {
        getNonEolToken(); //za závorkou možný EOL
        generateBeforeParamPass();
        retVal = params(paramCnt, &localTS, funcName);
        if (retVal == ERR_SYNTAX || token->type != R_BRACKET) return ERR_SYNTAX;

        //printf("---- Volána funkce: %s, paramCnt: %i\n", funcName, *paramCnt);
        TSInsertFuncOrCheck(stack.bottom, funcName, *paramCnt, localTS, retParamCnt); //bude-li už definovaná, kontrola typů/pořadí/počtu parametrů - (pokud bude sedět, přepis klíčů na reálně použité?; 
        generateFuncCall(funcName); //-- Generování volání funkce
        getNextToken();
    }

    //printf("FUNC: %i\n", retVal);
    return retVal;
}

int params(int *paramCnt, TNode** localTS, char* funcName) {
    int retVal = SUCCESS;

    if (token->type == ID || token->type == FLOAT_T || token->type == INT_T || token->type == STRING_T) {
        if (token->type == ID) {
            string idName;
            strInit(&idName);
            strCopyString(&idName, &(token->string));

            TSExitIfNotDefined(stack.top, idName.str, false); //Nelze použít nedefinovaný identifikátor jako parametr
            TNode* paramNode = TSSearchStackExceptFunc(stack.top, idName.str);
            TSInsert(localTS, idName.str, paramNode->type, funcName, *paramCnt, NULL); //TODO: Před vložením musí proběhnout kontrola typů
        }
        else {
            TNode *paramNode = TSSearchByNameAndParam(stack.bottom->node, funcName, *paramCnt);
            if (paramNode != NULL) {
                TSInsert(localTS, paramNode->key, nodeTypeFromTokenType(token->type), true, *paramCnt, NULL);
            }
            else {
                //TODO: Musí proběhnout pozdější kontrola, protože dunkce ještě nebyla definována,
                //Informace o tom, jaký je parametr funkce by se někam měla uložit tak i tak
            }
        }
        *paramCnt = *paramCnt + 1; //Načten další parametr funkce
        generateParamPass(*paramCnt, token); //-- Generování proměnné v dočasném rámci
        getToken();
        retVal = params_opt(paramCnt, localTS, funcName);
    }

    return retVal;
}

int params_opt(int* paramCnt, TNode** localTS, char* funcName) {
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        getToken();
        if (token->type == ID || token->type == FLOAT_T || token->type == INT_T || token->type == STRING_T) {
            if (token->type == ID) {
                TSExitIfNotDefined(stack.top, token->string.str, false); //Nelze použít nedefinovaný identifikátor jako parametr
                //TODO: Nutné vyhledat datový typ ID
            }
            else {
                TNode *paramNode = TSSearchByNameAndParam(stack.bottom->node, funcName, *paramCnt); //TODO: Pokud je NULL, funkce ještě nebyla definována
                if (paramNode != NULL) {
                    TSInsert(localTS, paramNode->key, nodeTypeFromTokenType(token->type), true, *paramCnt, NULL);
                }
            }
            *paramCnt = *paramCnt + 1;
            generateParamPass(*paramCnt, token); //-- Generování proměnné v dočasném rámci
            getToken();
            retVal = params_opt(paramCnt, localTS, funcName);
        }
    }

    //printf("IDS_OPT: %i\n", retVal);
    return retVal;
}

int assign_r(int* lParamCnt, IsUsedList* isUsedList) { 
    int data_type;
    int funcParamCnt = 0; //Počítadlo pro případné parametry funkce
    int rParamCnt = 1; //Začínají se počítat parametry pravé strany přiřazení, u volání funkce zde bude počet návratových hodnot
    int retVal = psa(&data_type, &funcParamCnt, &rParamCnt, true);
    if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;

    if (isUsedList != NULL && isUsedList->isUsed == true) { //Aktuální hodnota má být přiřazena
       int scope = TSSearchStackAndReturnScope(stack.top, isUsedList->varName);
       if (scope != -1) generateValAssignment(isUsedList->varName, scope, psa_var_cnt); //TODO: Nebude fungovat pro funkce
    }

    if (funcParamCnt != 0 && token->type != EOL_T) return ERR_SEM_OTHER; //Pokud se přiřazuje výstup funkce, nesmí již následovat další přiřazované hodnoty

    retVal = ids_exprs_opt(&rParamCnt, false, isUsedList); //Počítání dalších možných ID na pravé straně přiřazení
    //printf("--- Přiřazení lParam %i : rParam %i\n", *lParamCnt, rParamCnt);
    if (retVal != SUCCESS) return retVal;
    if (*lParamCnt != rParamCnt) {
        if (funcParamCnt != 0) return ERR_SEM_FUNC;
        else return ERR_SEM_OTHER;
    }
    isUsedListDispose(isUsedList);
    return retVal;
}

int ids_exprs_opt(int* rParamCnt, bool isReturn, IsUsedList* isUsedList) {
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        getToken();
        int data_type;
        int paramCnt = 0; //Počítadlo pro případné parametry funkce
        retVal = psa(&data_type, &paramCnt, rParamCnt, false);

        if (paramCnt != 0) return ERR_SYNTAX; //Přiřazení výstupu funkce může být pouze v případě, že je na pravé straně volání funkce jako jediné (zde by bylo 2. v pořadí)

        if (isUsedList != NULL && isUsedList->isUsed == true) { //Aktuální hodnota má být přiřazena
            int scope = TSSearchStackAndReturnScope(stack.top, isUsedList->varName);
            if (scope != -1) generateValAssignment(isUsedList->varName, scope, psa_var_cnt); //TODO: Nebude fungovat pro funkce
        }
        if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;
        *rParamCnt = *rParamCnt + 1; 
        if (isReturn) generateRetVal(*rParamCnt, psa_var_cnt);
        retVal = ids_exprs_opt(rParamCnt, isReturn, isUsedList);
    }

    //printf("IDS_EXPR_OPT: %i\n", retVal);
    return retVal;
}

int assign() { //TODO: třeba otestovat kontrolu přiřazování ve for cyklu, zatím netestováno
    int retVal = SUCCESS;

    if (token->type == ID || token->type == UNDERSCORE) {
        IsUsedList* isUsedList = malloc(sizeof(struct isUsedList));
        isUsedList->next = NULL;
        isUsedList->pos = 1;

        string idName;
        strInit(&idName);
        strCopyString(&idName, &(token->string));

        if (token->type == ID) {
            TSExitIfNotDefined(stack.top, token->string.str, false); //Kontrola, jestli nepřiřazujeme do nedefinované proměnné
            isUsedList->isUsed = true;
            isUsedList->varName = idName.str;
        }
        else {
            isUsedList->isUsed = false;
            isUsedList->varName = "";
        }
        getToken();
        int lParamCnt = 1;
        retVal = ids_l_opt(&lParamCnt, isUsedList, idName.str);
        if (retVal != SUCCESS) return retVal;
        if (token->type == EQUALS) {
            getToken(); 
            retVal = assign_r(&lParamCnt, isUsedList);
        }
        strFree(&idName);
    }
    return retVal;
}

int else_f() { //DONE ^^
    int retVal = SUCCESS;
    if (strCmpConstStr(&(token->string), "else") == 0) {
        if (getToken()->type == LC_BRACKET && getToken()->type == EOL_T) {
            PushFrame(&stack); //Začátek těla else
            getNonEolToken();
            retVal = body();
            if (retVal != SUCCESS) return retVal;
            if (token->type != RC_BRACKET) return ERR_SYNTAX;
            PopFrame(&stack); //Konec těla else 
            getToken();
        }
        else retVal = ERR_SYNTAX;
    }
    //printf("ELSE_F: %i\n", retVal);
    return retVal;
}