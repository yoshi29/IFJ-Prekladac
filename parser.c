/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "parser.h"

TTree* symTable;
int returnedSth;

int ifCnt = 0;
int forCnt = 0;
int forScope = 0;

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

/* ------------------------------------------------------------------------------- */

int parse(FILE* file) {
    TStackInit(&stack);
    PushFrame(&stack); // Vloží se nový prvek do zásobníku a nastaví se mu hodnoty
    stack.bottom = stack.top;

    token = malloc(sizeof(Token));
    strInit(&(token->string));
    currentFuncNode = malloc(sizeof(TNode));

    getSourceCode(file);

    generateHeader(); //-- Generování hlavičky
    insert_built_in_funcs();

    int retVal = program();
    if (retVal == SUCCESS) checkFunctionDefinition();
    PopFrame(&stack);

    generateExit();

    return retVal;
}

void insert_built_in_funcs() {
    TNode *func = NULL;

    // func print(term1, term2, ... , termN)
    func = TSInsert(&(stack.top->node), "print", FUNC, true, -1, NULL);
    
    // func inputs() (string, int)
    func = TSInsert(&(stack.top->node), "inputs", FUNC, true, 0, NULL);
    addMultipleRetType(&(func->retTypes), 2, STRING, INT);
    
    // func inputi() (int, int)
    func = TSInsert(&(stack.top->node), "inputi", FUNC, true, 0, NULL);
    addMultipleRetType(&(func->retTypes), 2, INT, INT);

    // func inputf() (float64, int)
    func = TSInsert(&(stack.top->node), "inputf", FUNC, true, 0, NULL);
    addMultipleRetType(&(func->retTypes), 2, FLOAT, INT);

    // func int2float(i int) (float64)
    func = TSInsert(&(stack.top->node), "int2float", FUNC, true, 1, NULL);
    TSInsert(&(func->localTS), "i", INT, true, 0, NULL);
    addMultipleRetType(&(func->retTypes), 1, FLOAT);

    // func float2int(f float64) (int)
    func = TSInsert(&(stack.top->node), "float2int", FUNC, true, 1, NULL);
    TSInsert(&(func->localTS), "f", FLOAT, true, 0, NULL);
    addMultipleRetType(&(func->retTypes), 1, INT);

    // func len(s string) (int)
    func = TSInsert(&(stack.top->node), "len", FUNC, true, 1, NULL);
    TSInsert(&(func->localTS), "s", STRING, true, 0, NULL);
    addMultipleRetType(&(func->retTypes), 1, INT);

    // func substr(s string, i int, n int) (string, int)
    func = TSInsert(&(stack.top->node), "substr", FUNC, true, 3, NULL);
    TSInsert(&(func->localTS), "s", STRING, true, 0, NULL);
    TSInsert(&(func->localTS), "i", INT, true, 1, NULL);
    TSInsert(&(func->localTS), "n", INT, true, 2, NULL);
    addMultipleRetType(&(func->retTypes), 2, STRING, INT);

    // func ord(s string, i int) (int, int)
    func = TSInsert(&(stack.top->node), "ord", FUNC, true, 2, NULL);
    TSInsert(&(func->localTS), "s", STRING, true, 0, NULL);
    TSInsert(&(func->localTS), "i", INT, true, 1, NULL);
    addMultipleRetType(&(func->retTypes), 2, INT, INT);

    // func chr(i int) (string, int)
    func = TSInsert(&(stack.top->node), "chr", FUNC, true, 1, NULL);
    TSInsert(&(func->localTS), "i", INT, true, 0, NULL);
    addMultipleRetType(&(func->retTypes), 2, STRING, INT);

    generateBuiltInFunctions();
}

