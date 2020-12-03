#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

#include "scanner.h"

#define MAX 40 //TODO: Kolik by m�lo b�t skute�n�? +p�ejmenovat

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
 * Vygeneruje prom�nnou pro parametr funkce
 * @param retValPos Po�ad� parametru
 */
void generateParam(int paramPos);