/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "parser.h"

TTree* symTable;
Token* lastToken;

// ------------- TODO: Smazat až bude get_next_token ze scanneru
//int index = 0; 
//tokenType t_types[] = {KEYWORD, KEYWORD, KEYWORD, ID,     L_BRACKET, ID, DATA_TYPE_INT, COMMA, ID, DATA_TYPE_STRING, R_BRACKET, LC_BRACKET, EOL_T, KEYWORD, ID, PLUS, INT_T, EOL_T, RC_BRACKET, EOL_T, KEYWORD, ID,     L_BRACKET, ID, DATA_TYPE, R_BRACKET, LC_BRACKET, EOL_T, RC_BRACKET, EOL_T, EOF_T};
//string* t_values[] = {"package", "main",   "func",  "main", "",        "a","int",         "",    "b","string", "",        "",         "",    "return","a","",   "12",  "",    "",         "",    "func",  "funkce","",        "c","float",   "",        "",         "",    "",         "",    "",};
//Volá se po úspěšném zpracování non-terminálu; pravděpodobně nikdy nevolat na konci definice funkce, vždy až po jejím volání
//Pravidla, která mohou vést na epsilon budou nastavovat defaultně SUCCESS, pak musí následovat if (token, který tam správně má být, 
//                                                               pokud není použita epsilon varianta) ... možná ne nejrychlejší, ale asi nejjednodušší
Token* test_get_next() { 
    /*token->type = t_types[index];
    token->string = t_values[index];
    index++;*/
    int retVal = getNextToken();
    //printf("> TYPE: %i, VALUE: %s\n", token->type, strGetStr(&(token->string)));
    if (retVal != SUCCESS) {
        print_err(retVal);
        exit(retVal);
    }
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

    //insert_built_in_funcs(&(symTable->root));
    getSourceCode(file);

    /*
    do {
        if (getNextToken() != ERR_LEXICAL) {
            printf("TOKEN: %i\n", token->type);
        }
        else {
            return ERR_LEXICAL;
        }
    } while (token->type != EOF_T);
    */

    int retVal = program();

    //-- Test, jestli se vše uložilo správně do tabulek symbolů
  /*  printf("----- SYM_TABLES_CHECK -----\n");
    TNode* fMain = TSSearch(stack.top->node, "b");
    if (fMain != NULL) printf("KEY: %s, TYPE: %i, PARAM_CNT: %i\n", fMain->key, fMain->type, fMain->param);*/

    //char* funcName = "main";
    //TNode *fMain = TSSearch(symTable->root, funcName);
    //if (fMain != NULL) {
    //    printf("KEY: %s, TYPE: %i, PARAM_CNT: %i\n", funcName, fMain->type, fMain->param);

    //    char* paramName = "a";
    //    TNode* aParam = TSSearch(fMain->localTS, paramName);
    //    if (aParam != NULL) printf("KEY: %s, TYPE: %i, PARAM_POS: %i\n", paramName, aParam->type, aParam->param);

    //    paramName = "b";
    //    TNode* bParam = TSSearch(fMain->localTS, paramName);
    //    if (bParam != NULL) printf("KEY: %s, TYPE: %i, PARAM_POS: %i\n", paramName, bParam->type, bParam->param);
    //}

    //funcName = "funkce";
    //TNode* fFunkce = TSSearch(symTable->root, funcName);
    //if (fFunkce != NULL) {
    //    printf("KEY: %s, TYPE: %i, PARAM_CNT: %i\n", funcName, fFunkce->type, fFunkce->param);

    //    char* paramName = "c";
    //    TNode* cParam = TSSearch(fFunkce->localTS, paramName);
    //    if (cParam != NULL) printf("KEY: %s, TYPE: %i, PARAM_POS: %i\n", paramName, cParam->type, cParam->param);
    //}
    //printf("--------- CHECK_END --------\n");
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

//nodeType getNodeType(char* dataType) { //Počítá s tím, že dostane korektní název datového typu
//    if (strcmp(dataType, "int") == 0) return INT;
//    else if (strcmp(dataType, "float") == 0) return FLOAT;
//    else if (strcmp(dataType, "string") == 0) return STRING;
//    else return NIL;
//}

/* ------------------------------------------------------------------------------- */

int program() { //DONE ^^
    int retVal = ERR_SYNTAX;

    if (strCmpConstStr(&(test_get_next()->string), "package") == 0
        && strCmpConstStr(&(test_get_next()->string), "main") == 0
        && test_get_next()->type == EOL_T) { //TODO: Ošetřit situaci, že před package bude EOL;
            test_get_next();
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
            test_get_next();
            retVal = def_func_opt();
        }
    }

    printf("DEF_FUNC_OPT: %i\n", retVal);
    return retVal;
}

