/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#include "psa.h"

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

int psa(int *data_type)
{
    int retVal = SUCCESS;

    // Inicializace zásobníku
    PSA_Stack stack;
    stack_init(&stack);

    // Do zásobníku se vloží $ pro označení dna zásobníku
    PSA_Stack_Elem *current_elem = elem_create(type_dollar, -1, -1, NULL, 0, 0);
    stack_push(&stack, current_elem);

    if (is_delimiter(token)) // Výraz je prázdný
    {
        stack_pop(&stack);
        return -1;
    }

    while (current_elem->elem_type != type_dollar || !is_delimiter(token))
    {
        int table_result = table_value(current_elem, token);

        if (table_result == X)
        {
            retVal = ERR_SYNTAX;
            break;
        }
        else if (table_result == S) // < (shift)
        {
            PSA_Stack_Elem *new_elem = elem_create_from_token(type_term, token);

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

            getNextToken();
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
            int rule = find_rule(reduce_start, current_top);
            if (rule == -1)
            {
                retVal = ERR_SYNTAX;
                break;
            }

            int ret = reduce(&stack, rule);
            if (ret != SUCCESS)
            {
                retVal = ret;
                break;
            }

        }
        else if (table_result == E) // = (equal)
        {
            PSA_Stack_Elem *new_elem = elem_create_from_token(type_term, token);

            if (new_elem == NULL)
            {
                retVal = ERR_COMPILER;
                break;
            }

            stack_push(&stack, new_elem);
            getNextToken();
        }
        else // Na vstupu byl nesprávný token
        {
            retVal = ERR_SYNTAX;
            break;
        }

        current_elem = stack_top_term(&stack);
    }

    if (retVal == SUCCESS)
        *data_type = stack_top(&stack)->node_type;

    stack_free(&stack);
    return retVal;
}

// Zatím pouze kontroluje sémantiku
// Možná bude lepší sloučit s find_rule()
// TODO: Generování kódu/stromu
int reduce(PSA_Stack *s, int rule)
{
    PSA_Stack_Elem *current = stack_top(s);
    TStack_Elem *sym_table = stack.top;

    switch (rule)
    {
    case R_E_PLUS_E: // E -> E + E
        if (current->node_type != current->nextPtr->nextPtr->node_type)
            return ERR_SEM_COMP;
        stack_pop_n(s, 2);
        elem_set_reduce(stack_top(s), 0);
        return SUCCESS;

    case R_E_MINUS_E: // E -> E - E
        if (current->node_type != current->nextPtr->nextPtr->node_type)
            return ERR_SEM_COMP;
        if (current->node_type == STRING)
            return ERR_SEM_COMP;
        stack_pop_n(s, 2);
        elem_set_reduce(stack_top(s), 0);
        return SUCCESS;

    case R_E_MUL_E: // E -> E * E
        if (current->node_type != current->nextPtr->nextPtr->node_type)
            return ERR_SEM_COMP;
        if (current->node_type == STRING)
            return ERR_SEM_COMP;
        stack_pop_n(s, 2);
        elem_set_reduce(stack_top(s), 0);
        return SUCCESS;

    case R_E_DIV_E: // E -> E / E
        if (current->node_type != current->nextPtr->nextPtr->node_type)
            return ERR_SEM_COMP;
        if (current->node_type == STRING)
            return ERR_SEM_COMP;
        if ((current->node_type == INT && current->intNumber == 0) || (current->node_type == FLOAT && current->floatNumber == 0.0))
            return ERR_DIVBYZERO;

        stack_pop_n(s, 2);
        elem_set_reduce(stack_top(s), 0);
        return SUCCESS;

    case R_E_EQ_E: // E -> E == E
        if (current->node_type != current->nextPtr->nextPtr->node_type)
            return ERR_SEM_COMP;
        stack_pop_n(s, 2);
        elem_set_reduce(stack_top(s), 0);
        return SUCCESS;

    case R_E_NE_E: // E -> E != E
        if (current->node_type != current->nextPtr->nextPtr->node_type)
            return ERR_SEM_COMP;
        stack_pop_n(s, 2);
        elem_set_reduce(stack_top(s), 0);
        return SUCCESS;

    case R_E_GT_E: // E -> E > E
        if (current->node_type != current->nextPtr->nextPtr->node_type)
            return ERR_SEM_COMP;
        stack_pop_n(s, 2);
        elem_set_reduce(stack_top(s), 0);
        return SUCCESS;

    case R_E_LT_E: // E -> E < E
        if (current->node_type != current->nextPtr->nextPtr->node_type)
            return ERR_SEM_COMP;
        stack_pop_n(s, 2);
        elem_set_reduce(stack_top(s), 0);
        return SUCCESS;

    case R_E_GE_E: // E -> E >= E
        if (current->node_type != current->nextPtr->nextPtr->node_type)
            return ERR_SEM_COMP;
        stack_pop_n(s, 2);
        elem_set_reduce(stack_top(s), 0);
        return SUCCESS;

    case R_E_LE_E: // E -> E <= E
        if (current->node_type != current->nextPtr->nextPtr->node_type)
            return ERR_SEM_COMP;
        stack_pop_n(s, 2);
        elem_set_reduce(stack_top(s), 0);
        return SUCCESS;

    case R_LBR_E_RBR: // E -> ( E )
        stack_pop(s);
        current = stack_top(s);
        PSA_Stack_Elem *new_elem = elem_create(current->elem_type, current->token_type, current->node_type, &(current->string), current->intNumber, current->floatNumber);
        if (new_elem == NULL) return ERR_COMPILER;
        stack_pop_n(s, 2);
        stack_push(s, new_elem);
        return SUCCESS;

    case R_I: // E -> i
        current->elem_type = type_nonterm;
        elem_set_reduce(current, 0);

        if (current->token_type == ID)
        {
            TNode *node = TSSearchStackAndReturn(sym_table, current->string.str);
            if (node == NULL || node->type == FUNC)
                return ERR_SEM_DEF;
            if (node->isDefined == false)
                return ERR_SEM_DEF;

            current->node_type = node->type;
        }
        else if (current->token_type == INT_T) current->node_type = INT;
        else if (current->token_type == FLOAT_T) current->node_type = FLOAT;
        else if (current->token_type == STRING_T) current->node_type = STRING;
        else return ERR_COMPILER;

        return SUCCESS;

    default:
        return ERR_COMPILER;
    }
}

