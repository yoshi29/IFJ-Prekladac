/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "main.h"

int main() {
    FILE* file = stdin;

    int retVal = parse(file);
    printf(">> PARSING: %i\n", retVal);
    print_err(retVal); //Zatím pro test
    return retVal;
}