int def_func() { //TODO: ALMOST DONE
    int retVal = ERR_SYNTAX;

    if (strCmpConstStr(&(token->string), "func") == 0 && test_get_next()->type == ID) {
        lastToken->type = token->type; // Uloží minulý token, který může být názvem funkce
        strCopyString(&(lastToken->string), &(token->string));

        if (test_get_next()->type == L_BRACKET) { 
            test_get_next();

            int paramCount = 0; //Počítadlo parametrů funkce
            TNode* localTS = NULL;

            retVal = formal_params(&paramCount, &localTS);
            if (retVal != SUCCESS) return ERR_SYNTAX;

            if (token->type == R_BRACKET) {
                TSInsert(&(symTable->root), strGetStr(&(lastToken->string)), FUNC, true, paramCount, localTS); //vložení informací o funkci do tabulky symbolů
                test_get_next();
                retVal = func_ret_types();
                if (retVal != SUCCESS) return ERR_SYNTAX;

                if (token->type == LC_BRACKET && test_get_next()->type == EOL_T) {
                    test_get_next();
                    retVal = body();
                    if (retVal != SUCCESS) return ERR_SYNTAX;     

                    if (token->type == RC_BRACKET) {
                        test_get_next();
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

        test_get_next();
        if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING || token->type == DATA_TYPE_NIL) {
            TSInsert(localTS, paramName.str, token->type, true, *paramCount, NULL);
            (*paramCount)++;

            test_get_next(); //Načte další token
            printf("1.");
            TSPrint(*localTS);
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
        printf("2.");
        TSPrint(*localTS);
        if (test_get_next()->type == ID) {
            printf("3.");
            TSPrint(*localTS);
            string paramName;
            strInit(&paramName);
            strCopyString(&paramName, &(token->string));
            test_get_next();

            if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING || token->type == DATA_TYPE_NIL) {
                TSInsertAndExitOnDuplicity(localTS, paramName.str, token->type, true, *paramCount, NULL);
                (*paramCount)++;

                test_get_next(); //Načte další token
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
        test_get_next();

        retVal = types();
        if (retVal != SUCCESS && token->type != R_BRACKET) return ERR_SYNTAX;
    }

    printf("FUNC_RET_TYPES: %i\n", retVal);
    return retVal;
}

int types() { //DONE ^^
    int retVal = ERR_SYNTAX;

    if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING || token->type == DATA_TYPE_NIL) {
        test_get_next();

        retVal = types_opt();
    }

    printf("TYPES: %i\n", retVal);
    return retVal;
}

int types_opt() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) { 
        test_get_next();
        if (token->type == DATA_TYPE_INT || token->type == DATA_TYPE_FLOAT || token->type == DATA_TYPE_STRING || token->type == DATA_TYPE_NIL) {
            test_get_next();
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
        test_get_next();
        retVal = return_f();
        if (retVal != SUCCESS) return ERR_SYNTAX;
        retVal = body();
    }
    else if (strCmpConstStr(&(token->string), "if") == 0) { //<body> → <if> EOL <body>
        test_get_next();
        retVal = if_f();
        if (retVal != SUCCESS && token->type != EOL_T) return ERR_SYNTAX;
        test_get_next();
        retVal = body();
    }
    else if (strCmpConstStr(&(token->string), "for") == 0) { //<body> → <for> EOL <body>
        test_get_next();
        retVal = for_f();
        if (retVal != SUCCESS && token->type != EOL_T) return ERR_SYNTAX;
        test_get_next();
        retVal = body();
    }
    else if (token->type == ID) { //<body> → id <after_id> EOL <body>
        test_get_next();
        retVal = after_id();
        if (retVal != SUCCESS && token->type != EOL_T) return ERR_SYNTAX;
        test_get_next();
        retVal = body();
    }
    else if (token->type == UNDERSCORE) { //<body> → _ <ids_lo> = <assign_r> EOL <body>
        test_get_next();
        retVal = ids_l_opt();
        if (retVal != SUCCESS && token->type != EQUALS) return ERR_SYNTAX;
        test_get_next();
        retVal = assign_r();
        if (retVal != SUCCESS && token->type != EOL_T) return ERR_SYNTAX;
        test_get_next();
        retVal = body();
    }

    printf("BODY: %i\n", retVal);
    return retVal;
}

int return_f() { //DONE ^^
    if (return_val() == SUCCESS) {
        if (token->type == EOL_T) {
            test_get_next();
            printf("RETURN_F: %i\n", 0);
        }
        return SUCCESS; //Počítá s tím, že inulý token byl return
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

int ids_opt() { 
    int retVal = SUCCESS;
    
    if (token->type == COMMA) {
        test_get_next();
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

    if (token->type == LC_BRACKET && test_get_next()->type == EOL_T) {
        test_get_next();
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
        test_get_next();
        retVal = psa();
        if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;
        if (token->type == SEMICOLON) {
            test_get_next();
            retVal = assign();

        }
        else return ERR_SYNTAX;
    }
    else return ERR_SYNTAX;

    return retVal;
}

int def() { //Asi hotovo
    int retVal = SUCCESS;

    if (token->type == ID) {
        test_get_next();
        retVal = def_var();
    }

    return retVal;
}

int def_var() {
    int retVal = ERR_SYNTAX;

    if (token->type == DEF) {
        test_get_next();
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
        test_get_next();
        retVal = assign_r();
    }

    printf("AFTER_ID: %i\n", retVal);

    return retVal;
}

int ids_l_opt() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        if (test_get_next()->type == ID) {
            test_get_next();
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
        test_get_next();
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
    int retVal = psa();
    if (retVal == -1 || retVal == ERR_SYNTAX) return ERR_SYNTAX;

    retVal = ids_opt(); //TODO: Bude v pořádku zde použít ids_opt();

    return retVal;
}

int assign() {
    int retVal = SUCCESS;

    if (token->type == ID || token->type == UNDERSCORE) {
        test_get_next();
        if (ids_l_opt() == ERR_SYNTAX) return ERR_SYNTAX;
        if (token->type == EQUALS) {
            test_get_next();
            retVal = assign_r();
        }
    }
    return retVal;
}

int else_f() { //DONE ^^
    int retVal = SUCCESS;

    if (strCmpConstStr(&(token->string), "else") == 0) {
        if (test_get_next()->type == LC_BRACKET && test_get_next()->type == EOL_T) {
            retVal = body();
        }
        else retVal = ERR_SYNTAX;
    }
    return retVal;
}