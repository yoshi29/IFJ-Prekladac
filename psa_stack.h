/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "error.h"
#include "scanner.h"
#include "symtable.h"


typedef enum {
    type_term,
    type_nonterm,
    type_dollar
} PSA_Data_Type;

typedef struct t_psa_stack_elem {
    PSA_Data_Type elem_type; // typ prvku

    tokenType token_type;
    nodeType node_type;

    string string;
    int64_t intNumber;
    double floatNumber;

    int reduce; // příznak pro redukci
    int var_count;
    
    struct t_psa_stack_elem *nextPtr;
} PSA_Stack_Elem;

typedef struct t_psa_stack {
    PSA_Stack_Elem *topPtr;
} PSA_Stack;

/**
 * Vytvoří nový prvek zásobníku
 * @param elem_type Typ prvku
 * @param token_type Typ tokenu
 * @param node_type Datový typ neterminálu
 * @param string Ukazatel na string
 * @param intNumber Celočíselná hodnota
 * @param floatNumber Desetinná hodnota
 * @return Vrací ukazatel na nově vytvořený prvek
 */
PSA_Stack_Elem *elem_create(PSA_Data_Type elem_type, tokenType token_type, nodeType node_type, string *string, int64_t intNumber, double floatNumber);

/**
 * Vytvoří z daného tokenu nový prvek zásobníku
 * @param elem_type Typ prvku
 * @param token Ukazatel na token
 * @return Vrací ukazatel na nově vytvořený prvek
 */
PSA_Stack_Elem *elem_create_from_token(PSA_Data_Type elem_type, Token *token);

/**
 * Nastaví, zda je prvek začátek redukce nebo ne
 * @param elem Ukazatel na prvek
 * @param r 1, pokud má být prvek začátek redukce, jinak 0
 */
void elem_set_reduce(PSA_Stack_Elem *elem, int r);

/**
 * Dealokuje prvek
 * @param elem Ukazatel na prvek
 */
void elem_destroy(PSA_Stack_Elem *elem);

/**
 * Inicializuje zásobník
 * @param s Ukazatel na zásobník
 */
void stack_init(PSA_Stack *s);

/**
 * Vloží prvek do zásobníku
 * @param s Ukazatel na zásobník
 * @param elem Ukazatel na prvek
 */
void stack_push(PSA_Stack *s, PSA_Stack_Elem *elem);

/**
 * Odstraní prvek z vrcholu zásobníku
 * @param s Ukazatel na zásobník
 */
void stack_pop(PSA_Stack *s);

/**
 * Odstraní několik prvků z vrcholu zásobníku
 * @param s Ukazatel na zásobník
 * @param n Počet prvků k odstranění
 */
void stack_pop_n(PSA_Stack *s, int n);

/**
 * Vrací ukazatel na prvek, který je na vrcholu zásobníku
 * @param s Ukazatel na zásobník
 */
PSA_Stack_Elem *stack_top(PSA_Stack *s);

/**
 * Vrací ukazatel na terminál, který je nejblíže vrcholu zásobníku
 * @param s Ukazatel na zásobník
 */
PSA_Stack_Elem *stack_top_term(PSA_Stack *s);

/**
 * Vrací ukazatel na prvek, který je označen jako začátek redukce a je nejblíže vrcholu zásobníku
 * @param s Ukazatel na zásobník
 */
PSA_Stack_Elem *stack_top_reduce(PSA_Stack *s);

/**
 * Odstraní všechny prvky ze zásobníku
 * @param s Ukazatel na zásobník
 */
void stack_free(PSA_Stack *s);
