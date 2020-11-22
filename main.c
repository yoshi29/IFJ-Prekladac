/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "main.h"

int main() {
    FILE* file = stdin;

    // Testování scanneru
    // ./ifj20 < source.go
    /*
    getSourceCode(file);
    token = malloc(sizeof(struct token));
    strInit(&(token->string));

    char *str_token[] = { "STRING_T", "FLOAT_T", "INT_T","KEYWORD","DATA_TYPE_INT","DATA_TYPE_FLOAT","DATA_TYPE_NIL","DATA_TYPE_STRING","ID","UNDERSCORE",
        "L_BRACKET","R_BRACKET","LC_BRACKET","RC_BRACKET","EQ_T","NE_T","GT_T","LT_T","GE_T","LE_T","PLUS","MINUS","MUL","DIV","EQUALS","COMMA",
        "SEMICOLON","ASSIGN","DEF","EOL_T" };

    int retval = getNextToken();
    printf("Type [Value]\n=============\n");
    while (token->type != EOF_T && retval == SUCCESS) {
        printf("%s", str_token[token->type]);
        if (token->type == STRING_T || token->type == KEYWORD || token->type == ID)
            printf(" [%s]", token->string.str);
        else if (token->type == INT_T)
            printf(" [%ld]", token->intNumber);
        else if (token->type == FLOAT_T)
            printf(" [%g]", token->floatNumber);
        printf("\n");
        retval = getNextToken();
    }
    printf("- Návratový kód: %d\n", retval);
    */
    
    int retVal = parse(file);
    printf(">> PARSING: %i\n", retVal);
    print_err(retVal); //Zatím pro test
    

    /********************** JUST TESTS - CAN BE DELETED ****************************/
    /*
    TTree* tree = (TTree*)malloc(sizeof(struct tTree));
    TSInit(tree);

    TSInsert(&(tree->root), "1", INT, false, 2, NULL); 

    if (tree->root == NULL) {
        printf(">> ERR: MAIN ROOT NULL\n");
    }
    else {
        printf("NODE: %s\n", tree->root->key);
    }

    TSInsert(&(tree->root), "2", INT, false, 2, NULL); 
    TSInsert(&(tree->root), "3", INT, false, 2, NULL); 
    TSInsert(&(tree->root), "4", INT, false, 2, NULL); 


    if (tree->root->lptr == NULL && tree->root->rptr == NULL) {
        printf(">> ERR: MAIN BOTH NODES NULL\n");
    }
    else if (tree->root->rptr == NULL) {
        printf("LEFT NODE: %s\n", tree->root->lptr->key);
    }
    else if (tree->root->lptr == NULL) {
        printf("RIGHT NODE: %s\n", tree->root->rptr->key);
    }

    TNode* searched = TSSearch(tree->root, "7");
    if (searched != NULL) {
        printf("FOUND\n");
    }

    TSDispose(tree->root);
    free(tree);
    tree->root = NULL;
    tree->last = NULL;
    TSSearch(tree->root, "7");
    */
    /********************** JUST TESTS - CAN BE DELETED ****************************/
}

