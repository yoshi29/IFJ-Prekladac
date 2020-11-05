/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

#include "error.h"
#include "scanner.h"


typedef enum {
    type_token,
    type_nonterminal,
    type_dollar
} PSA_Data_Type;

typedef struct t_psa_stack_elem {
    PSA_Data_Type type; // typ
    Token* token;   // ukazatel na token (pokud se jedná o token)
    int reduce; // příznak pro redukci
    struct t_psa_stack_elem *nextPtr;
} PSA_Stack_Elem;

typedef struct t_psa_stack {
    PSA_Stack_Elem *topPtr;
} PSA_Stack;

/**
 * Vytvoří nový prvek zásobníku
 * @param type Typ prvku
 * @param token Ukazatel na token (pokud se jedná o token), jinak NULL
 * @return Vrací ukazatel na nově vytvořený prvek
 */
PSA_Stack_Elem *elem_create(PSA_Data_Type type, Token *token);

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
PSA_Stack_Elem* stack_top_reduce(PSA_Stack *s);

/**
 * Dealokuje zásobník i všechny jeho prvky
 * @param s Ukazatel na zásobník
 */
void stack_free(PSA_Stack *s);