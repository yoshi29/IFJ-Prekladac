/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "parser.h"

TTree* symTable;

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
    TSInsert(&(stack.top->node), "inputs", FUNC, true, 0, NULL);

    TSInsert(&(stack.top->node), "inputi", FUNC, true, 0, NULL);
    TSInsert(&(stack.top->node), "inputf", FUNC, true, 0, NULL);

    TSInsert(&(stack.top->node), "print", FUNC, true, -1, NULL); //TODO: -1 by mohlo značit, že má n parametrů ??

    TSInsert(&(stack.top->node), "int2float", FUNC, true, 1, NULL); //TODO: Jak bude pobíhat kontrola datových typů vestavěných funkcí
    TSInsert(&(stack.top->node), "float2int", FUNC, true, 1, NULL);

    TSInsert(&(stack.top->node), "len", FUNC, true, 1, NULL);
    TSInsert(&(stack.top->node), "substr", FUNC, true, 3, NULL);
    TSInsert(&(stack.top->node), "ord", FUNC, true, 2, NULL);
    TSInsert(&(stack.top->node), "chr", FUNC, true, 1, NULL);
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
        case DATA_TYPE_INT: 
            return INT;
        case DATA_TYPE_FLOAT:
            return FLOAT;
        case DATA_TYPE_STRING:
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

int def_func() { //TODO: ALMOST DONE
    int retVal = ERR_SYNTAX;
    int isMain = 0;

    if (strCmpConstStr(&(token->string), "func") == 0 && getToken()->type == ID) {
        string funcName;
        strInit(&funcName);
        strCopyString(&funcName, &(token->string));

        if (strCmpConstStr(&funcName, "main") == 0) isMain = 1;

        if (getToken()->type == L_BRACKET) { 
            getToken();

            int paramCount = 0; //Počítadlo parametrů funkce
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
                    retVal = body();

                    if (retVal != SUCCESS) return retVal;

                    if (token->type == RC_BRACKET) {
                        PopFrame(&stack); //Konec těla funkce
                        getToken();
                        retVal = SUCCESS;
                    }
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

    if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING) {
        if (isMain == 1) return ERR_SEM_FUNC;
        addRetType(currentFuncNode, nodeTypeFromTokenType(token->type));
        getToken();

        retVal = types_opt();
    }

    //printf("TYPES: %i\n", retVal);
    return retVal;
}

int types_opt() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) { 
        getToken();
        if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING) {
            addRetType(currentFuncNode, nodeTypeFromTokenType(token->type));
            getToken();
            retVal = types_opt();
        }
        else retVal = ERR_SYNTAX;
    }

    //printf("TYPES_OPT: %i\n", retVal);
    return retVal;
}

int body() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == KEYWORD && strCmpConstStr(&(token->string), "return") == 0) { //<body> → <return> <body>
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
    else if (token->type == ID) { //<body> → id <after_id> EOL <body>
        int lParamCnt = 1;
        string idName;
        strInit(&idName);
        strCopyString(&idName, &(token->string));

        getToken();
        retVal = after_id(idName.str, &lParamCnt);
        strFree(&idName);
        if (retVal != SUCCESS) return retVal;
        if (token->type != EOL_T) return ERR_SYNTAX;
        getNonEolToken();
        retVal = body();
    }
    else if (token->type == UNDERSCORE) { //<body> → _ <ids_lo> = <assign_r> EOL <body>
        int lParamCnt = 1;
        getToken();
        retVal = ids_l_opt(&lParamCnt);
        if (retVal != SUCCESS) return retVal;
        if (token->type != EQUALS) return ERR_SYNTAX;
        getToken();
        retVal = assign_r(&lParamCnt);
        if (retVal != SUCCESS) return retVal;
        if (token->type != EOL_T) return ERR_SYNTAX;
        getNonEolToken();
        retVal = body();
    }

    //printf("BODY: %i\n", retVal);
    return retVal;
}

int return_f() { //DONE ^^
    int rParamCnt = 0;
    if (return_val(&rParamCnt) == SUCCESS) {
        if (token->type == EOL_T) {
            getNonEolToken();
            //printf("RETURN_F: %i\n", 0);
            return SUCCESS;
        }
        else return ERR_SYNTAX; 
    }
    else {
        //printf("RETURN_F: %i\n", 2);
        return ERR_SYNTAX;
    }
}

int return_val(int* rParamCnt) {
    int data_type;
    int retVal = psa(&data_type);
    if (retVal != SUCCESS) return SUCCESS;
    *rParamCnt = *rParamCnt + 1;

    retVal = ids_exprs_opt(rParamCnt);
    if (retVal == -1) retVal = SUCCESS;
    return retVal;
}

int params_opt() { //TODO: Možná se ještě podívat na opt EOL
    int retVal = SUCCESS;
    
    if (token->type == COMMA) {
        getToken();
        if (token->type == ID || token->type == FLOAT_T || token->type == INT_T || token->type == STRING_T) {
            if (token->type == ID) {
                TSExitIfNotDefined(stack.top, token->string.str, false); //Nelze použít nedefinovaný identifikátor jako parametr
            }
            getToken();
            retVal = params_opt();
        }
    }

    //printf("IDS_OPT: %i\n", retVal);
    return retVal;
}

