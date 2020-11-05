/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

#include "error.h"
#include "symtable.h"
#include "scanner.h"
#include "psa_stack.h"

#define PSA_TABLE_SIZE 7

typedef enum {
	PSA_SUCCESS,		// PSA proběhla úspěšně
	PSA_WRONG,			// Výraz je neplatný
	PSA_EXPR_MISSING,	// PSA byla zavolána nad prázdným výrazem
	//PSA_IS_FUNCTION,	// PSA byla zavolána nad funkcí
	PSA_ERR,			// Chyba během PSA
} psa_retval;

typedef enum {
	X,	//   (Error)
	S,	// < (Shift)
	R,	// > (Reduce)
	E	// = (Equal)
} psa_table_sign;


/**
 * Hlavní funkce precedenční syntaktické analýzy
 * @return Vrací hodnotu z enumu psa_retval
 */
int psa();

/**
 * Zjistí, zda má daný token ukončovat PSA
 * @param token Ukazatel na token
 * @return Vrací 1 pokud token ukončuje PSA, jinak 0
 */
int is_delimeter(Token *token);

/**
 * Podle prvku v zásobníku a tokenu na vstupu určí pravidlo
 * @param elem Ukazatel na prvek
 * @param token Ukazatel na token
 * @return Při chybě vrací -1, jinak hodnotu z enumu psa_table_sign
 */
int table_value(PSA_Stack_Elem *elem, Token *token);