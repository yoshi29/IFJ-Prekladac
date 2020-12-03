#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

#include "scanner.h"
#include "symtable.h"

/*
 * Vrac� velikost pot�ebnou k alokaci �et�zce
 * @param num ��slo ur�en� k p�evodu na �et�zec
 */
int getStrSize(int num);

/*
 * Postupn� vypisuje ��sti instrukce a v�pis zakon�� znakem konce ��dku
 * @param num Po�et p�edan�ch parametr�
 */
void printCode(int num, ...);

/*
 * Vyp�e zadanou instrukci
 * @param instruction Instrukce
 */
void printInstr(char* instruction);

/*
 * Vygeneruje hlavi�ku
 */
void generateHeader();

/*
 * Vygeneruje za��tek funkce
 * @param funcName Jm�no funkce
 */
void generateFuncStart(char* funcName);

/*
 * Vygeneruje konec funkce
 */
void generateFuncEnd();

/*
 * Vygeneruje definice n�vratov�ch prom�nn�ch
 * @param retValPos Po�ad� n�vratov� hodnoty
 * @param type Typ tokenu (mo�no p�ijmout pouze DATA_TYPE_FLOAT/DATA_TYPE_INT/DATA_TYPE_STRING)
 */
void generateRetVal(int retValPos, tokenType type);

/*
 * Vygeneruje prom�nnou, do kter� se ulo�� p�edan� parametr funkce
 * @param retValPos Po�ad� parametru
 */
void generateVarFromParam(int paramPos);

/*
 * Vygeneruje prom�nnou s parametrem funkce v do�asn�m r�mci
 * @param retValPos Po�ad� parametru
 * @param token Token s parametrem
 */
void generateParamPass(int paramPos, Token* token);

/*
 * Vygeneruje vol�n� funkce
 * @param funcName Jm�no funkce
 */
void generateFuncCall(char* funcName);

/*
 * Generuje CREATEFRAME
 */
void generateBeforeParamPass();
