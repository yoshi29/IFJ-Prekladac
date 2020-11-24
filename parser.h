/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#pragma once
#include "stdio.h"
#include "stdlib.h"

#include "error.h"
#include "symtable.h"
#include "scanner.h"
#include "psa.h"


/**
 * Hlavní funkce syntaktického analyzátoru
 * @return 1-99 v případě chyby, 0 jinak
 */
int parse(FILE* file);

Token* getToken();

Token* getNonEolToken();

/**
 * Vloží do tabulky symbolů vestavěné funkce
 */
void insert_built_in_funcs(TNode** root);

/**
 * Pravidlo <program> → package main EOL <def_func_o> EOF
 */
int program();

/**
 * Nepovinná definice funkce
 * Pravidlo <def_func_o> → <def_func> EOL <def_func_o>
 * Pravidlo <def_func_o> → ε
 */
int def_func_opt();

/**
 * Definice funkce
 * Pravidlo <def_func> → func id ( <f_params> ) <f_types> { EOL <body> }
 */
int def_func();

/**
 * Nepovinné formální parametry funkce
 * Pravidlo <f_params> → id <type> <f_params_o>
 * Pravidlo <f_params> → ε
 */
int formal_params(int* paramCount, TNode** localTS);

/**
 * Nepovinné formální parametry funkce
 * Pravidlo <f_params_o> → , id <type> <f_params_o>
 * Pravidlo <f_params_o> → ε
 */
int formal_params_opt(int* paramCount, TNode** localTS);

/**
 * Nepovinné návratové typy funkce
 * Pravidlo <f_types> → ( <types> )
 * Pravidlo <f_types> → ε
 */
int func_ret_types();

/**
 * Datový typ, případně další oddělené čárkou
 * Pravidlo <types> → <type> <types_o>
 */
int types();

/**
 * Nepovinné další datové typy
 * Pravidlo <types_o> → , <type> <types_o>
 * Pravidlo <types_o> → ε
 */
int types_opt();


/**
 * Tělo funkce 
 * Pravidlo <body> → ε
 * Pravidlo <body> → <if> EOL <body>
 * Pravidlo <body> → <return> <body>
 * Pravidlo <body> → <for> EOL <body>
 * Pravidlo <body> → id <after_id> EOL <body>
 * Pravidlo <body> → _ <ids_lo> = <assign_r> EOL <body>
 */
int body();

/**
 * Návrat z funkce
 * Pravidlo <return> → return <return_v> EOL
 */
int return_f();

/**
 * Vrácení hodnoty
 * Pravidlo <return_v> → ε 
 * Pravidlo <return_v> → id <ids_o>
 * Pravidlo <return_v> → <expr> <exprs_o>
 */
int return_val();

/**
 * Nepovinné další identifikátory
 * Pravidlo <ids_o> → ε 
 * Pravidlo <ids_o> → , id <ids_o>
 */
int params_opt();

/*
 * Nepovinné další identifikátory, nebo výrazy
 */
int ids_exprs_opt();

/**
 * If
 * Pravidlo <if> → if <expr> { EOL <body> } <else>
 */
int if_f();

/**
 * Else
 * Pravidlo <else> → else { EOL <body> }
 */
int else_f();

/**
 * For
 * Pravidlo <for> → for <def> ; <expr> ; <assign> { EOL <body> }
 */
int for_f();

/**
 * Definice ve for cyklu
 * Pravidlo <def> → ε 
 * Pravidlo <def> → id <def_var>
 */
int def();

/**
 * Definice proměnné
 * Pravidlo <def_var> → ε
 * Pravidlo <def> → id <def_var>
 */
int def_var();

/**
 * Neterminály, které mohou následovat za id v těle funkce
 * Pravidlo <after_id> → <func>
 * Pravidlo <after_id> → <def_var>
 * Pravidlo <after_id> → <ids_lo>
 */
int after_id();

/**
 * Nepovinné další identifikátory na levé straně přiřazení
 * Pravidlo <ids_lo> → ε
 * Pravidlo <ids_lo> → , <ids_l> <ids_lo>
 */
int ids_l_opt();

/**
 * Identifikátor nebo _
 * Pravidlo <ids_l> → id
 * Pravidlo <ids_l> → _
 */
int ids_l();

/**
 * Volání funkce
 * Pravidlo <func> → ( <params> )
 */
int func();

/**
 * Parametry funkce
 * Pravidlo <params> → ε
 * Pravidlo <params> → id <ids_lo>
 */
int params();

/**
 * Pravá strana přiřazení
 * Pravidlo <assign_r> → <expr> <exprs_o>
 * Pravidlo <assign_r> → id <func>
 */
int assign_r();

/**
 * Přiřazení
 * Pravidlo <assign> → id <ids_lo> = <assign_r>
 * Pravidlo <assign> → _ <ids_lo> = <assign_r>
 * Pravidlo <assign> → ε
 */
int assign();