int find_rule(PSA_Stack_Elem *start, PSA_Stack_Elem *end)
{
    int exit = 0;
    int state = 0;
    int result = -1;

    while (!exit)
    {
        switch (state)
        {
        case 0:
            if (end->elem_type == type_nonterm)
                state = 1;
            else if (end->elem_type == type_term && end->token_type == R_BRACKET)
                state = 2;
            else if (end->elem_type == type_term && end == start)
            {
                switch (end->token_type)
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
            if (end->elem_type != type_term)
            {
                exit = 1;
                break;
            }
            switch (end->token_type)
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
            if (end->elem_type == type_nonterm && end == start) // E -> E + E
                result = R_E_PLUS_E;
            break;
        case 11:
            exit = 1;
            if (end->elem_type == type_nonterm && end == start) // E -> E - E
                result = R_E_MINUS_E;
            break;
        case 12:
            exit = 1;
            if (end->elem_type == type_nonterm && end == start) // E -> E * E
                result = R_E_MUL_E;
            break;
        case 13:
            exit = 1;
            if (end->elem_type == type_nonterm && end == start) // E -> E / E
                result = R_E_DIV_E;
            break;
        case 14:
            exit = 1;
            if (end->elem_type == type_nonterm && end == start) // E -> E == E
                result = R_E_EQ_E;
            break;
        case 15:
            exit = 1;
            if (end->elem_type == type_nonterm && end == start) // E -> E != E
                result = R_E_NE_E;
            break;
        case 16:
            exit = 1;
            if (end->elem_type == type_nonterm && end == start) // E -> E > E
                result = R_E_GT_E;
            break;
        case 17:
            exit = 1;
            if (end->elem_type == type_nonterm && end == start) // E -> E < E
                result = R_E_LT_E;
            break;
        case 18:
            exit = 1;
            if (end->elem_type == type_nonterm && end == start) // E -> E >= E
                result = R_E_GE_E;
            break;
        case 19:
            exit = 1;
            if (end->elem_type == type_nonterm && end == start) // E -> E <= E
                result = R_E_LE_E;
            break;

        case 2:
            if (end->elem_type == type_nonterm)
                state = 20;
            else
                exit = 1;
            break;
        case 20:
            exit = 1;
            if (end->elem_type == type_term && end->token_type == L_BRACKET && end == start) // E -> ( E )
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
        if (elem->elem_type == type_term)
        {
            switch (elem->token_type)
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
        else if (elem->elem_type == type_dollar)
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