void checkFunctionDefinition() {
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
        
        TNode* func = TSSearch(stack.bottom->node, funcName.str);
        if (func != NULL && func->isDefined == true)
            return ERR_SEM_DEF;
        
        if (strCmpConstStr(&funcName, "main") == 0) isMain = 1;

        if (getToken()->type == L_BRACKET) { 
            getToken();

            int paramCount = 0; //Počítadlo formálních parametrů funkce
            TNode* localTS = NULL;

            retVal = formal_params(&paramCount, &localTS, isMain);
            if (retVal != SUCCESS) return retVal;

            if (token->type == R_BRACKET) {
                getToken();
                RetType *ret = NULL;
                retVal = func_ret_types(isMain, &ret);
                if (retVal != SUCCESS) return retVal;

                int retValCnt;
                TSInsertFuncOrCheck(stack.bottom, funcName.str, paramCount, localTS, &retValCnt, NULL, ret, true);
                currentFuncNode = TSSearch(stack.bottom->node, funcName.str);

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

        strFree(&funcName);
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

int func_ret_types(int isMain, RetType** retType) { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == L_BRACKET) {
        getToken();

        retVal = types(isMain, retType);
        if (retVal != SUCCESS && token->type != R_BRACKET) return ERR_SEM_FUNC;
        getToken();
    }

    //printf("FUNC_RET_TYPES: %i\n", retVal);
    return retVal;
}

int types(int isMain, RetType** retType) { //DONE ^^
    int retVal = SUCCESS;
    int cnt = 0; //Počítadlo parametrů;

    if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING) {
        if (isMain == 1) return ERR_SEM_FUNC; //Funkce main nesmí mít žádné návratové typy
        addRetType(retType, nodeTypeFromTokenType(token->type));
        generateRetValDef(++cnt, token->type); //-- Generování návratového typu
        //printf("--- current func: %s\n", currentFuncNode->key);
        getToken();

        retVal = types_opt(&cnt, retType);
    }

    //printf("TYPES: %i\n", retVal);
    return retVal;
}

int types_opt(int* cnt, RetType** retType) { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) { 
        getToken();
        if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING) {
            addRetType(retType, nodeTypeFromTokenType(token->type));
            generateRetValDef(++(*cnt), token->type); //-- Generování návratového typu
            getToken();
            retVal = types_opt(cnt, retType);
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

            int tmpVar;
            TNode *var = TSSearchStackAndReturn(stack.top, idName.str, &tmpVar);
            if (var != NULL)
                isUsedList->type = var->type;
            else
                isUsedList->type = UNKNOWN;

            isUsedList->isUsed = true;
            isUsedListSetVarName(isUsedList, idName.str);
            

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
            isUsedListSetVarName(isUsedList, "");
            isUsedList->type = UNKNOWN;

            getToken();
            retVal = ids_l_opt(&lParamCnt, isUsedList);
            if (retVal != SUCCESS) return retVal;
            if (token->type != EQUALS) return ERR_SYNTAX;
            getToken();
            retVal = assign_r(&lParamCnt, isUsedList);
            if (retVal != SUCCESS) return retVal;
            if (token->type != EOL_T) return ERR_SYNTAX;
            getNonEolToken();
            retVal = body();
        }
        isUsedListDispose(isUsedList);
    }

    //printf("BODY: %i\n", retVal);
    return retVal;
}

int return_f() { //DONE ^^
    int rParamCnt = 0; //Počítadlo návratových hodnot
    
    RetType *retType = NULL;

    int retVal = return_val(&rParamCnt, &retType);
    if (retVal == SUCCESS || retVal == -1) {
        if (token->type == EOL_T) {

            for (int i = 0; i < forScope; i++)
                generateForFrame(false, i);
                
            if (strcmp(currentFuncNode->key, "main") == 0)
                printInstr("JUMP $$main-exit");
            else
                printCode(5, "JUMP", " ", "$", currentFuncNode->key, "-end");
                
            // Kontrola datových typů u 'return' oproti definici funkce
            RetType* funcRetTypes = currentFuncNode->retTypes;
            while (funcRetTypes != NULL && retType != NULL) {
                if (funcRetTypes->type != retType->type) // Pokud se liší datové typy
                    return ERR_SEM_FUNC;

                funcRetTypes = funcRetTypes->next;
                retType = retType->next;
            }
            if (funcRetTypes != NULL || retType != NULL) // Pokud je špatný počet návratových hodnot
                return ERR_SEM_FUNC;

            getNonEolToken();
            //printf("RETURN_F: %i\n", 0);

            if (rParamCnt != countRetTypes(currentFuncNode)) return ERR_SEM_FUNC;
            return SUCCESS;
        }
        else return ERR_SYNTAX; 
    }
    else {
        //printf("RETURN_F: %i\n", 2);
        return retVal;
    }
}

int return_val(int* retParamCnt, RetType** retType) { 
    int data_type, paramCnt; 
    int retVal = psa(&data_type, &paramCnt, retParamCnt, false, NULL); //Psa může zvýšit paramCnt
    if (retVal != SUCCESS) return retVal;
    if (data_type == BOOL) return ERR_SEM_COMP; // Výsledkem nesmí být pravdivostní hodnota
    addRetType(retType, (nodeType)data_type);
    *retParamCnt = *retParamCnt + 1;
    generateRetVal(*retParamCnt, psa_var_cnt);
    retVal = ids_exprs_opt(retParamCnt, true, NULL, retType);
    if (retVal == -1) retVal = SUCCESS;
    return retVal;
}

