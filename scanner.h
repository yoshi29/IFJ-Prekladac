/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 * Autor: Tomáš Matušák, xmatus34
 */

#pragma once
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "stdint.h"

#include "error.h"
#include "scanner.h"
#include "str.h"

extern struct token *token;

typedef enum {
    START_STATE,
    EQ_T_STATE,
    NE_T_STATE,
    LE_T_STATE,
    GE_T_STATE,
    DEF_STATE,
    COMMENT_STATE,
    ONE_LINE_C_STATE,
    M_LINE_C_STATE,
    M_LINE_C_END_STATE,
    NUMBER_STATE,
    ZERO_STATE,
    FLOAT_STATE,
    EXPONENT_STATE,
    EXPONENT_SIGN_STATE,
    TEXT_STATE,
    STRING_STATE,
    ESCAPE_STATE,
    HEX_ESCAPE_STATE,
    HEX_ESCAPE_STATE_2
} states;

/**
 * Typ tokenu
 */
typedef enum {
    /*0*/ STRING_T,
    /*1*/ FLOAT_T,
    /*2*/ INT_T,
    /*3*/ KEYWORD,
    /*4*/ DATA_TYPE_INT,
    /*5*/ DATA_TYPE_FLOAT,
    /*6*/ DATA_TYPE_NIL,
    /*7*/ DATA_TYPE_STRING,
    /*8*/ ID,
    /*9*/ UNDERSCORE,
    /*10*/ L_BRACKET,
    /*11*/ R_BRACKET,
    /*12*/ LC_BRACKET,
    /*13*/ RC_BRACKET,
    /*14*/ EQ_T, // ==
    /*15*/ NE_T, // !=
    /*16*/ GT_T, // >
    /*17*/ LT_T, // <
    /*18*/ GE_T, // >=
    /*19*/ LE_T, // <=
    /*20*/ PLUS,
    /*21*/ MINUS,
    /*22*/ MUL,
    /*23*/ DIV,
    /*24*/ EQUALS,
    /*25*/ COMMA,
    /*26*/ SEMICOLON,
    /*27*/ ASSIGN,
    /*28*/ DEF,
    /*29*/ EOL_T,
    /*30*/ EOF_T,
} tokenType;

/**
 * Token
 */
typedef struct token {
    tokenType type;
    string string;
    int64_t intNumber;
    double floatNumber;
} Token;

void getSourceCode(FILE *code);

int getNextToken();

int isSpaceNext();

void newToken(tokenType type, string *s, int c);