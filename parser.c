/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "parser.h"

TTree* symTable;
Token* lastToken;

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

// -------------

int parse(FILE* file) {
    symTable = (TTree*)malloc(sizeof(struct tTree));
    TSInit(symTable);

    TStack stack;
    TStackInit(&stack); // Zásobník je prázdný

    PushFrame(&stack); // Vloží se nový prvek do zásobníku a nastaví se mu hodnoty
    stack.top->node = symTable->root;
    stack.top->next = NULL;

    //TSInsert(&(stack.top->node), "a", FUNC, false, 0, NULL);
    //PushFrame(&stack);
    //TSInsert(&(stack.top->node), "b", FUNC, false, 0, NULL);
    //PushFrame(&stack);
    //TSInsert(&(stack.top->node), "c", FUNC, false, 0, NULL);
    //PopFrame(&stack);

    token = malloc(sizeof(Token));
    strInit(&(token->string));
    lastToken = malloc(sizeof(Token));
    strInit(&(lastToken->string));

    insert_built_in_funcs(&(symTable->root));
    getSourceCode(file);

    int retVal = program();

    //-- Test, jestli se vše uložilo správně do tabulek symbolů
  /*printf("----- SYM_TABLES_CHECK -----\n");
    TNode* fMain = TSSearch(stack.top->node, "b");
    if (fMain != NULL) printf("KEY: %s, TYPE: %i, PARAM_CNT: %i\n", fMain->key, fMain->type, fMain->param);

    char* funcName = "main";
    TNode *fMain = TSSearch(symTable->root, funcName);
    if (fMain != NULL) {
        printf("KEY: %s, TYPE: %i, PARAM_CNT: %i\n", funcName, fMain->type, fMain->param);

        char* paramName = "a";
        TNode* aParam = TSSearch(fMain->localTS, paramName);
        if (aParam != NULL) printf("KEY: %s, TYPE: %i, PARAM_POS: %i\n", paramName, aParam->type, aParam->param);

        paramName = "b";
        TNode* bParam = TSSearch(fMain->localTS, paramName);
        if (bParam != NULL) printf("KEY: %s, TYPE: %i, PARAM_POS: %i\n", paramName, bParam->type, bParam->param);
    }

    funcName = "funkce";
    TNode* fFunkce = TSSearch(symTable->root, funcName);
    if (fFunkce != NULL) {
        printf("KEY: %s, TYPE: %i, PARAM_CNT: %i\n", funcName, fFunkce->type, fFunkce->param);

        char* paramName = "c";
        TNode* cParam = TSSearch(fFunkce->localTS, paramName);
        if (cParam != NULL) printf("KEY: %s, TYPE: %i, PARAM_POS: %i\n", paramName, cParam->type, cParam->param);
    }
    printf("--------- CHECK_END --------\n");*/
    //-- Test konec

    return retVal;
}

void insert_built_in_funcs(TNode** root) { //TODO: Ještě stále nejisté, jak to přesně v tabulkce symbolů bude
    TSInsert(root, "inputs", FUNC, true, 0, NULL); 
    TSInsert(root, "inputi", FUNC, true, 0, NULL);
    TSInsert(root, "inputf", FUNC, true, 0, NULL);

    TSInsert(root, "print", FUNC, true, -1, NULL); //TODO: -1 by mohlo značit, že má n parametrů ??

    TSInsert(root, "int2float", FUNC, true, 1, NULL); //TODO: Jak bude pobíhat kontrola datových typů vestavěných funkcí
    TSInsert(root, "float2int", FUNC, true, 1, NULL);

    TSInsert(root, "len", FUNC, true, 1, NULL);
    TSInsert(root, "substr", FUNC, true, 3, NULL);
    TSInsert(root, "ord", FUNC, true, 2, NULL);
    TSInsert(root, "chr", FUNC, true, 1, NULL);
}

/* ------------------------------------------------------------------------------- */

int program() { //DONE ^^
    int retVal = ERR_SYNTAX;

    if (strCmpConstStr(&(getNonEolToken()->string), "package") == 0
        && strCmpConstStr(&(getToken()->string), "main") == 0
        && getToken()->type == EOL_T) {
            getNonEolToken();
            retVal = def_func_opt();
            if (retVal != SUCCESS) return ERR_SYNTAX;

            if (token->type == EOF_T) {
                retVal = SUCCESS;
            }
    }

    printf("PROGRAM: %i\n", retVal);
    return retVal;
}


int def_func_opt() { //DONE ^^
    int retVal = SUCCESS;

    if (strCmpConstStr(&(token->string), "func") == 0) {
        retVal = def_func();
        if (retVal != SUCCESS) return ERR_SYNTAX;

        if (token->type == EOL_T) {
            getNonEolToken();
            retVal = def_func_opt();
        }
    }

    printf("DEF_FUNC_OPT: %i\n", retVal);
    return retVal;
}