int if_f() {
    //Minulý token byl if
    int cnt = ifCnt++;
    int data_type, paramCnt, rParamCnt; //Zde nevyužito
    int retVal = psa(&data_type, &paramCnt, &rParamCnt, false, NULL);
    if (retVal == -1) return ERR_SYNTAX; //Nezadán výraz => chyba
    if (retVal != SUCCESS) return retVal;
    if (data_type != BOOL) return ERR_SEM_COMP; // Výsledkem musí být pravdivostní hodnota
    generateIfJump(cnt, psa_var_cnt); // If podmínka
    if (token->type == LC_BRACKET && getToken()->type == EOL_T) {
        PushFrame(&stack); //Začátek těla if
        getNonEolToken();
        retVal = body();
        if (retVal != SUCCESS) return retVal;

        if (token->type != RC_BRACKET) return ERR_SYNTAX;
        getToken();
        PopFrame(&stack); //Konec těla if

        generateJump("if-end", cnt);
        generateLabel("else", cnt);

        retVal = else_f();

        generateLabel("if-end", cnt);
    }
    else retVal = ERR_SYNTAX;

    return retVal;
}

int for_f() {
    forScope++;
    int cnt = forCnt++;
    printInstr("# --------- FOR start");
    int retVal = def();
    if (retVal != SUCCESS && retVal != -1) return retVal;

    if (token->type == SEMICOLON) {
        generateLabel("for-start", cnt);
        generateForFrame(true, 0); // Vygeneruj nový TF, vlož do něj proměnné a udělej z něho LF
        getToken();
        int data_type, paramCnt, rParamCnt;
        retVal = psa(&data_type, &paramCnt, &rParamCnt, false, NULL);
        if (retVal == -1) return ERR_SYNTAX; //Nezadán výraz => chyba
        if (retVal != SUCCESS) return retVal;
        if (data_type != BOOL) return ERR_SEM_COMP; // Výsledkem musí být pravdivostní hodnota
        generateForJump(cnt, psa_var_cnt); // For podmínka
        generateJump("for-body", cnt);
        generateLabel("for-assign", cnt);
        if (token->type == SEMICOLON) {
            getToken();
            retVal = assign();
            if (retVal != SUCCESS) return retVal;
            generateForFrame(false, 0); // Z LF udělej TF a aktualizuj proměnné
            generateJump("for-start", cnt);
            generateLabel("for-body", cnt);
            if (token->type == LC_BRACKET && getToken()->type == EOL_T) {
                PushFrame(&stack); //Začátek těla for
                getNonEolToken();
                retVal = body();
                if (retVal != SUCCESS && retVal != -1) return SUCCESS;
                if (token->type != RC_BRACKET) return ERR_SYNTAX;
                generateJump("for-assign", cnt);
                generateLabel("for-end", cnt);
                PopFrame(&stack); //Konec těla for
                generateForFrame(false, 0); // Z LF udělej TF a aktualizuj proměnné
                PopFrame(&stack); //Konec definiční části for cyklu
                printInstr("# --------- FOR end");
                getToken();
            }
            else return ERR_SYNTAX;
        }
        else return ERR_SYNTAX;
    }
    else return ERR_SYNTAX;
    forScope--;
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
        retVal = psa(&data_type, &paramCnt, &rParamCnt, false, NULL);
        if (retVal == -1) retVal = ERR_SYNTAX;
        if (data_type == BOOL) return ERR_SEM_COMP; // Výsledkem nesmí být pravdivostní hodnota
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
        retVal = func(&retParamCnt, &paramCnt, idName, NULL);

        if (*lParamCnt != retParamCnt) return ERR_SEM_FUNC; //Kontrola počtu parametrů na levé a pravé straně
    }
    else if (token->type == DEF) { //<after_id> → <def_var>
        retVal = def_var(idName);
    }
    else if (token->type == EQUALS || token->type == COMMA) { //<after_id> → <ids_lo> = <assign_r>
        TSExitIfNotDefined(stack.top, idName, false); //Kontrola, jestli nepřiřazujeme do nedefinované proměnné
        /*if (token->type == ID) addPos(isUsedList, true, idName);
        else if (token->type == UNDERSCORE) addPos(isUsedList, false, "");*/
        retVal = ids_l_opt(lParamCnt, isUsedList);
        if (retVal != SUCCESS && token->type != EQUALS) return ERR_SYNTAX;
        getToken();
        retVal = assign_r(lParamCnt, isUsedList);
    }
    else
        return ERR_SYNTAX;

    //printf("AFTER_ID: %i\n", retVal);
    return retVal;
}

