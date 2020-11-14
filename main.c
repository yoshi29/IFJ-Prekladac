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

