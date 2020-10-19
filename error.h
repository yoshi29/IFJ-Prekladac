/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#pragma once
#include <stdio.h>
#include <stdlib.h>

#ifndef ERROR_H
#define ERROR_H

#define SUCCESS 0

#define ERR_LEXICAL 1
#define MSG_LEXICAL "Lexical Analysis Error: incorrect structure of lexem"

#define ERR_SYNTAX 2
#define MSG_SYNTAX "Syntax Analysis Error: incorrect syntax of the source program / unexpected line breaks"

#define ERR_SEM_DEF 3
#define MSG_SEM_DEF "Semantic Error: undefined function or variable / trying to redefine function or variable"

#define ERR_SEM_TYPE 4
#define MSG_SEM_TYPE "Semantic Error: incorrect data type derivation of newly defined variable"

#define ERR_SEM_COMP 5
#define MSG_SEM_COMP "Semantic Error: incorrect type compatibility in expression"

#define ERR_SEM_FUNC 6
#define MSG_SEM_FUNC "Semantic Error: incorrect number/type of parameters or return values of the function"

#define ERR_SEM_OTHER 7
#define MSG_SEM_OTHER "Semantic Error"

#define ERR_DIVBYZERO 9
#define MSG_DIVBYZERO "Error: division by zero"

#define ERR_COMPILER 99
#define MSG_COMPILER "Compiler Error: internal compiler error"

/**
 * @brief Prints an error message based on the error code.
 * @param err Error code
 */
void print_err(int err);

#endif