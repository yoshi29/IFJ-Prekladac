/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Tomáš Matušák, xmatus34
 */
#include <scanner.h>

FILE *SourceCode;
char readToken[200];

void GetSouceCode(FILE *Code){
    SourceCode = Code;
}



int GetNextToken(Token *token){

    if (SourceCode == NULL){
        return ERR_COMPILER;
    }

    else
    {
        fopen(SourceCode,r);
        while(fgets(readToken,1,SourceCode)!= NULL){
            switch (readToken)
            {
                case '(':
                    token->type = R_BRACKET;
                    return SUCCESS;    

                    break;
            
                case ')':
                    token->type = L_BRACKET;
                    return SUCCESS;

                    break;

                case '{':
                    token->type = LC_BRACKET;
                    return SUCCESS;

                    break;
            
                case '}':
                    token->type = RC_BRACKET;
                    return SUCCESS;

                    break;
            
                case ';':
                    token->type = SEMICOLON; 
                    return SUCCESS;
                
                    break;
            
                case '+':
                    tekon->type = PLUS;
                    return SUCCESS;
                
                    break;
            
                case '-':
                    token->type = MINUS;
                    return SUCCESS;
                
                    break;
            
                case '*':
                    token->type = MUL;
                    return SUCCESS;
                
                    break;
            
                case '<':
                   fgets(readToken,1,SourceCode);
                    if (readToken == '=')
                    {
                        token->type  = LE_T;
                        return SUCCESS;
                    }
                    else
                    {
                        token->type = LT_T;
                        ungetc(readToken,SourceCode);
                        return SUCCESS;
                    }
                
                    break;
            
                case '>':
                   fgets(readToken,1,SourceCode);
                    if (readToken == '=')
                    {
                        token->type  = GE_T;
                        return SUCCESS;
                    }
                    else
                    {
                        token->type = GT_T;
                        ungetc(readToken,SourceCode);
                        return SUCCESS;
                    }
                
                    break;
            
                case ',':
                    token->type = COMMA;
                
                    break;
                
                case '!':
                    fgets(readToken,1,SourceCode);
                    if (readToken == '=')
                    {
                        token->type  = NE_T;
                        return SUCCESS;
                    }
                    else
                    {
                        ungetc(readToken,SourceCode);
                        return ERR_LEXICAL;
                    }
                
                    break;

            default:

                if(strchr("123456789",readToken) != NULL ){
                        char Numbers[200]

                }
                break;
            }
        }

    }
    
}