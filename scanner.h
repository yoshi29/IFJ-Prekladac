/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 * Autor: Tomáš Matušák, xmatus34
 */

#pragma once
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "error.h"
#include "scanner.h"
#include "str.h"
#include "ctype.h"

#define START_STATE 1
#define EQ_T_STATE 2
#define NE_T_STATE 3
#define LE_T_STATE 4
#define GE_T_STATE 5
#define DEF_STATE 6
#define COMMENT_STATE 7
#define INT_STATE 8
#define FLOAT_STATE 9
#define SIGNED_FLOAT_STATE 10
#define FLOAT_EXPONENT_STATE 11
#define FLOAT_EXPONENT2_STATE 12
#define ONE_LINE_C_STATE 13
#define M_LINE_C_STATE 14

/**
 * Typ tokenu
 */
typedef enum {
	/*0*/ STRING_T,
    /*1*/ FLOAT_T,
    /*2*/ INT_T,
    /*3*/ KEYWORD,
    /*4*/ DATA_TYPE,
    /*5*/ ID,
    /*6*/ UNDERSCORE,
    /*7*/ L_BRACKET,
    /*8*/ R_BRACKET,
    /*9*/ LC_BRACKET,
    /*10*/ RC_BRACKET,
    /*11*/ EQ_T, // ==
    /*12*/ NE_T, // !=
    /*13*/ GT_T, // >
    /*14*/ LT_T, // <
    /*15*/ GE_T, // >=
    /*16*/ LE_T, // <=
    /*17*/ PLUS,
    /*18*/ MINUS,
    /*19*/ MUL,
    /*20*/ DIV,
    /*21*/ EQUALS,
    /*22*/ COMMA,
    /*23*/ SEMICOLON,
    /*24*/ ASSIGN,
    /*25*/ DEF,
    /*26*/ EOL_T,
    /*27*/ EOF_T,
} tokenType;

/**
 * Token
 */
typedef struct token {
    tokenType type;
    char* value;
    int intNumber;
    float floatNumber;
} Token;

void getSourceCode(FILE *code);

int getNextToken();

int isSpaceNext();

void newToken(tokenType type, string s, int c);