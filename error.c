/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Filip Vágner, xvagne08
 */

#include "error.h"

void print_err(int err)
{
	switch (err)
	{
		case ERR_LEXICAL:
			fprintf(stderr, "%s\n", MSG_LEXICAL);
			break;
		case ERR_SYNTAX:
			fprintf(stderr, "%s\n", MSG_SYNTAX);
			break;
		case ERR_SEM_DEF:
			fprintf(stderr, "%s\n", MSG_SEM_DEF);
			break;
		case ERR_SEM_TYPE:
			fprintf(stderr, "%s\n", MSG_SEM_TYPE);
			break;
		case ERR_SEM_COMP:
			fprintf(stderr, "%s\n", MSG_SEM_COMP);
			break;
		case ERR_SEM_FUNC:
			fprintf(stderr, "%s\n", MSG_SEM_FUNC);
			break;
		case ERR_SEM_OTHER:
			fprintf(stderr, "%s\n", MSG_SEM_OTHER);
			break;
		case ERR_DIVBYZERO:
			fprintf(stderr, "%s\n", MSG_DIVBYZERO);
			break;
		case ERR_COMPILER:
			fprintf(stderr, "%s\n", MSG_COMPILER);
			break;
	}
}