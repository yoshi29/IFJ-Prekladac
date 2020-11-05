/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#include "psa.h"

Token *token;

int psa_table[PSA_TABLE_SIZE][PSA_TABLE_SIZE] =
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


int is_delimeter(Token *token)
{
    // TODO: EOL nemusí nutně ukončovat výraz
    if (token->type == COMMA || token->type == SEMICOLON || token->type == LC_BRACKET || token->type == EOF_T || token->type == EOL_T)
        return 1;
    return 0;
}

int table_value(PSA_Stack_Elem *elem, Token *token)
{
    if (elem != NULL && token != NULL)
    {
        int row, col = -1;

        // row
        if (elem->type == type_token)
        {
            switch (elem->token->type)
            {
            case EQ_T:
            case NE_T:
            case GT_T:
            case LT_T:
            case GE_T:
            case LE_T:
                row = 0;
                break;
            case PLUS:
            case MINUS:
                row = 1;
                break;
            case MUL:
            case DIV:
                row = 2;
                break;
            case L_BRACKET:
                row = 3;
                break;
            case R_BRACKET:
                row = 4;
                break;
            case ID:
            case STRING_T:
            case FLOAT_T:
            case INT_T:
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
        case EQ_T:
        case NE_T:
        case GT_T:
        case LT_T:
        case GE_T:
        case LE_T:
            col = 0;
            break;
        case PLUS:
        case MINUS:
            col = 1;
            break;
        case MUL:
        case DIV:
            col = 2;
            break;
        case L_BRACKET:
            col = 3;
            break;
        case R_BRACKET:
            col = 4;
            break;
        case ID:
        case STRING_T:
        case FLOAT_T:
        case INT_T:
            col = 5;
        default:
            break;
        }
        if (is_delimeter(token))
            col = 6;
        
        // kontrola
        if (row != -1 && col != -1)
            return psa_table[row][col];
    }
    return -1;
}


int psa()
{
    int retVal;

    // Inicializace zásobníku
    PSA_Stack stack;
    stack_init(&stack);

    int exit = 0;

    // Do zásobníku se vloží $ pro označení dna zásobníku
    // ( možná bude stačit 'if (stack_top(&stack) == NULL)' )
    PSA_Stack_Elem* current_elem = elem_create(type_dollar, NULL);
    stack_push(&stack, current_elem);
    
    Token *token = test_get_next();

    if (is_delimeter(token))
        return PSA_EXPR_MISSING;


    do {
        current_elem = stack_top_term(&stack);

        int table_result = table_value(current_elem, token);

        if (table_result == X)
        {
            retVal = PSA_WRONG;
            exit = 1;
        }
        else if (table_result == S)
        {
            PSA_Stack_Elem *new_elem = elem_create(type_token, token);

            if (new_elem == NULL)
            {
                // TODO: error
                retVal = PSA_ERR;
                exit = 1;
            }
            else
            {
                elem_set_reduce(new_elem, 1);   // nastaví token jako začátek redukce
                stack_push(&stack, new_elem);
                token = test_get_next();
            }
            
        }
        else if (table_result == R)
        {
            PSA_Stack_Elem* reduce_start = stack_top_reduce(&stack);
            if (reduce_start == NULL)
            {
                retVal = PSA_WRONG;
                exit = 1;
            }

            // TODO
        }
        else if (table_result == E)
        {
            PSA_Stack_Elem* new_elem = elem_create(type_token, token);

            if (new_elem == NULL)
            {
                // TODO: error
                retVal = PSA_ERR;
                exit = 1;
            }
            else
            {
                stack_push(&stack, new_elem);
                token = test_get_next();
            }
        }
        else
        {
            // TODO: error
            retVal = PSA_ERR;
            exit = 1;
        }
    } while (!exit && (!is_delimeter(token) || current_elem->type != type_dollar)); // TODO: upravit podmínku


}