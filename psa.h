/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "error.h"
#include "symtable.h"
#include "scanner.h"
#include "parser.h"
#include "psa_stack.h"
#include "generator.h"

#define PSA_TABLE_SIZE 7

extern int psa_var_cnt;

typedef enum {
	X,	//   (Error)
	S,	// < (Shift)
	R,	// > (Reduce)
	E	// = (Equal)
} psa_table_sign;

typedef enum {
    R_E_PLUS_E,     // E -> E + E
    R_E_MINUS_E,    // E -> E - E
    R_E_MUL_E,      // E -> E * E
    R_E_DIV_E,      // E -> E / E
    R_E_EQ_E,       // E -> E == E
    R_E_NE_E,       // E -> E != E
    R_E_GT_E,       // E -> E > E
    R_E_LT_E,       // E -> E < E
    R_E_GE_E,       // E -> E >= E
    R_E_LE_E,       // E -> E <= E
    R_LBR_E_RBR,    // E -> ( E )
    R_I             // E -> i
} psa_rules;


/**
 * Hlavní funkce precedenční syntaktické analýzy
 * @param data_type Ukazatel na int, kam se má uložit datový typ výrazu
 * @param paramCnt Ukazatel na int, kam se má uložit 1 v případě, že byla načtena funkce
 * @param paramCnt Ukazatel na int, kam se má uložit 0 v případě, že byla načtena funkce, hodnotu rParamCnt má nastavit jiná funkce
 * @param parseFunc Má se zpracovat volání funkce, anebo ne a vracet ERR_SYNTAX
 * @return Vrací 0 při úspěchu, -1 při chybějícím výrazu, 1-99 v případě chyby
 */
int psa(int* data_type, int* paramCnt, int* rParamCnt, bool parseFunc);

/**
 * Zjistí, zda daný token je operátor
 * @param token Ukazatel na token
 * @return Vrací 1 pokud se jedná o operátor, jinak 0
 */
int is_operator(Token *token);

/**
 * Zjistí, zda má daný token ukončovat PSA
 * @param token Ukazatel na token
 * @return Vrací 1 pokud token ukončuje PSA, jinak 0
 */
int is_delimiter(Token *token);

/**
 * Provede redukci na zásobníku a zkontroluje sémantiku
 * @param s Ukazatel na zásobník
 * @param rule Číslo pravidla pro redukci
 */
int reduce(PSA_Stack *s, int rule);

/**
 * Najde pravidlo pro redukci
 * @param start Ukazatel na prvek, kde začíná pravidlo
 * @param end Ukazatel na prvek, kde končí pravidlo
 * @return Při chybě vrací -1, jinak hodnotu z enumu psa_rules
 */
int find_rule(PSA_Stack_Elem *start, PSA_Stack_Elem *end);

/**
 * Podle prvku v zásobníku a tokenu na vstupu určí následující akci
 * @param elem Ukazatel na prvek
 * @param token Ukazatel na token
 * @return Při chybě vrací -1, jinak hodnotu z enumu psa_table_sign
 */
int table_value(PSA_Stack_Elem *elem, Token *token);