int if_f() {
    //Minulý token byl if
    int data_type;
    int retVal = psa(&data_type);
    if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;

    if (token->type == LC_BRACKET && getToken()->type == EOL_T) {
        PushFrame(&stack); //Začátek těla if
        getNonEolToken();
        retVal = body();
        if (retVal != SUCCESS) return SUCCESS;;

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
        int data_type;
        retVal = psa(&data_type);
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

int def_var(char* idName) { //TODO: vkládání do symtable by se asi nedělalo v after_id, ale zde, kde bychom dostali informaci o typu od psa()
    int retVal = ERR_SYNTAX;

    if (token->type == DEF) {
        getToken();
        int data_type;
        retVal = psa(&data_type);
        if (retVal == -1) retVal = ERR_SYNTAX;
        TSInsertOrExitOnDuplicity(&(stack.top->node), idName, data_type, true, 0, NULL);
    }

    return retVal;
}

int after_id(char* idName, int* lParamCnt) { //DONE ^^
    int retVal = ERR_SYNTAX;

    if (token->type == L_BRACKET) { //<after_id> → <func>
        //TODO: Insert do &stack.top, potřeba získat počet parametrů a lokální TS, udělat podobně jako při definici funkce; 
        //bude-li už definovaná, kontrola typů/pořadí/počtu parametrů - (pokud bude sedět, přepis klíčů na reálně použité?; 
        //isDefined funkce ale musí zůstat na true), pokud nebude sedět pak hodit patřičnou chybu
        retVal = func();
    }
    else if (token->type == DEF) { //<after_id> → <def_var>
        retVal = def_var(idName);
    }
    else { //<after_id> → <ids_lo>
        TSExitIfNotDefined(stack.top, idName, false); //Kontrola, jestli nepřiřazujeme do nedefinované proměnné
        retVal = ids_l_opt(lParamCnt);
        if (retVal != SUCCESS && token->type != EQUALS) return ERR_SYNTAX;
        getToken();
        retVal = assign_r(lParamCnt);
    }

    //printf("AFTER_ID: %i\n", retVal);

    return retVal;
}

int ids_l_opt(int* lParamCnt) { //DONE ^^ HERE
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        getToken();
        retVal = ids_l();
        *lParamCnt = *lParamCnt + 1;
        if (retVal != SUCCESS) return retVal;
        retVal = ids_l_opt(lParamCnt);
    }

    //printf("IDS_L_OPT: %i\n", retVal);
    return retVal;
}

int ids_l() {
    if (token->type == ID) {
        getToken();
        TSExitIfNotDefined(stack.top, token->string.str, false); //Kontrola, jestli nepřiřazujeme do nedefinované proměnné
        return SUCCESS;
    }
    else if (token->type == UNDERSCORE) {
        getToken();
        return SUCCESS;
    }
    else return ERR_SYNTAX;
}

int func() { //DONE ^^
    int retVal = ERR_SYNTAX;

    if (token->type == L_BRACKET) {
        getNonEolToken(); //za závorkou možný EOL
        retVal = params();
        if (retVal == ERR_SYNTAX || token->type != R_BRACKET) return ERR_SYNTAX;
        getNextToken();
    }

    //printf("FUNC: %i\n", retVal);
    return retVal;
}

int params() {
    int retVal = SUCCESS;
    
    if (token->type == ID || token->type == FLOAT_T || token->type == INT_T || token->type == STRING_T) {
        if (token->type == ID) {
            TSExitIfNotDefined(stack.top, token->string.str, false); //Nelze použít nedefinovaný identifikátor jako parametr
        }
        getToken();
        retVal = params_opt();
    }

    return retVal;
}

int assign_r(int* lParamCnt) { 
    int data_type;
    int retVal = psa(&data_type);
    if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;
    int rParamCnt = 1;

    retVal = ids_exprs_opt(&rParamCnt);
    if (retVal != SUCCESS) return retVal;
    //if (*lParamCnt != rParamCnt) return ERR_SEM_FUNC; //TODO: Nemůže být odkomentováno, dokud se nebudou do rParamCnt připočítávat návratové hodnoty funkce
    if (*lParamCnt != rParamCnt) {} //Pouze, aby nenastávaly warningy ohledně nepoužívané proměnné, pak odstranit
    return retVal;
}

int ids_exprs_opt(int* rParamCnt) {
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        getToken();
        int data_type;
        retVal = psa(&data_type);
        if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;
        *rParamCnt = *rParamCnt + 1;
        retVal = ids_exprs_opt(rParamCnt);
    }

    //printf("IDS_EXPR_OPT: %i\n", retVal);
    return retVal;
}

int assign() {
    int retVal = SUCCESS;

    if (token->type == ID || token->type == UNDERSCORE) {
        if (token->type == ID) TSExitIfNotDefined(stack.top, token->string.str, false); //Kontrola, jestli nepřiřazujeme do nedefinované proměnné
        getToken();
        int lParamCnt = 1;
        retVal = ids_l_opt(&lParamCnt);
        if (retVal != SUCCESS) return retVal;
        if (token->type == EQUALS) {
            getToken(); 
            retVal = assign_r(&lParamCnt);
        }
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