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
    UNDERSCORE_STATE,
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
    /*6*/ DATA_TYPE_STRING,
    /*7*/ ID,
    /*8*/ UNDERSCORE,
    /*9*/ L_BRACKET,
    /*10*/ R_BRACKET,
    /*11*/ LC_BRACKET,
    /*12*/ RC_BRACKET,
    /*13*/ EQ_T, // ==
    /*14*/ NE_T, // !=
    /*15*/ GT_T, // >
    /*16*/ LT_T, // <
    /*17*/ GE_T, // >=
    /*18*/ LE_T, // <=
    /*19*/ PLUS,
    /*20*/ MINUS,
    /*21*/ MUL,
    /*22*/ DIV,
    /*23*/ EQUALS,
    /*24*/ COMMA,
    /*25*/ SEMICOLON,
    /*26*/ DEF,
    /*27*/ EOL_T,
    /*28*/ EOF_T,
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

void newToken(tokenType type, string *s, int c);