int def_func() { //TODO: ALMOST DONE
    int retVal = ERR_SYNTAX;

    if (strCmpConstStr(&(token->string), "func") == 0 && getToken()->type == ID) {
        lastToken->type = token->type; // Uloží minulý token, který může být názvem funkce
        strCopyString(&(lastToken->string), &(token->string));

        if (getToken()->type == L_BRACKET) { 
            getToken();

            int paramCount = 0; //Počítadlo parametrů funkce
            TNode* localTS = NULL;

            retVal = formal_params(&paramCount, &localTS);
            if (retVal != SUCCESS) return ERR_SYNTAX;

            if (token->type == R_BRACKET) {
                TSInsert(&(symTable->root), strGetStr(&(lastToken->string)), FUNC, true, paramCount, localTS); //vložení informací o funkci do tabulky symbolů
                getToken();
                retVal = func_ret_types();
                if (retVal != SUCCESS) return ERR_SYNTAX;

                if (token->type == LC_BRACKET && getToken()->type == EOL_T) {
                    getNonEolToken();
                    retVal = body();
                    if (retVal != SUCCESS) return ERR_SYNTAX;     

                    if (token->type == RC_BRACKET) {
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

    printf("DEF_FUNC: %i\n", retVal);
    return retVal;
}

int formal_params(int *paramCount, TNode** localTS) { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == ID) {
        string paramName;
        strInit(&paramName);
        strCopyString(&paramName, &(token->string));

        getToken();
        if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING || token->type == DATA_TYPE_NIL) {
            TSInsert(localTS, paramName.str, token->type, true, *paramCount, NULL);
            (*paramCount)++;

            getToken();
            retVal = formal_params_opt(paramCount, localTS);
        }
        else retVal = ERR_SYNTAX;
        strFree(&paramName);
    }

    printf("FORMAL_PARAMS: %i\n", retVal);
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

            if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING || token->type == DATA_TYPE_NIL) {
                TSInsertAndExitOnDuplicity(localTS, paramName.str, token->type, true, *paramCount, NULL);
                (*paramCount)++;

                getToken(); //Načte další token
                retVal = formal_params_opt(paramCount, localTS);
            }
            else retVal = ERR_SYNTAX;

            strFree(&paramName);
        }
        else retVal = ERR_SYNTAX;
    }

    printf("FORMAL_PARAMS_OPT: %i\n", retVal);
    return retVal;
}

int func_ret_types() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == L_BRACKET) {
        getToken();

        retVal = types();
        if (retVal != SUCCESS && token->type != R_BRACKET) return ERR_SYNTAX;
        getToken();
    }

    printf("FUNC_RET_TYPES: %i\n", retVal);
    return retVal;
}

int types() { //DONE ^^
    int retVal = ERR_SYNTAX;

    if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING || token->type == DATA_TYPE_NIL) {
        getToken();

        retVal = types_opt();
    }

    printf("TYPES: %i\n", retVal);
    return retVal;
}

int types_opt() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) { 
        getToken();
        if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING || token->type == DATA_TYPE_NIL) {
            getToken();
            retVal = types_opt();
        }
        else retVal = ERR_SYNTAX;
    }

    printf("TYPES_OPT: %i\n", retVal);
    return retVal;
}

int body() { //DONE ^^
    int retVal = SUCCESS;

    if (strCmpConstStr(&(token->string), "return") == 0) { //<body> → <return> <body>
        getToken();
        retVal = return_f();
        if (retVal != SUCCESS) return ERR_SYNTAX;
        retVal = body();
    }
    else if (strCmpConstStr(&(token->string), "if") == 0) { //<body> → <if> EOL <body>
        getToken();
        retVal = if_f();
        if (retVal != SUCCESS && token->type != EOL_T) return ERR_SYNTAX;
        getNonEolToken();
        retVal = body();
    }
    else if (strCmpConstStr(&(token->string), "for") == 0) { //<body> → <for> EOL <body>
        getToken();
        retVal = for_f();
        if (retVal != SUCCESS && token->type != EOL_T) return ERR_SYNTAX;
        getNonEolToken();
        retVal = body();
    }
    else if (token->type == ID) { //<body> → id <after_id> EOL <body>
        getToken();
        retVal = after_id();
        if (retVal != SUCCESS && token->type != EOL_T) return ERR_SYNTAX;
        getNonEolToken();
        retVal = body();
    }
    else if (token->type == UNDERSCORE) { //<body> → _ <ids_lo> = <assign_r> EOL <body>
        getToken();
        retVal = ids_l_opt();
        if (retVal != SUCCESS && token->type != EQUALS) return ERR_SYNTAX;
        getToken();
        retVal = assign_r();
        if (retVal != SUCCESS && token->type != EOL_T) return ERR_SYNTAX;
        getNonEolToken();
        retVal = body();
    }

    printf("BODY: %i\n", retVal);
    return retVal;
}

int return_f() { //DONE ^^
    if (return_val() == SUCCESS) {
        if (token->type == EOL_T) {
            getNonEolToken();
            printf("RETURN_F: %i\n", 0);
        }
        return SUCCESS;
    }
    else {
        printf("RETURN_F: %i\n", 2);
        return ERR_SYNTAX;
    }
}

