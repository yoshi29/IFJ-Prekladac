/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#include "psa.h"

extern Token* token;
extern Token* getToken();

static int psa_table[PSA_TABLE_SIZE][PSA_TABLE_SIZE] =
{
    //  | r |+- |*/ | ( | ) | i | $ |
        { X , S , S , S , R , S , R },  // r [ == , <= , >= , != , < , > ]
        { R , R , S , S , R , S , R },  // +-
        { R , R , R , S , R , S , R },  // */
        { S , S , S , S , E , S , X },  // (
        { R , R , R , X , R , X , R },  // )
        { R , R , R , X , R , X , R },  // i
        { S , S , S , S , X , S , X }   // $
};

int psa()
{
    int retVal;

    // Inicializace zásobníku
    PSA_Stack stack;
    stack_init(&stack);

    // Do zásobníku se vloží $ pro označení dna zásobníku
    PSA_Stack_Elem *current_elem = elem_create(type_dollar, NULL);
    stack_push(&stack, current_elem);
    
    int check_function = 0;

    if (is_delimiter(token)) // Výraz je prázdný
    {
        stack_pop(&stack);
        return -1;
    }
    else if (token->type == ID) // Na vstupu je ID, mohlo by se jednat o funkci
        check_function = 1;
    
    retVal = SUCCESS;

    while (current_elem->type != type_dollar || !is_delimiter(token))
    {
        int table_result = table_value(current_elem, token);

        if (table_result == X)
        {
            retVal = ERR_SYNTAX;
            break;
        }
        else if (table_result == S) // < (shift)
        {
            PSA_Stack_Elem *new_elem = elem_create(type_token, token);

            if (new_elem == NULL)
            {
                retVal = ERR_COMPILER;
                break;
            }

            stack_push(&stack, new_elem);

            // Najde prvek v zásobníku, který je nad naposledy vybraným terminálem
            while (new_elem->nextPtr != current_elem)
                new_elem = new_elem->nextPtr;

            elem_set_reduce(new_elem, 1);   // Nastaví prvek jako začátek redukce
                
            token = getToken();
        }
        else if (table_result == R) // > (reduce)
        {
            PSA_Stack_Elem *reduce_start = stack_top_reduce(&stack);

            if (reduce_start == NULL) // V zásobníku není žádný prvek, který by byl začátkem redukce
            {
                retVal = ERR_SYNTAX;
                break;
            }

            PSA_Stack_Elem *current_top = stack_top(&stack);

            // Pokusí se najít pravidlo pro redukci
            int rule = check_rule(reduce_start, current_top);

            if (rule == -1)
            {
                retVal = ERR_SYNTAX;
                break;
            }

            // TODO: Nahradit pravou stranu neterminálem, sémantická kontrola
            
            while (current_top != reduce_start)
            {
                current_top = current_top->nextPtr;
                stack_pop(&stack);
            }
            stack_pop(&stack);
            stack_push(&stack, elem_create(type_nonterminal, NULL));

        }
        else if (table_result == E) // = (equal)
        {
            PSA_Stack_Elem *new_elem = elem_create(type_token, token);

            if (new_elem == NULL)
            {
                retVal = ERR_COMPILER;
                break;
            }

            stack_push(&stack, new_elem);
            token = getToken();
        }
        else // Na vstupu byl nesprávný token
        {
            retVal = ERR_SYNTAX;
            break;
        }

        if (check_function && token->type == L_BRACKET) // Jedná se o funkci
        {
            check_function = 0;
            // TODO: předání, sémantika
            retVal = func();
            break;
        }

        current_elem = stack_top_term(&stack);
    }

    stack_free(&stack);

    return retVal;
}

