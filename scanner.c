/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Tomáš Matušák, xmatus34
 *        Filip Vágner, xvagne08
 */
#include "scanner.h"

Token *token = NULL;
FILE *sourceCode;
int state = START_STATE;

/**
 * @brief načte soubor zdrojového kódu do proměnné sourceCode
 * 
 * @param code soubor se zdrojovým kódem IFJ20
 */
void getSourceCode(FILE *code){
    sourceCode = code;
}
/**
 * @brief Načte další token a předá informace o něm funkci NewToken
 * 
 * @return celočíselná hodnota určující zda funkce proběhla správně nebo pokud došlo k 
 * jedné z chyb definovaných v err.h, tak k jaké 
 */
int getNextToken(){

    if (sourceCode == NULL)
        return ERR_COMPILER;

    int c;
    int c_prev;
    string s;

    if (strInit(&s) == 1)
        return ERR_COMPILER;

    while(1) {
        c = getc(sourceCode);
        switch (state) // stavy jsou okomentované svými ekvivalenty z diagramu konečného automatu
        {
            case START_STATE:
                if (c == EOF) token->type = EOF_T; //F3
                else if (c == '(') token->type = L_BRACKET; //F4
                else if (c == ')') token->type = R_BRACKET; //F5
                else if (c == '{') token->type = LC_BRACKET; //F6
                else if (c == '}') token->type = RC_BRACKET; //F7
                else if (c == ';') token->type = SEMICOLON; //F8
                else if (c == '+') token->type = PLUS;  //F9
                else if (c == '-') token->type = MINUS; //F10 
                else if (c == '*') token->type = MUL; //F11
                else if (c == ',') token->type = COMMA; //F12
                else if (c == '/') state = COMMENT_STATE;
                else if (c == '<') state = LE_T_STATE;
                else if (c == '>') state = GE_T_STATE;
                else if (c == ':') state = DEF_STATE;
                else if (c == '=') state = EQ_T_STATE;
                else if (c == '!') state = NE_T_STATE;
                else if (c == '"') state = STRING_STATE;
                else if (c == '_') {
                    strAddChar(&s, c);
                    state = UNDERSCORE_STATE;
                }
                else if (isspace(c)) {
                    if (c == '\n') token->type = EOL_T;
                    else continue; //Přeskočení mezery
                }
                else if (isalpha(c)) { // přechod do F22
                    strAddChar(&s, c);
                    c_prev = c;
                    state = TEXT_STATE;
                }
                else if (isdigit(c)) { 
                    strAddChar(&s, c);
                    if (c == '0') {     // přechod do F23
                        state = ZERO_STATE;
                    }
                    else {  // přechod do F24
                        state = NUMBER_STATE;
                    }
                }
                else { // Chybný znak
                    strFree(&s);
                    return ERR_LEXICAL;
                }
            break;

            case EQ_T_STATE: //F17
                if (c == '=') token->type = EQ_T; //přechod do F18 a konec v F18
                else { 
                    token->type = EQUALS; //konec v F17
                    ungetc(c, sourceCode);
                }
                state = START_STATE;
            break;

            case LE_T_STATE: //F13
                if (c == '=') token->type = LE_T; // přechod do F14 a konec v F14
                else {
                    token->type = LT_T; //konec v F14
                    ungetc(c, sourceCode);
                }
                state = START_STATE;
            break;

            case GE_T_STATE: //F15
                if (c == '=') token->type = GE_T; //přechod do F16 a konec v F16
                else {
                    token->type = GT_T; //konec v F15          
                    ungetc(c, sourceCode);
                }
                state = START_STATE;
            break;

            case NE_T_STATE: //Q5
                if (c == '=') token->type = NE_T; //přechod do F20 a konec v F20
                else {
                    strFree(&s);    //konec v Q5 lexikální chyba
                    return ERR_LEXICAL;
                }
                state = START_STATE;
            break;

            case DEF_STATE: //Q4
                if (c == '=') token->type = DEF; //přechod do F19 a konec v F19
                else {
                    strFree(&s);    //konec v Q4 lexikální chyba
                    return ERR_LEXICAL;
                }
                state = START_STATE;
            break;

            case ZERO_STATE: //F23
                if (c == '0') { // neplatný lexém 00
                    strFree(&s);
                    return ERR_LEXICAL;
                }
                else if (c == '.') { //přechod do Q15
                    c_prev = c;
                    strAddChar(&s, c);
                    state = FLOAT_STATE;
                }
                else if (c == 'e' || c == 'E') {// přechod do Q16
                    c_prev = c;
                    strAddChar(&s, c);
                    state = EXPONENT_STATE;
                }
                else newToken(INT_T, &s, c); // konec v F23
            break;

            case NUMBER_STATE: //F24
                if (isdigit(c)) { // zůstane v F24
                    strAddChar(&s, c);
                }
                else if (c == '.') { // přechod do Q15
                    c_prev = c;
                    strAddChar(&s, c);
                    state = FLOAT_STATE;
                }
                else if (c == 'e' || c == 'E') { // přechod do Q16
                    c_prev = c;
                    strAddChar(&s, c);
                    state = EXPONENT_STATE;
                }
                else newToken(INT_T, &s, c); // konec v F24
            break;

            case FLOAT_STATE: //Q15 a F25
                if (isdigit(c)) { // přechod do F25
                    c_prev = c;
                    strAddChar(&s, c);
                }
                else if (c == 'e' || c == 'E') {  
                    if (c_prev == '.') { //chyba zadání exponentu bez desetinné části
                        strFree(&s);
                        return ERR_LEXICAL;
                    }
                    else {  // přechod do Q16
                        c_prev = c;
                        strAddChar(&s, c);
                        state = EXPONENT_STATE;
                    }
                }
                else { 
                    if (c_prev == '.') { // lexikální chyba po tečce nenásledovalo číslo
                        strFree(&s);
                        return ERR_LEXICAL;
                    }
                    newToken(FLOAT_T, &s, c); // konec v F25
                }
            break;

            case EXPONENT_STATE: //Q16 a F26
                if (c == '+' || c == '-') { // přechod do Q17
                    c_prev = c;
                    strAddChar(&s, c);
                    state = EXPONENT_SIGN_STATE;
                }
                else if (isdigit(c)) { // přechod do F26
                    c_prev = c;
                    strAddChar(&s, c);
                }
                else {
                    if (c_prev == 'e' || c_prev == 'E') { // lexikální chyba po e nebo E nenásledovalo číslo ani + nebo -
                        strFree(&s);
                        return ERR_LEXICAL;
                    }
                    newToken(FLOAT_T, &s, c); //konec v F26
                }
            break;

            case EXPONENT_SIGN_STATE: //Q17 a F26
                if (isdigit(c)) { // přechod do F26
                    c_prev = c;
                    strAddChar(&s, c);
                }
                else {
                    if (c_prev == '+' || c_prev == '-') { // lexikální chyba po E+, e+, E- nebo e- nenásledovalo číslo
                        strFree(&s);
                        return ERR_LEXICAL;
                    }
                    newToken(FLOAT_T, &s, c); // konec v F26
                }
            break;

            case COMMENT_STATE: //F1
                if(c == '/'){   //přechod do Q1
                    state = ONE_LINE_C_STATE;
                }     
                else if (c == '*'){  // přechod do Q2
                    state = M_LINE_C_STATE;
                }
                else { // skončí v F1
                    token->type = DIV;
                    ungetc(c,sourceCode);
                    state = START_STATE;
                }
            break;

            case ONE_LINE_C_STATE: //Q1
                if (c == '\n') { // přechod do  F2
                    token->type = EOL_T;
                    state = START_STATE;
                }
                else if (c == EOF) { //přechod do F3
                    token->type = EOF_T;
                    state = START_STATE;
                }
            break;

            // Jsme v blokovém komentáři
            case M_LINE_C_STATE: //Q2
                if (c == '*') { // přechod do Q3
                    state = M_LINE_C_END_STATE;
                }
                else if (c == EOF) {
                    strFree(&s);
                    return ERR_LEXICAL;
                }
            break;

            case M_LINE_C_END_STATE: //Q3
                if (c == '/') { // přechod do S
                    state = START_STATE;
                    continue; // Komentář se přeskočí, SUCCESS se nevrátí
                }
                else if (c == EOF) {
                    strFree(&s);
                    return ERR_LEXICAL;
                }
                else { // zůstane v Q2
                    state = M_LINE_C_STATE;
                }
            break;

            case UNDERSCORE_STATE: //F27
                if (isalpha(c) || isdigit(c) || c == '_') { // přechod do F22
                    strAddChar(&s, c);
                    state = TEXT_STATE;
                }
                else { // konec v F27 vrací _
                    ungetc(c, sourceCode);
                    token->type = UNDERSCORE;
                    state = START_STATE;
                }
            break;

            case TEXT_STATE: // F22
                if (isalpha(c) || isdigit(c) || c == '_') { // zůstane v F22
                    c_prev = '\0';
                    strAddChar(&s, c);
                }

                else { //ověří zda zadaný text není klíčové slovo
                    if (strCmpConstStr(&s, "else") == 0 || strCmpConstStr(&s, "for") == 0 || strCmpConstStr(&s, "func") == 0 ||
                        strCmpConstStr(&s, "if") == 0 || strCmpConstStr(&s, "package") == 0 || strCmpConstStr(&s, "return") == 0) {
                        token->type = KEYWORD;
                        strCopyString(&(token->string), &s);
                    }
                    else if (strCmpConstStr(&s, "float64") == 0)    token->type = DATA_TYPE_FLOAT;
                    else if (strCmpConstStr(&s, "int") == 0)        token->type = DATA_TYPE_INT;
                    else if (strCmpConstStr(&s, "string") == 0)     token->type = DATA_TYPE_STRING;
                    else {
                        token->type = ID;
                        strCopyString(&(token->string), &s);
                    }
                    state = START_STATE;
                    ungetc(c, sourceCode);
                }
            break;

            case STRING_STATE: //Q6
                if (c == '"') { // přechod do F21
                    token->type = STRING_T;
                    strCopyString(&(token->string), &s);
                    state = START_STATE;
                }
                else if (c == '\\') { //přechod do Q7
                    state = ESCAPE_STATE;
                }
                // převod do formůtu požadovaného IFJcode20
                else if (c == '#') {
                    strAddChar(&s, '\\');
                    strAddChar(&s, '0');
                    strAddChar(&s, '3');
                    strAddChar(&s, '5');
                }
                else if (c == ' ') {
                    strAddChar(&s, '\\');
                    strAddChar(&s, '0');
                    strAddChar(&s, '3');
                    strAddChar(&s, '2');
                }
                else if (c == '\n' || c == EOF) { //neplánované ukončení řádku nebo celého souboru bez ukončení řetězce znaků
                    strFree(&s);
                    return ERR_LEXICAL;
                }
                else {
                    strAddChar(&s, c);
                }
            break;

            case ESCAPE_STATE: //Q7
                state = STRING_STATE; // příští stav bude Q6
                switch (c) {
                    case '"': strAddChar(&s, c); break; // přechod do Q12
                    case '\\': strAddChar(&s, '\\'); strAddChar(&s, '0'); strAddChar(&s, '9'); strAddChar(&s, '2'); break;  // přechod do Q11
                    case 'n': strAddChar(&s, '\\'); strAddChar(&s, '0'); strAddChar(&s, '1'); strAddChar(&s, '0'); break;   // přechod do Q14
                    case 't': strAddChar(&s, '\\'); strAddChar(&s, '0'); strAddChar(&s, '0'); strAddChar(&s, '9'); break;   // přechod do Q13
                    case 'x': state = HEX_ESCAPE_STATE; break; // přechod do Q8 
                    default:
                        strFree(&s);
                        return ERR_LEXICAL;
                }
            break;

            case HEX_ESCAPE_STATE: //Q8
                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) { // přechod do Q9
                    state = HEX_ESCAPE_STATE_2;
                    c_prev = c;
                }
                else {
                    strFree(&s);
                    return ERR_LEXICAL;
                }
            break;

            case HEX_ESCAPE_STATE_2: //Q9
                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) { //Q10
                    char hex[3] = { c_prev, c, '\0' };
                    int number = (int)strtol(hex, NULL, 16);
                    char c_new[4];
                    snprintf(c_new, 4, "%03d", number);
                    strAddChar(&s, '\\'); strAddChar(&s, c_new[0]); strAddChar(&s, c_new[1]); strAddChar(&s, c_new[2]);
                    state = STRING_STATE; // navrácení do Q6
                }
                else {
                    strFree(&s);
                    return ERR_LEXICAL;
                }
            break;
        }

        if (state == START_STATE) { 
            strFree(&s);
            return SUCCESS;
        }
    }
}

/**
 * @brief uloží token do struktury Token
 * 
 * @param type typ nečteného tokenu
 * @param s řetězec znaků s obsahem tokenu
 * @param c obsahuje hodnotu posledního znaku řetězce
 */
void newToken(tokenType type, string *s, int c) {
    token->type = type;
    if (type == INT_T) {
        token->intNumber = strtoll(strGetStr(s), NULL, 10);
    }
    else if (type == FLOAT_T) {
        token->floatNumber = atof(strGetStr(s));
    }
    else {
        strCopyString(&(token->string), s);
    }
    state = START_STATE;
    ungetc(c, sourceCode);
}