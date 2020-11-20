/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Tomáš Matušák, xmatus34
 */
#include "scanner.h"

Token *token = NULL;
FILE *sourceCode;
char readToken[200];
int state = START_STATE;

void getSourceCode(FILE *code){
    sourceCode = code;
}

int getNextToken(){

    if (sourceCode == NULL){
        return ERR_COMPILER;
    }

    int c;
    int c_prev;
    string s;

    if (strInit(&s) == 1)
        return ERR_COMPILER; //TODO: Je to správná chyba?

    while((c = getc(sourceCode)) != EOF) {
        switch (state)
        {
            case START_STATE:
                if (c == '(') token->type = L_BRACKET;
                else if (c == ')') token->type = R_BRACKET;
                else if (c == '{') token->type = LC_BRACKET;
                else if (c == '}') token->type = RC_BRACKET;
                else if (c == ';') token->type = SEMICOLON;
                else if (c == '+') token->type = PLUS; //TODO: Nastavit stav, kdy to může být +, anebo kladné číslo; řešit asi podobně jako třeba LE_T_STATE
                else if (c == '-') token->type = MINUS; //TODO: Nastavit stav, kdy to může být -, anebo záporné číslo; řešit asi podobně jako třeba LE_T_STATE
                else if (c == '*') token->type = MUL;
                else if (c == ',') token->type = COMMA;
                else if (c == '/') state = COMMENT_STATE;
                else if (c == '<') state = LE_T_STATE;
                else if (c == '>') state = GE_T_STATE;
                else if (c == ':') state = DEF_STATE;
                else if (c == '=') state = EQ_T_STATE;
                else if (c == '!') state = NE_T_STATE;
                else if (c == '"') state = STRING_STATE;
                else if (isspace(c)) {
                    if (c == '\n') token->type = EOL_T;
                    else continue; //Přeskočení mezery
                }
                else if (isalpha(c) || c == '_') {
                    strAddChar(&s, c);
                    c_prev = c;
                    state = TEXT_STATE;
                }
                else if (isdigit(c)) { 
                    strAddChar(&s, c);
                    if ((c = getc(sourceCode)) == '\n') { //Následuje konec řádku => konec čísla
                        token->type = INT_T;
                        token->intNumber = atoi(strGetStr(&s));
                        ungetc(c, sourceCode);
                        state = START_STATE;
                    }
                    else {
                        state = INT_STATE;
                    }
                }
                else { // Chybný znak
                    return ERR_LEXICAL;
                }
            break;

            case EQ_T_STATE: //f9 nebo f18
                if (c == '=') token->type = EQ_T;
                else {
                    token->type = EQUALS;
                    ungetc(c, sourceCode);
                }
                state = START_STATE;
            break;

            case LE_T_STATE: //f16 nbo f17
                if (c == '=') token->type = LE_T;
                else {
                    token->type = LT_T;
                    ungetc(c, sourceCode);
                }
                state = START_STATE;
            break;

            case GE_T_STATE: //f19 nebo f20
                if (c == '=') token->type = GE_T;
                else {
                    token->type = GT_T;
                    ungetc(c, sourceCode);
                }
                state = START_STATE;
            break;

            case NE_T_STATE: //v2 nebo v21
                if (c == '=') token->type = NE_T;
                else {
                    ungetc(c, sourceCode);
                    return ERR_LEXICAL;
                }
                state = START_STATE;
            break;

            case DEF_STATE: //v1 nebo f15
                if (c == '=') token->type = DEF;
                else {
                    ungetc(c, sourceCode);
                    return ERR_LEXICAL;
                }
                state = START_STATE;
            break;

            case INT_STATE: //f3
                if (isdigit(c)) {
                    if (strCmpConstStr(&s, "0") == 0) return ERR_LEXICAL; //Číslo začíná na 0 a nenásleduje ./e/E => chyba
                    strAddChar(&s, c);
                }
                else if (c == '.') {
                    if (isSpaceNext() == 1) return ERR_LEXICAL;
                    state = FLOAT_STATE;
                    strAddChar(&s, c);
                }
                else if (c == 'e' || c == 'E') {
                    if (isSpaceNext() == 1) return ERR_LEXICAL; //Nesmí končit na ./e/E
                    state = FLOAT_EXPONENT_STATE;
                    strAddChar(&s, c);
                }
                else { //Konec celého čísla
                    newToken(INT_T, &s, c);
                }
            break;

            case FLOAT_STATE: //v7
                if (isdigit(c)) {
                    strAddChar(&s, c);
                    state = FLOAT_EXPONENT2_STATE;
                }
                else {
                    newToken(FLOAT_T, &s, c);
                }
            break;

            case FLOAT_EXPONENT_STATE: //v4
                if (c == '+' || c == '-') { //TODO: Tady buď bude chyba, pokud následující znak bude whitespace, anebo konec tokenu a unget; které?
                    strAddChar(&s, c);
                    state = SIGNED_FLOAT_STATE;
                }
                else if (isdigit(c)) {
                    strAddChar(&s, c);
                }
                else {
                    newToken(FLOAT_T, &s, c);
                }
            break;

            case FLOAT_EXPONENT2_STATE: //f4
                if (c == 'e' || c == 'E') {
                    if (isSpaceNext() == 1) return ERR_LEXICAL; //Nesmí končit na ./e/E
                    state = FLOAT_EXPONENT_STATE;
                    strAddChar(&s, c);
                }
                if (isdigit(c)) {
                    strAddChar(&s, c);
                }
                else {
                    newToken(FLOAT_T, &s, c);
                }
            break;

            case COMMENT_STATE: //k1
                if(c == '/'){   
                    state = ONE_LINE_C_STATE;
                    }     
                else if (c == '*'){ //k3
                    state = M_LINE_C_STATE;            
                    }
                else {
                    token->type = DIV;
                    ungetc(c,sourceCode);
                    state = START_STATE;
                }
            break;

            case ONE_LINE_C_STATE: //k2
                if (c == '\n') {
                    token->type = EOL_T;
                    state = START_STATE;
                }
            break;

            // Jsme v blokovém komentáři
            case M_LINE_C_STATE:
                if (c == '*') { // k4
                    state = M_LINE_C_END_STATE;
                }
            break;

            case M_LINE_C_END_STATE:
                if (c == '/') {
                    state = START_STATE;
                    continue; // Komentář se přeskočí, SUCCESS se nevrátí
                }
                else {
                    state = M_LINE_C_STATE;
                }
            break;

            case TEXT_STATE: // f2
                if (isalpha(c) || isdigit(c) || c == '_') {
                    c_prev = '\0';
                    strAddChar(&s, c);
                }
                else if (c_prev == '_') {
                    token->type = UNDERSCORE;
                    state = START_STATE;
                    ungetc(c, sourceCode);
                    strFree(&s);
                }
                else {
                    if (strCmpConstStr(&s, "else") == 0 || strCmpConstStr(&s, "for") == 0 || strCmpConstStr(&s, "func") == 0 ||
                        strCmpConstStr(&s, "if") == 0 || strCmpConstStr(&s, "package") == 0 || strCmpConstStr(&s, "return") == 0) {
                        token->type = KEYWORD;
                        strCopyString(&(token->string), &s);
                    }
                    else if (strCmpConstStr(&s, "float64") == 0)    token->type = DATA_TYPE_FLOAT;
                    else if (strCmpConstStr(&s, "int") == 0)        token->type = DATA_TYPE_INT;
                    else if (strCmpConstStr(&s, "string") == 0)     token->type = DATA_TYPE_STRING;
                    else if (strCmpConstStr(&s, "nil") == 0)        token->type = DATA_TYPE_NIL;
                    else {
                        token->type = ID;
                        strCopyString(&(token->string), &s);
                    }
                    state = START_STATE;
                    ungetc(c, sourceCode);
                    strFree(&s);
                }
            break;

            case STRING_STATE:
                if (c == '"') {
                    token->type = STRING_T;
                    strCopyString(&(token->string), &s);
                    state = START_STATE;
                    strFree(&s);
                }
                else if (c == '\\') {
                    state = ESCAPE_STATE;
                }
                else if (c == '\n') {
                    return ERR_LEXICAL;
                }
                else {
                    strAddChar(&s, c);
                }
            break;

            case ESCAPE_STATE:
                state = STRING_STATE;
                switch (c) {
                case '"': strAddChar(&s, c); break;
                case '\\': strAddChar(&s, '\\'); break;
                case 'n': strAddChar(&s, '\n'); break;
                case 't': strAddChar(&s, '\t'); break;
                case 'x': state = HEX_ESCAPE_STATE; break;
                default: return ERR_LEXICAL;
                }
            break;

            case HEX_ESCAPE_STATE:
                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
                    state = HEX_ESCAPE_STATE_2;
                    c_prev = c;
                }
                else return ERR_LEXICAL;
            break;

            case HEX_ESCAPE_STATE_2:
                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
                    char hex[3] = { c_prev, c, '\0' };
                    char c_new = (char)strtol(hex, NULL, 16);
                    strAddChar(&s, c_new);
                    state = STRING_STATE;
                }
                else return ERR_LEXICAL;
            break;

        }

        if (state == START_STATE) {
            return SUCCESS;
        }
    }

    if (c == EOF) {
        strFree(&s);
        token->type = EOF_T;
        return SUCCESS;
    }
    else return ERR_LEXICAL;
}

int isSpaceNext() {
    int c = getc(sourceCode);
    int retVal;
    if (isspace(c) == 0) { //Není whitespace
        retVal = 0;
    }
    else retVal = 1;
    ungetc(c, sourceCode);
    return retVal;
}

void newToken(tokenType type, string *s, int c) { //TODO: Upravit jak bude třeba
    token->type = type;
    if (type == INT_T) {
        token->intNumber = atoi(strGetStr(s));
    }
    else if (type == FLOAT_T) {
        token->floatNumber = atof(strGetStr(s));
    }
    else {
        strCopyString(&(token->string), s);
    }
    state = START_STATE;
    ungetc(c, sourceCode);
    strFree(s);
}