int return_val() {
    int retVal = psa();
    printf("RETURN_VAL: %i\n", retVal);

    if (retVal == ERR_SYNTAX) return ERR_SYNTAX;

    retVal = ids_opt();
    if (retVal == -1) retVal = SUCCESS;
    return retVal;
}

int ids_opt() { //TODO: Možná se ještě podívat na opt EOL
    int retVal = SUCCESS;
    
    if (token->type == COMMA) {
        getToken();
        retVal = psa();
        if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;
        retVal = ids_opt();
    }

    printf("IDS_OPT: %i\n", retVal);
    return retVal;
}

int if_f() {
    //Minulý token byl if
    int retVal = psa();
    if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;

    if (token->type == LC_BRACKET && getToken()->type == EOL_T) {
        getNonEolToken();
        retVal = body();
        if (retVal == ERR_SYNTAX) return ERR_SYNTAX;

        if (token->type != RC_BRACKET) return ERR_SYNTAX;

        retVal = else_f();
    }
    else retVal = ERR_SYNTAX;

    return retVal;
}

int for_f() {
    int retVal = def();

    if (retVal == SUCCESS && token->type == SEMICOLON) {
        getToken();
        retVal = psa();
        if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;
        if (token->type == SEMICOLON) {
            getToken();
            retVal = assign();
            if (token->type == LC_BRACKET && getToken()->type == EOL_T) {
                getNonEolToken();
                retVal = body();
                if (retVal == ERR_SYNTAX) return ERR_SYNTAX;
                if (token->type != RC_BRACKET) return ERR_SYNTAX;
                getToken();
            }
        }
        else return ERR_SYNTAX;
    }
    else return ERR_SYNTAX;

    return retVal;
}

int def() { //Asi hotovo
    int retVal = SUCCESS;

    if (token->type == ID) {
        getToken();
        retVal = def_var();
    }

    return retVal;
}

int def_var() {
    int retVal = ERR_SYNTAX;

    if (token->type == DEF) {
        getToken();
        retVal = psa();
        if (retVal == -1) retVal = ERR_SYNTAX;
    }

    return retVal;
}

int after_id() { //DONE ^^
    int retVal = ERR_SYNTAX;

    if (token->type == L_BRACKET) { //<after_id> → <func>
        retVal = func();
    }
    else if (token->type == DEF) { //<after_id> → <def_var>
        retVal = def_var();
    }
    else if (token->type == COMMA) { //<after_id> → <ids_lo>
        retVal = ids_l_opt();
        if (retVal != SUCCESS && token->type != EQUALS) return ERR_SYNTAX;
        getToken();
        retVal = assign_r();
    }

    printf("AFTER_ID: %i\n", retVal);

    return retVal;
}

int ids_l_opt() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        if (getToken()->type == ID) {
            getToken();
            retVal = ids_l();
            if (retVal != SUCCESS) return ERR_SYNTAX;
            retVal = ids_l_opt();
        }
        else retVal = ERR_SYNTAX;
    }

    printf("IDS_L_OPT: %i\n", retVal);
    return retVal;
}

int ids_l() { //DONE ^^
    return (token->type == ID || token->type == UNDERSCORE) ? SUCCESS : ERR_SYNTAX;
}

int func() { //DONE ^^
    int retVal = ERR_SYNTAX;

    if (token->type == L_BRACKET) {
        getNonEolToken(); //za závorkou možný EOL
        retVal = params();
        if (retVal == ERR_SYNTAX || token->type != R_BRACKET) return ERR_SYNTAX;
    }

    printf("FUNC: %i\n", retVal);
    return retVal;
}

int params() {
    int retVal = psa();
    if (retVal == -1) return SUCCESS;
    if (retVal == ERR_SYNTAX) return ERR_SYNTAX;

    retVal = ids_opt(); //TODO: Bude použítí ok?
    if (retVal == -1) retVal = SUCCESS;

    return retVal;
}

int assign_r() {
    printf("----- TYPE: %i, retVal: %i\n", token->type, 0);
    int retVal = psa();
    printf("----- TYPE: %i, retVal: %i\n", token->type, retVal);

    //if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;

    //retVal = ids_opt(); //TODO: Bude v pořádku zde použít ids_opt();

    return retVal;
}

int assign() {
    int retVal = SUCCESS;

    if (token->type == ID || token->type == UNDERSCORE) {
        getToken();
        if (ids_l_opt() == ERR_SYNTAX) return ERR_SYNTAX;
        if (token->type == EQUALS) {
            getToken(); 
            retVal = assign_r();
        }
    }
    return retVal;
}

int else_f() { //DONE ^^
    int retVal = SUCCESS;

    if (strCmpConstStr(&(token->string), "else") == 0) {
        if (getToken()->type == LC_BRACKET && getToken()->type == EOL_T) {
            getNonEolToken();
            retVal = body();
            if (retVal == ERR_SYNTAX || token->type != RC_BRACKET) return ERR_SYNTAX;
            getToken();
        }
        else retVal = ERR_SYNTAX;
    }
    return retVal;
}