int ids_l(IsUsedList *isUsedList) {
    if (token->type == ID) {
        string idName;
        strInit(&idName);
        strCopyString(&idName, &(token->string));

        int tmpVar;
        TNode *var = TSSearchStackAndReturn(stack.top, idName.str, &tmpVar);
        if (var != NULL)
            addPos(isUsedList, true, var->type, idName.str);
        else
            addPos(isUsedList, true, UNKNOWN, idName.str);

        strFree(&idName);
        getToken();
        TSExitIfNotDefined(stack.top, token->string.str, false); //Kontrola, jestli nepřiřazujeme do nedefinované proměnné
        return SUCCESS;
    }
    else if (token->type == UNDERSCORE) {
        addPos(isUsedList, false, UNKNOWN, "");
        getToken();
        return SUCCESS;
    }
    else return ERR_SYNTAX;
}

int ids_l_opt(int* lParamCnt, IsUsedList *isUsedList) {
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        getToken();
        retVal = ids_l(isUsedList);
        *lParamCnt = *lParamCnt + 1;
        if (retVal != SUCCESS) return retVal;
        retVal = ids_l_opt(lParamCnt, isUsedList);
    }

    //printf("IDS_L_OPT: %i\n", retVal);
    return retVal;
}

int func(int* retParamCnt, int* paramCnt, char* funcName, IsUsedList* isUsedList) { //DONE ^^
    int retVal = ERR_SYNTAX;
    TNode* localTS = NULL;

    TNode *node = TSSearch(stack.top->node, funcName);
    if (node != NULL) {
        if (node->type != FUNC && node->isDefined == true)
            return ERR_SEM_OTHER; // Volá se funkce, která je překrytá proměnnou
    }

    if (token->type == L_BRACKET) {
        *paramCnt = 0;
        getNonEolToken(); //za závorkou možný EOL
        generateBeforeParamPass();
        retVal = params(paramCnt, &localTS, funcName);
        if (retVal == ERR_SYNTAX || token->type != R_BRACKET) return ERR_SYNTAX;
        //printf("---- Volána funkce: %s, paramCnt: %i\n", funcName, *paramCnt);
        TSInsertFuncOrCheck(stack.bottom, funcName, *paramCnt, localTS, retParamCnt, isUsedList, NULL, false);
        
        if (strcmp(funcName, "print") == 0)
            generatePrintCall(*paramCnt);
        else
            generateFuncCall(funcName);

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
            TSInsert(localTS, idName.str, paramNode->type, false, *paramCnt, NULL);
        }
        else {
            char str[getStrSize(*paramCnt)];
            sprintf(str, "%d", *paramCnt);
            TSInsert(localTS, str, nodeTypeFromTokenType(token->type), false, *paramCnt, NULL);
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
                string idName;
                strInit(&idName);
                strCopyString(&idName, &(token->string));

                TSExitIfNotDefined(stack.top, idName.str, false); //Nelze použít nedefinovaný identifikátor jako parametr
                TNode *paramNode = TSSearchStackExceptFunc(stack.top, idName.str);
                TSInsert(localTS, idName.str, paramNode->type, false, *paramCnt, NULL);
            }
            else {
                char str[getStrSize(*paramCnt)];
                sprintf(str, "%d", *paramCnt);
                TSInsert(localTS, str, nodeTypeFromTokenType(token->type), false, *paramCnt, NULL);
            }
            *paramCnt = *paramCnt + 1;
            generateParamPass(*paramCnt, token); //-- Generování proměnné v dočasném rámci
            getToken();
            retVal = params_opt(paramCnt, localTS, funcName);
        }
        else retVal = ERR_SYNTAX;
    }

    //printf("IDS_OPT: %i\n", retVal);
    return retVal;
}

