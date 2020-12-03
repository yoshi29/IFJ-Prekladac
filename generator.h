#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

#include "scanner.h"
#include "symtable.h"

/*
 * Vrací velikost potøebnou k alokaci øetìzce
 * @param num Èíslo urèené k pøevodu na øetìzec
 */
int getStrSize(int num);

/*
 * Postupnì vypisuje èásti instrukce a výpis zakonèí znakem konce øádku
 * @param num Poèet pøedaných parametrù
 */
void printCode(int num, ...);

/*
 * Vypíše zadanou instrukci
 * @param instruction Instrukce
 */
void printInstr(char* instruction);

/*
 * Vygeneruje hlavièku
 */
void generateHeader();

/*
 * Vygeneruje zaèátek funkce
 * @param funcName Jméno funkce
 */
void generateFuncStart(char* funcName);

/*
 * Vygeneruje konec funkce
 */
void generateFuncEnd();

/*
 * Vygeneruje definice návratových promìnných
 * @param retValPos Poøadí návratové hodnoty
 * @param type Typ tokenu (možno pøijmout pouze DATA_TYPE_FLOAT/DATA_TYPE_INT/DATA_TYPE_STRING)
 */
void generateRetVal(int retValPos, tokenType type);

/*
 * Vygeneruje promìnnou, do které se uloží pøedaný parametr funkce
 * @param retValPos Poøadí parametru
 */
void generateVarFromParam(int paramPos);

/*
 * Vygeneruje promìnnou s parametrem funkce v doèasném rámci
 * @param retValPos Poøadí parametru
 * @param token Token s parametrem
 */
void generateParamPass(int paramPos, Token* token);

/*
 * Vygeneruje volání funkce
 * @param funcName Jméno funkce
 */
void generateFuncCall(char* funcName);

/*
 * Generuje CREATEFRAME
 */
void generateBeforeParamPass();
