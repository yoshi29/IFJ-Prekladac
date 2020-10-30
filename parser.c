/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "parser.h"

Token* token;

// ------------- TODO: Smazat až bude get_next_token ze scanneru
int index = 0; 
tokenType t_types[] = {KEYWORD, KEYWORD, KEYWORD, ID, L_BRACKET, R_BRACKET, LC_BRACKET, EOL_T, RC_BRACKET, EOL_T, EOF_T};
char* t_values[] = {"package", "main",   "func",  "main", "",      "",        "",         "",    "",         "",    ""};
//Volá se po úspěšném zpracování non-terminálu; pravděpodobně nikdy nevolat na konci definice funkce, vždy až po jejím volání
//Pravidla, která mohou vést na epsilon budou nastavovat defaultně SUCCESS, pak musí následovat if (token, který tam správně má být, 
//                                                               pokud není použita epsilon varianta) ... možná ne nejrychlejší, ale asi nejjednodušší
void test_get_next() { 
    token->type = t_types[index];
    token->value = t_values[index];
    index++;
}
// -------------

int parse() {
    TTree* symTable = (TTree*)malloc(sizeof(struct tTree));
    TSInit(symTable);
    token = malloc(sizeof(Token));

    insert_built_in_funcs(&(symTable->root));

    // ------------- TODO: Smazat
    test_get_next(); 
    // -------------

    int retVal = program();
    return retVal;
}

//TODO: StoreToken() ... pokusí se od scanneru získat token a uložit ho do proměnné token; pokud žádný nedostane, nastaví token na NULL
//                   ... možná ve StoreToken() dát exit pokud je token NULL; 

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

int program() {
    int retVal = ERR_SYNTAX;

    if (strcmp(token->value, "package") == 0) { //TODO: Ošetřit situaci, že před package bude EOL
        test_get_next();
        if (strcmp(token->value, "main") == 0) {
            test_get_next();

            retVal = def_func_opt();
            if (token->type == EOF_T) {
                retVal = SUCCESS;
            }
        }
    }

    return retVal;
}

int def_func_opt() { //TODO: DONE ^^
    int retVal = SUCCESS;

    retVal = def_func();

    if (token->type == EOL_T) {
        test_get_next();
        retVal = def_func_opt();
    }

    printf("DEF_FUNC_OPT: %i\n", retVal);
    return retVal;
}

int def_func() { //TODO: ALMOST DONE
    int retVal = ERR_SYNTAX;

    if (strcmp(token->value, "func") == 0) {
        test_get_next();
        if (token->type == ID) { //TODO: Zde si uložit token, mohla (a dle pravidel i měla) by to být funkce
            test_get_next();

            if (token->type == L_BRACKET) { //Uložený token vložit do tabulky symbolů jako funkci, pak uložený token smazat
                test_get_next();

                retVal = formal_params();

                if (token->type == R_BRACKET) {
                    test_get_next();

                    retVal = func_ret_types(); 
                    if (token->type == LC_BRACKET) {
                        test_get_next();
                        
                        if (token->type == EOL_T) {
                            test_get_next();

                            retVal = body();

                            if (token->type == RC_BRACKET) retVal = SUCCESS;
                        }
                    }
                }
            }
        }
    }

    printf("DEF_FUNC: %i\n", retVal);
    return retVal;
}

int formal_params() { //TODO: DONE ^^
    int retVal = SUCCESS;

    if (token->type == ID) {
        test_get_next();

        if (token->type == DATA_TYPE) {
            test_get_next();

            retVal = formal_params_opt();
        }
    }

    printf("FORMAL_PARAMS: %i\n", retVal);
    return retVal;
}

int formal_params_opt() { //TODO: DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        test_get_next();

        if (token->type == ID) {
            test_get_next();

            if (token->type == DATA_TYPE) {
                test_get_next();
                retVal = formal_params_opt();
            }
        }
    }

    printf("FORMAL_PARAMS_OPT: %i\n", retVal);
    return retVal;
}

int func_ret_types() { //TODO: DONE ^^
    int retVal = SUCCESS;

    if (token->type == L_BRACKET) {
        test_get_next();

        retVal = types();
        if (token->type == R_BRACKET) {
            test_get_next();
        }
    }

    printf("FUNC_RET_TYPES: %i\n", retVal);
    return retVal;
}

int types() { //TODO: DONE ^^
    int retVal = ERR_SYNTAX;

    if (token->type == DATA_TYPE) {
        test_get_next();

        retVal = types_opt();
    }

    printf("TYPES: %i\n", retVal);
    return retVal;
}

int types_opt() { //TODO: DONE ^^
    int retVal = SUCCESS;

    if (token->type == COMMA) {
        test_get_next();

        if (token->type == DATA_TYPE) {
            test_get_next();

            retVal = types_opt();
        }
    }
    printf("TYPES_OPT: %i\n", retVal);
    return retVal;
}

int body() {
    int retVal = SUCCESS;

    //TODO: Toto je pouze prozatímní řešení pro epsilon
    printf("BODY: %i\n", retVal);
    return retVal;
}
