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

/**
 * Hlavní funkce syntaktického analyzátoru
 * @return 1-99 v případě chyby, 0 jinak
 */
int parse();

/**
 * Vloží do tabulky symbolů vestavěné funkce
 */
void insert_built_in_funcs(TNode** root);

/**
 * Pravidlo <program> → package main <def_func_o> EOF
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
int formal_params();

/**
 * Nepovinné formální parametry funkce
 * Pravidlo <f_params_o> → , id <type> <f_params_o>
 * Pravidlo <f_params_o> → ε
 */
int formal_params_opt();

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
 * TODO: Dodělat zbytek pravidel pro body a upravit LL tabulku
 * Tělo funkce 
 * Pravidlo <body> → ε
 */
int body();