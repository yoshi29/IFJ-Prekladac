/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#pragma once
#include "stdio.h"
#include "stdlib.h"

typedef enum nodetype {
    /*0*/ INT,
    /*1*/ FLOAT,
    /*2*/ STRING,
    /*3*/ FUNC,
    /*4*/ BOOL,
    /*5*/ UNKNOWN
} nodeType;
