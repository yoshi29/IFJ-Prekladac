/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "parser.h"

TTree* symTable;
Token* token;
Token* lastToken;

// ------------- TODO: Smazat až bude get_next_token ze scanneru
int index = 0; 
tokenType t_types[] = {KEYWORD, KEYWORD, KEYWORD, ID, L_BRACKET, R_BRACKET, LC_BRACKET, EOL_T, /**/ID, L_BRACKET, R_BRACKET, EOL_T, RC_BRACKET, EOL_T, EOF_T};
char* t_values[] = {"package", "main",   "func",  "main", "",      "",        "",         "",  /**/"a", "",       "",        "",    "",         "",    ""};
//Volá se po úspěšném zpracování non-terminálu; pravděpodobně nikdy nevolat na konci definice funkce, vždy až po jejím volání
//Pravidla, která mohou vést na epsilon budou nastavovat defaultně SUCCESS, pak musí následovat if (token, který tam správně má být, 
//                                                               pokud není použita epsilon varianta) ... možná ne nejrychlejší, ale asi nejjednodušší
Token* test_get_next() { 
    token->type = t_types[index];
    token->value = t_values[index];
    index++;
    return token;
}
// -------------

int parse() {
    symTable = (TTree*)malloc(sizeof(struct tTree));
    TSInit(symTable);
    token = malloc(sizeof(Token));

    insert_built_in_funcs(&(symTable->root));

    int retVal = program();
    return retVal;
}

//TODO: StoreToken() ... pokusí se od scanneru získat token a uložit ho do proměnné token; pokud žádný nedostane, nastaví token na NULL
//                   ... možná ve StoreToken() dát exit pokud je token NULL; 
//                   ... bude vůbec něco takového třeba?

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

    if (strcmp(test_get_next()->value, "package") == 0 
        && strcmp(test_get_next()->value, "main") == 0) { //TODO: Ošetřit situaci, že před package bude EOL
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

    if (strcmp(token->value, "func") == 0) {
        retVal = def_func();
        if (retVal != SUCCESS) return SUCCESS;

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

    if (strcmp(token->value, "func") == 0 && test_get_next()->type == ID) {
        lastToken = token; // Uloží minulý token, který může být názvem funkce

        if (test_get_next()->type == L_BRACKET) { 
            test_get_next();
            retVal = formal_params(); 
            if (retVal != SUCCESS) return ERR_SYNTAX;
            //TODO: Uložený token vložit do tabulky symbolů jako funkci společně s dalšími informacemi (např. parametry, atd, ještě nwm, co vše bude třeba), pak uložený token smazat

            if (token->type == R_BRACKET) {
                test_get_next();
                retVal = func_ret_types();
                if (retVal != SUCCESS) return ERR_SYNTAX;

                if (token->type == LC_BRACKET && test_get_next()->type == EOL_T) {
                    test_get_next();
                    retVal = body();
                    if (retVal != SUCCESS) return ERR_SYNTAX;

                    if (token->type == RC_BRACKET) retVal = SUCCESS;
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

int formal_params() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == ID) {
        if (test_get_next()->type == DATA_TYPE) {
            test_get_next();
            retVal = formal_params_opt();
        }
        else retVal = ERR_SYNTAX;
    }

    printf("FORMAL_PARAMS: %i\n", retVal);
    return retVal;
}

int formal_params_opt() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        if (test_get_next()->type == ID && test_get_next()->type == DATA_TYPE) {
            test_get_next();
            retVal = formal_params_opt();
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

    if (token->type == DATA_TYPE) {
        test_get_next();

        retVal = types_opt();
    }

    printf("TYPES: %i\n", retVal);
    return retVal;
}

int types_opt() { //DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) { 
        if (test_get_next()->type == DATA_TYPE) {
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

    if (strcmp(token->value, "return") == 0) { //<body> → <return> <body>
        retVal = return_f();
        if (retVal != SUCCESS) return ERR_SYNTAX;
        retVal = body();
    }
    else if (strcmp(token->value, "if") == 0) { //<body> → <if> EOL <body>
        test_get_next();
        retVal = if_f();
        if (retVal != SUCCESS && token->type != EOL_T) return ERR_SYNTAX;
        test_get_next();
        retVal = body();
    }
    else if (strcmp(token->value, "for") == 0) { //<body> → <for> EOL <body>
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
        retVal = assign_r();
        if (retVal != SUCCESS && token->type != EOL_T) return ERR_SYNTAX;
        test_get_next();
        retVal = body();
    }

    printf("BODY: %i\n", retVal);
    return retVal;
}

int return_f() { //DONE ^^
    if (return_val() == SUCCESS && token->type == EOL_T) return SUCCESS; //Počítá s tím, že inulý token byl return
    else return ERR_SYNTAX;
}

int return_val() {
    int retVal = SUCCESS;
    //TODO: Toto celé asi přenechat na řešení Filipovi
    return retVal;
}

int ids_opt() { //Hotovo, ale bude třeba? Nebude to řešit Filip?
    int retVal = SUCCESS;
    
    if (token->type == COMMA && test_get_next()->type == ID) {
        test_get_next();
        retVal = ids_opt();
    }

    printf("IDS_OPT: %i\n", retVal);
    return retVal;
}

int if_f() {
    int retVal = ERR_SYNTAX;
    //Minulý token byl if
    //TODO: Následuje <expr> ...
}

int for_f() {
    //Minulý token byl for
    int retVal = def();
    if (retVal != SUCCESS && token->type != SEMICOLON) return ERR_SYNTAX;

    //TODO: Řešení výrazu
}

int def() {
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
        //TODO: Zpracování výrazu
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
    }

    printf("FUNC: %i\n", retVal);
    return retVal;
}

int params() {
    int retVal = SUCCESS;
    //Možná předat Filipovi
}

int assign_r() {
    int retVal = ERR_SYNTAX;

    //TODO: Nějak předat Filipovi pro vracení výrazů

    if (token->type == ID) {
        test_get_next();
        retVal = func();
    }

    return retVal;
}

int else_f() { //DONE ^^
    int retVal = SUCCESS;

    if (strcmp(token->value, "else") == 0) {
        if (test_get_next()->type == LC_BRACKET && test_get_next()->type == EOL_T) {
            retVal = body();
        }
        else retVal = ERR_SYNTAX;
    }
    return retVal;
}