int check_rule(PSA_Stack_Elem *start, PSA_Stack_Elem *end)
{
    int exit = 0;
    int state = 0;
    int result = -1;

    while (!exit)
    {
        switch (state)
        {
        case 0:
            if (end->type == type_nonterminal)
                state = 1;
            else if (end->type == type_token && end->token->type == R_BRACKET)
                state = 2;
            else if (end->type == type_token && end == start)   
            {
                switch (end->token->type)
                {
                case STRING_T: case FLOAT_T: case INT_T: case ID:   // E -> i
                    exit = 1;
                    result = R_I;
                default:
                    break;
                }
            }
            else
                exit = 1;
            break;
        case 1:
            if (end->type != type_token)
            {
                exit = 1;
                break;
            }
            switch (end->token->type)
            {
            case PLUS: state = 10; break;
            case MINUS: state = 11; break;
            case MUL: state = 12; break;
            case DIV: state = 13; break;
            case EQ_T: state = 14; break;
            case NE_T: state = 15; break;
            case GT_T: state = 16; break;
            case LT_T: state = 17; break;
            case GE_T: state = 18; break;
            case LE_T: state = 19; break;
            default: exit = 1; break;
            }
            break;
        case 10:
            exit = 1;
            if (end->type == type_nonterminal && end == start) // E -> E + E
                result = R_E_PLUS_E;
            break;
        case 11:
            exit = 1;
            if (end->type == type_nonterminal && end == start) // E -> E - E
                result = R_E_MINUS_E;
            break;
        case 12:
            exit = 1;
            if (end->type == type_nonterminal && end == start) // E -> E * E
                result = R_E_MUL_E;
            break;
        case 13:
            exit = 1;
            if (end->type == type_nonterminal && end == start) // E -> E / E
                result = R_E_DIV_E;
            break;
        case 14:
            exit = 1;
            if (end->type == type_nonterminal && end == start) // E -> E == E
                result = R_E_EQ_E;
            break;
        case 15:
            exit = 1;
            if (end->type == type_nonterminal && end == start) // E -> E != E
                result = R_E_NE_E;
            break;
        case 16:
            exit = 1;
            if (end->type == type_nonterminal && end == start) // E -> E > E
                result = R_E_GT_E;
            break;
        case 17:
            exit = 1;
            if (end->type == type_nonterminal && end == start) // E -> E < E
                result = R_E_LT_E;
            break;
        case 18:
            exit = 1;
            if (end->type == type_nonterminal && end == start) // E -> E >= E
                result = R_E_GE_E;
            break;
        case 19:
            exit = 1;
            if (end->type == type_nonterminal && end == start) // E -> E <= E
                result = R_E_LE_E;
            break;

        case 2:
            if (end->type == type_nonterminal)
                state = 20;
            else
                exit = 1;
            break;
        case 20:
            exit = 1;
            if (end->type == type_token && end->token->type == L_BRACKET && end == start) // E -> ( E )
                result = R_LBR_E_RBR;
            break;
        }

        end = end->nextPtr;
    }

    return result;
}

// Zatím k ničemu
int is_operator(Token *token)
{
    if (token != NULL)
    {
        switch (token->type)
        {
        case EQ_T: case NE_T: case GT_T: case LT_T: case GE_T: case LE_T: case PLUS: case MINUS: case MUL: case DIV:
            return 1;
        default:
            break;
        }
    }
    return 0;
}

int is_delimiter(Token *token)
{
    // TODO: EOL nemusí nutně ukončovat výraz
    switch (token->type)
    {
    case COMMA: case SEMICOLON: case LC_BRACKET: case EOL_T: case EOF_T:
        return 1;
    default:
        return 0;
    }
}

int table_value(PSA_Stack_Elem *elem, Token *token)
{
    if (elem != NULL && token != NULL)
    {
        int row = -1;
        int col = -1;

        // row
        if (elem->type == type_token)
        {
            switch (elem->token->type)
            {
            case EQ_T: case NE_T: case GT_T: case LT_T: case GE_T: case LE_T:
                row = 0; break;
            case PLUS: case MINUS:
                row = 1; break;
            case MUL: case DIV:
                row = 2; break;
            case L_BRACKET:
                row = 3; break;
            case R_BRACKET:
                row = 4; break;
            case ID: case STRING_T: case FLOAT_T: case INT_T:
                row = 5;
            default:
                break;
            }
        }
        else if (elem->type == type_dollar)
            row = 6;

        // col
        switch (token->type)
        {
        case EQ_T: case NE_T: case GT_T: case LT_T: case GE_T: case LE_T:
            col = 0; break;
        case PLUS: case MINUS:
            col = 1; break;
        case MUL: case DIV:
            col = 2; break;
        case L_BRACKET:
            col = 3; break;
        case R_BRACKET:
            col = 4; break;
        case ID: case STRING_T: case FLOAT_T: case INT_T:
            col = 5;
        default:
            break;
        }
        if (is_delimiter(token))
            col = 6;

        if (row != -1 && col != -1)
            return psa_table[row][col];
    }
    return -1;
}
