/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#include "psa_stack.h"

PSA_Stack_Elem *elem_create(PSA_Data_Type elem_type, tokenType token_type, nodeType node_type, string *string, int64_t intNumber, double floatNumber)
{
    PSA_Stack_Elem *new_elem = (PSA_Stack_Elem *)malloc(sizeof(PSA_Stack_Elem));
    if (new_elem == NULL) return NULL;

    new_elem->elem_type = elem_type;
    new_elem->token_type = token_type;
    new_elem->node_type = node_type;
    new_elem->intNumber = intNumber;
    new_elem->floatNumber = floatNumber;
    strInit(&(new_elem->string));
    if (string != NULL)
        strCopyString(&(new_elem->string), string);

    new_elem->reduce = 0;
    new_elem->nextPtr = NULL;

    return new_elem;
}

PSA_Stack_Elem *elem_create_from_token(PSA_Data_Type elem_type, Token *token)
{
    if (token != NULL)
        return elem_create(elem_type, token->type, -1, &(token->string), token->intNumber, token->floatNumber);

    return NULL;
}

void elem_set_reduce(PSA_Stack_Elem *elem, int r)
{
    if (elem != NULL)
    {
        if (r == 0)
            elem->reduce = 0;
        else
            elem->reduce = 1;
    }
}

void elem_destroy(PSA_Stack_Elem *elem)
{
    if (elem != NULL)
    {
        strFree(&(elem->string));
        free(elem);
    }
}

void stack_init(PSA_Stack *s)
{
    s->topPtr = NULL;
}

void stack_push(PSA_Stack *s, PSA_Stack_Elem *elem)
{
    if (s != NULL && elem != NULL)
    {
        elem->nextPtr = s->topPtr;
        s->topPtr = elem;
    }
}

void stack_pop(PSA_Stack *s)
{
    if (s != NULL && s->topPtr != NULL)
    {
        PSA_Stack_Elem *temp = s->topPtr;
        s->topPtr = s->topPtr->nextPtr;

        elem_destroy(temp);
    }
}

void stack_pop_n(PSA_Stack *s, int n)
{
    if (s != NULL && n > 0)
    {
        for (int i = 0; i < n; i++)
        {
            if (s->topPtr == NULL)
                break;

            PSA_Stack_Elem *temp = s->topPtr;
            s->topPtr = s->topPtr->nextPtr;
            elem_destroy(temp);
        }
    }
}

PSA_Stack_Elem *stack_top(PSA_Stack *s)
{
    if (s != NULL)
        return s->topPtr;

    return NULL;
}

PSA_Stack_Elem *stack_top_term(PSA_Stack *s)
{
    if (s != NULL)
    {
        PSA_Stack_Elem *current = s->topPtr;
        while (current != NULL)
        {
            if (current->elem_type == type_term || current->elem_type == type_dollar)
                return current;

            current = current->nextPtr;
        }
    }
    return NULL;
}

PSA_Stack_Elem *stack_top_reduce(PSA_Stack *s)
{
    if (s != NULL)
    {
        PSA_Stack_Elem *current = s->topPtr;
        while (current != NULL)
        {
            if (current->reduce == 1)
                return current;

            current = current->nextPtr;
        }
    }
    return NULL;
}

void stack_free(PSA_Stack *s)
{
    if (s != NULL)
    {
        while (s->topPtr != NULL)
            stack_pop(s);
    }
}
