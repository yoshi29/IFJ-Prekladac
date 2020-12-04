/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

#include "scanner.h"
#include "symtable.h"
#include "psa.h"

/*
 * Vrací velikost potřebnou k alokaci řetězce
 * @param num Číslo určené k převodu na řetězec
 */
int getStrSize(int num);

/*
 * Postupně vypisuje části instrukce a výpis zakončí znakem konce řádku
 * @param num Počet předaných parametrů
 */
void printCode(int num, ...);

/*
 * Vypíše zadanou instrukci
 * @param instruction Instrukce
 */
void printInstr(char* instruction);

/*
 * Vygeneruje hlavičku
 */
void generateHeader();

/*
 * Vygeneruje začátek funkce
 * @param funcName Jméno funkce
 */
void generateFuncStart(char* funcName);

/*
 * Vygeneruje konec funkce
 * @param funcName Jméno funkce
 */
void generateFuncEnd(char* funcName);

/*
 * Vygeneruje definice návratových proměnných
 * @param retValPos Pořadí návratové hodnoty
 * @param type Typ tokenu (možno přijmout pouze DATA_TYPE_FLOAT/DATA_TYPE_INT/DATA_TYPE_STRING)
 */
void generateRetValDef(int retValPos, tokenType type);

/*
 * Vygeneruje přiřazení hodnot do návratových proměnných
 * @param retValPos Pořadí návratové hodnoty
 * @param valueSuffix
 */
void generateRetVal(int retValPos, int valueSuffix);

/*
 * Vygeneruje proměnnou, do které se uloží předaný parametr funkce
 * @param retValPos Pořadí parametru
 */
void generateVarFromParam(int paramPos);

/*
 * Vygeneruje proměnnou s parametrem funkce v dočasném rámci
 * @param retValPos Pořadí parametru
 * @param token Token s parametrem
 */
void generateParamPass(int paramPos, Token* token);

/*
 * Vygeneruje volání funkce
 * @param funcName Jméno funkce
 */
void generateFuncCall(char* funcName);

/*
 * Vyeneruje CREATEFRAME
 */
void generateBeforeParamPass();

/*
 * Vygeneruje definici proměnné
 * @param name Název proměnné
 * @param suffix Číslo aktuálního rámce, budoucí suffix proměnné
 * @param valueSuffix
 */
void generateVariable(char* name, int suffix, int valueSuffix);

void generateValAssignment(char* name, int pos, int valueSuffix);

/*
 * Vygeneruje návěští
 * @param name Název návěští
 * @param suffix Unikátní číslo pro návěští
 */
void generateLabel(char* name, int suffix);

/*
 * Vygeneruje podmíněný skok pro for
 * @param suffix Unikátní číslo pro for
 * @param valueSuffix Suffix proměnné, kde se nachází výsledek výrazu
 */
void generateForJump(int suffix, int valueSuffix);

/*
 * Vygeneruje podmíněný skok pro if
 * @param suffix Unikátní číslo pro if
 * @param valueSuffix Suffix proměnné, kde se nachází výsledek výrazu
 */
void generateIfJump(int suffix, int valueSuffix);

/*
 * Vygeneruje skok
 * @param name Název návěští, kam se má skočit
 * @param suffix Suffix návěští
 */
void generateJump(char* name, int suffix);

void generateForVar(char* name, int scope, bool start);

void generateForAllVars(TNode* root, int scope, bool start);

void generateForParam(int scope, bool start);

void generateForAllParams(TNode* root, bool start);

/*
 * Vygeneruje nový rámec pro for (jedná-li se o začátek foru) a nebo ho odstraní (jedná-li se o konec)
 * @param start true, pokud se jedná o začátek foru, jinak false
 */
void generateForFrame(bool start);