int assign_r(int* lParamCnt, IsUsedList* isUsedList) { 
    int data_type;
    int funcParamCnt = -1; //Počítadlo pro případné parametry funkce, pokud zůstane -1, nejedná se o volání funkce
    int rParamCnt = 1; //Začínají se počítat parametry pravé strany přiřazení, u volání funkce zde bude počet návratových hodnot
    int retVal = psa(&data_type, &funcParamCnt, &rParamCnt, true, isUsedList);
    if (retVal == -1) return ERR_SYNTAX;
    if (retVal != SUCCESS) return retVal;
    if (data_type == BOOL) return ERR_SEM_COMP; // Výsledkem nesmí být pravdivostní hodnota

    if (funcParamCnt != -1) { //Následuje přiřazování hodnot vrácených z funkce
        for (int i = 1; i < rParamCnt+1; i++) { //i = 1, rParamCnt+1 protože proměnné s návratovými hodnotami jsou číslovány od 1
            if (isUsedList != NULL && isUsedList->isUsed == true) { //Aktuální hodnota má být přiřazena                
                int scope = TSSearchStackAndReturnScope(stack.top, isUsedList->varName);
                if (scope != -1) generateRetValAssignment(isUsedList->varName, scope, i);
            }
            else if (isUsedList == NULL)
                return ERR_SEM_OTHER;
                
            isUsedList = isUsedList->next;
        }
    }
    else {
        if (isUsedList != NULL && isUsedList->isUsed == true) { //Aktuální hodnota má být přiřazena
            if (isUsedList->type == UNKNOWN || isUsedList->type != (nodeType)data_type)
                return ERR_SEM_COMP;
            int scope = TSSearchStackAndReturnScope(stack.top, isUsedList->varName);
            if (scope != -1) generateValAssignment(isUsedList->varName, scope, psa_var_cnt); 
        }
        else if (isUsedList == NULL) {
            return ERR_SEM_OTHER;
        }
    }
    
    if (funcParamCnt != -1 && token->type != EOL_T) return ERR_SEM_OTHER; //Pokud se přiřazuje výstup funkce, nesmí již následovat další přiřazované hodnoty
    retVal = ids_exprs_opt(&rParamCnt, false, isUsedList, NULL); //Počítání dalších možných ID na pravé straně přiřazení
    //printf("--- Přiřazení lParam %i : rParam %i\n", *lParamCnt, rParamCnt);

    if (retVal != SUCCESS) return retVal;
    
    if (*lParamCnt != rParamCnt) {
        if (funcParamCnt != 0) return ERR_SEM_OTHER;
        else return ERR_SEM_OTHER;
    }
    
    return retVal;
}

int ids_exprs_opt(int* rParamCnt, bool isReturn, IsUsedList* isUsedList, RetType** retType) {
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        getToken();
        int data_type;
        int paramCnt = -1; //Počítadlo pro případné parametry funkce
        retVal = psa(&data_type, &paramCnt, rParamCnt, false, NULL);

        if (paramCnt != -1) return ERR_SYNTAX; //Přiřazení výstupu funkce může být pouze v případě, že je na pravé straně volání funkce jako jediné (zde by bylo 2. v pořadí)

        if (isUsedList != NULL) { //NULL bude v případě, že jsme v return části
            isUsedList = isUsedList->next;
            if (isUsedList == NULL)
                return ERR_SEM_OTHER;
            if (isUsedList->isUsed == true) { //Aktuální hodnota má být přiřazena
                if (isUsedList->type != (nodeType)data_type)
                    return ERR_SEM_COMP;
                int scope = TSSearchStackAndReturnScope(stack.top, isUsedList->varName);
                if (scope != -1) generateValAssignment(isUsedList->varName, scope, psa_var_cnt);
            }
        }
        if (retVal == -1) return ERR_SYNTAX;
        if (retVal != SUCCESS) return retVal;
        if (data_type == BOOL) return ERR_SEM_COMP; // Výsledkem nesmí být pravdivostní hodnota
        
        *rParamCnt = *rParamCnt + 1; 
        if (isReturn) {
            generateRetVal(*rParamCnt, psa_var_cnt);
            
            if (retType != NULL)
                addRetType(retType, (nodeType)data_type);
        }
        retVal = ids_exprs_opt(rParamCnt, isReturn, isUsedList, retType);
    }

    //printf("IDS_EXPR_OPT: %i\n", retVal);
    return retVal;
}

int assign() {
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
            
            int tmpVar;
            TNode *var = TSSearchStackAndReturn(stack.top, idName.str, &tmpVar);
            if (var != NULL)
                isUsedList->type = var->type;
            else
                isUsedList->type = UNKNOWN;

            isUsedList->isUsed = true;
            isUsedListSetVarName(isUsedList, idName.str);
        }
        else {
            isUsedList->type = UNKNOWN;
            isUsedList->isUsed = false;
            isUsedListSetVarName(isUsedList, "");
        }
        getToken();
        int lParamCnt = 1;
        retVal = ids_l_opt(&lParamCnt, isUsedList);
        if (retVal != SUCCESS) return retVal;
        if (token->type == EQUALS) {
            getToken(); 
            retVal = assign_r(&lParamCnt, isUsedList);
        }
        strFree(&idName);
        isUsedListDispose(isUsedList);
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
