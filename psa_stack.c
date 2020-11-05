/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#include "psa_stack.h"


PSA_Stack_Elem *elem_create(PSA_Data_Type type, Token *token)
{
    PSA_Stack_Elem *new_elem = (PSA_Stack_Elem *)malloc(sizeof(PSA_Stack_Elem));
    if (new_elem == NULL) return NULL;

    Token *new_token = NULL;
    if (token != NULL)
    {
        new_token = (Token *)malloc(sizeof(Token));
        if (new_token == NULL)
        {
            free(new_elem);
            return NULL;
        }

        new_token->type = token->type;
        new_token->value = token->value;
    }


    new_elem->type = type;
    new_elem->token = new_token;
    new_elem->reduce = 0;
    new_elem->nextPtr = NULL;

    return new_elem;
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
        if (elem->token != NULL)
            free(elem->token);

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
            if (current->type == type_token || current->type == type_dollar)
                return current;

            current = current->nextPtr;
        }
    }
    return NULL;
}

PSA_Stack_Elem* stack_top_reduce(PSA_Stack* s)
{
    if (s != NULL)
    {
        PSA_Stack_Elem* current = s->topPtr;
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