/**
 * Projekt: Implementace překladače imperativního jazyka IFJ20
 * Autor: Jana Stopková, xstopk01
 */

#include "generator.h"

int getStrSize(int num) {
	int size = 2;
	while (num > 9) {
		size++;
		num /= 10;
	}
	return size;
}

void printCode(int num, ...) {
	va_list args;
	va_start(args, num);
	for (int i = 0; i < num; i++) {
		fprintf(stdout, "%s", va_arg(args, char*));
	}
	va_end(args);
	fprintf(stdout, "\n");
}

void printInstr(char* instruction) {
	fprintf(stdout, "%s\n", instruction);
}

void generateHeader() {
	printInstr(".IFJcode20");
	printInstr("JUMP $$main");
}

void generateExit() {
	printInstr("LABEL $$main-exit");
}

void generateFuncStart(char* funcName) {
	if (strcmp(funcName, "main") == 0) {
		printCode(4, "LABEL $$", funcName, " # ---------- Start of function ", funcName);
		printInstr("CREATEFRAME");
	}
	else 
		printCode(4, "LABEL $", funcName, " # ---------- Start of function ", funcName);

	printInstr("PUSHFRAME");
}

void generateFuncEnd(char* funcName) {
	if (strcmp(funcName, "main") != 0) {
		printCode(3, "POPFRAME", " # ---------- End of function ", funcName);
		printInstr("RETURN");
	}
	else {
		printInstr("JUMP $$main-exit");
	}
}

void generateRetValDef(int retValPos, tokenType type) {
	char pos[getStrSize(retValPos)];
	sprintf(pos, "%i", retValPos);

	printCode(2, "DEFVAR LF@%retval", pos);

	switch (type) {
		case DATA_TYPE_FLOAT:
			printCode(4, "MOVE LF@%retval", pos, " ", "float@0x0p+0");
			break;
		case DATA_TYPE_INT:
			printCode(4, "MOVE LF@%retval", pos, " ", "int@0");
			break;
		case DATA_TYPE_STRING:
			printCode(4, "MOVE LF@%retval", pos, " ", "string@");
			break;
		default: //Načten nesprávný token
			print_err(ERR_SYNTAX);
			exit(ERR_SYNTAX);
			break;
	}
}

void generateRetVal(int retValPos, int valueSuffix) {
	char pos[getStrSize(retValPos)];
	sprintf(pos, "%i", retValPos);
	char valueSuffixStr[getStrSize(valueSuffix)];
	sprintf(valueSuffixStr, "%i", valueSuffix);

	printCode(5, "MOVE LF@%retval", pos, " ", "LF@*E", valueSuffixStr);
}

void generateRetValAssignment(char* name, int suffix, int retValPos) {
	char retValPosStr[getStrSize(retValPos)];
	sprintf(retValPosStr, "%i", retValPos);

	if (suffix == 0) { // Jedná se o formální parametr funkce
		char suffixStr[getStrSize(suffix + 1)];
		sprintf(suffixStr, "%i", suffix + 1);

		printCode(5, "MOVE LF@%param", suffixStr, " ", "TF@%retval", retValPosStr);
	}
	else { // Jedná se o lokální proměnnou
		char suffixStr[getStrSize(suffix)];
		sprintf(suffixStr, "%i", suffix);

		printCode(6, "MOVE LF@", name, suffixStr, " ", "TF@%retval", retValPosStr);
	}
}

void generateVarFromParam(int paramPos) {
	char pos[getStrSize(paramPos)];
	sprintf(pos, "%i", paramPos);

	printCode(2, "DEFVAR LF@%param", pos);
	printCode(5, "MOVE LF@%param", pos, " ", "LF@%", pos);
}

void generateBeforeParamPass() {
	printCode(2, "CREATEFRAME", " # ---------- Passing params to function");
}

void generateParamPass(int paramPos, Token* token) {
	char pos[getStrSize(paramPos)];
	sprintf(pos, "%i", paramPos);

	printCode(2, "DEFVAR TF@%", pos);
	if (token->type == FLOAT_T) {
		char floatStr[30];
		sprintf(floatStr, "%a", token->floatNumber);
		printCode(5, "MOVE TF@%", pos, " ", "float@", floatStr);
	}
	else if (token->type == INT_T) {
		char intStr[getStrSize(token->intNumber)];
		sprintf(intStr, "%li", token->intNumber);
		printCode(5, "MOVE TF@%", pos, " ", "int@", intStr);
	}
	else if (token->type == STRING_T) {
		printCode(5, "MOVE TF@%", pos, " ", "string@", token->string.str);
	}
	else if (token->type == ID) {
		int scope;
		TNode* idNode = TSSearchStackAndReturn(stack.top, token->string.str, &scope);

		if (scope == 0) { // Jedná se o formální parametr funkce
			char suffixStr[getStrSize(paramPos)];
			sprintf(suffixStr, "%i", paramPos);
			printCode(5, "MOVE TF@%", pos, " ", "LF@%param", suffixStr);
		}
		else { // Jedná se o lokální proměnnou
			char scopeStr[getStrSize(scope)];
			sprintf(scopeStr, "%i", scope);
			printCode(6, "MOVE TF@%", pos, " ", "LF@", idNode->key, scopeStr);
		}
	}
	else { //Načten nesprávný token
		print_err(ERR_SYNTAX);
		exit(ERR_SYNTAX);
	}
}

void generateFuncCall(char* funcName) {
	if (strcmp(funcName, "main") == 0)
		printCode(4, "CALL $$", funcName, " # ---------- Calling function ", funcName);
	else 
		printCode(4, "CALL $", funcName, " # ---------- Calling function ", funcName);
}

void generateVariable(char* name, int suffix, int valueSuffix) {
	char suffixStr[getStrSize(suffix)];
	sprintf(suffixStr, "%i", suffix);
	char valueSuffixStr[getStrSize(valueSuffix)];
	sprintf(valueSuffixStr, "%i", valueSuffix);

	printCode(3, "DEFVAR LF@", name, suffixStr);
	printCode(6, "MOVE LF@", name, suffixStr, " ", "LF@*E", valueSuffixStr);
}

void generateValAssignment(char* name, int suffix, int valueSuffix) {
	
	char valueSuffixStr[getStrSize(valueSuffix)];
	sprintf(valueSuffixStr, "%i", valueSuffix);

	if (suffix == 0) { // Jedná se o parametr funkce
		char suffixStr[getStrSize(suffix + 1)];
		sprintf(suffixStr, "%i", suffix + 1);
		
		printCode(5, "MOVE LF@%param", suffixStr, " ", "LF@*E", valueSuffixStr);
	}
	else { // Jedná se o lokální proměnnou
		char suffixStr[getStrSize(suffix)];
		sprintf(suffixStr, "%i", suffix);

		printCode(6, "MOVE LF@", name, suffixStr, " ", "LF@*E", valueSuffixStr);
	}
}

void generateLabel(char* name, int suffix) {
	char suffixStr[getStrSize(suffix)];
	sprintf(suffixStr, "%i", suffix);

	printCode(4, "LABEL", " ", name, suffixStr);
}

void generateForJump(int suffix, int valueSuffix) {
	char suffixStr[getStrSize(suffix)];
	sprintf(suffixStr, "%i", suffix);
	char valueSuffixStr[getStrSize(valueSuffix)];
	sprintf(valueSuffixStr, "%i", valueSuffix);

	printCode(9, "JUMPIFEQ", " ", "for-end", suffixStr, " ", "LF@*E", valueSuffixStr, " ", "bool@false");
}

void generateIfJump(int suffix, int valueSuffix) {
	char suffixStr[getStrSize(suffix)];
	sprintf(suffixStr, "%i", suffix);
	char valueSuffixStr[getStrSize(valueSuffix)];
	sprintf(valueSuffixStr, "%i", valueSuffix);

	printCode(9, "JUMPIFEQ", " ", "else", suffixStr, " ", "LF@*E", valueSuffixStr, " ", "bool@false");
}

void generateJump(char* name, int suffix) {
	char suffixStr[getStrSize(suffix)];
	sprintf(suffixStr, "%i", suffix);

	printCode(4, "JUMP", " ", name, suffixStr);
}

void generateForVar(char* name, int scope, bool start) {
	char suffixStr[getStrSize(scope)];
	sprintf(suffixStr, "%i", scope);
	if (start) {
		printCode(3, "DEFVAR TF@", name, suffixStr);
		printCode(7, "MOVE TF@", name, suffixStr, " ", "LF@", name, suffixStr);
	}
	else {
		printCode(7, "MOVE LF@", name, suffixStr, " ", "TF@", name, suffixStr);
	}
}

void generateForAllVars(TNode* root, int scope, bool start) {
	if (root != NULL) {
		if (root->type != FUNC)
			generateForVar(root->key, scope, start);
		generateForAllVars(root->lptr, scope, start);
		generateForAllVars(root->rptr, scope, start);
	}
}

void generateForParam(int scope, bool start) {
	char suffixStr[getStrSize(scope)];
	sprintf(suffixStr, "%i", scope);
	if (start) {
		printCode(2, "DEFVAR TF@%param", suffixStr);
		printCode(5, "MOVE TF@%param", suffixStr, " ", "LF@%param", suffixStr);
	}
	else {
		printCode(5, "MOVE LF@%param", suffixStr, " ", "TF@%param", suffixStr);
	}
}

void generateForAllParams(TNode* root, bool start) {
	if (root != NULL) {
		if (root->type != FUNC)
			generateForParam(root->param + 1, start);
		generateForAllParams(root->lptr, start);
		generateForAllParams(root->rptr, start);
	}
}

void generateForFrame(bool start) {
	if (start)
		printInstr("CREATEFRAME");
	else
		printInstr("POPFRAME");

	TStack_Elem* current_frame = stack.top;
	while (current_frame != NULL) {
		generateForAllVars(current_frame->node, current_frame->scope, start);
		current_frame = current_frame->next;
	}
	generateForAllParams(currentFuncNode->localTS, start);

	if (start)
		printInstr("PUSHFRAME");
}

/* ------------------------------------------------------------------------------- */

void generateBuiltInFunctions() {
	generateInt2Float();
	generateFloat2Int();
	generateLen();
	generateOrd();
	generateChr();
	generateSubstr();
	generateInputs();
	generateInputi();
	generateInputf();
}

void generateInt2Float() {
	generateFuncStart("int2float");
	printInstr(
		"DEFVAR LF@%retval1\n"
		"INT2FLOAT LF@%retval1 LF@%1"
	);
	generateFuncEnd("int2float");
}

void generateFloat2Int() {
	generateFuncStart("float2int");
	printInstr(
		"DEFVAR LF@%retval1\n"
		"FLOAT2INT LF@%retval1 LF@%1"
	);
	generateFuncEnd("float2int");
}

void generateLen() {
	generateFuncStart("len");
	printInstr(
		"DEFVAR LF@%retval1\n"
		"STRLEN LF@%retval1 LF@%1"
	);
	generateFuncEnd("len");
}

void generateOrd() {
	generateFuncStart("ord");
	printInstr( //LF@%1 - s string, LF@%2 - i int
		"DEFVAR LF@%retval1\n"
		"MOVE LF@%retval1 string@\n"

		"DEFVAR LF@%retval2\n"
		"MOVE LF@%retval2 int@0\n"

		"CREATEFRAME\n"
		"DEFVAR TF@%1\n"
		"MOVE TF@%1 LF@%1\n"
		"CALL $len\n"
		"DEFVAR LF@%string_length\n"
		"MOVE LF@%string_length TF@%retval1\n" //string_length = délka řetězce s

		"SUB LF@%string_length LF@%string_length int@1\n" //-1 => Korekce počítání od 0 kvůli i

		"DEFVAR LF@%err\n"
		"LT LF@%err LF@%2 int@0\n"
		"JUMPIFEQ $ord$err LF@%err bool@true\n" //je-li i < 0 => chyba
		"GT LF@%err LF@%2 LF@%string_length\n"
		"JUMPIFNEQ $ord$not_err LF@%err bool@true\n" //je-li i > string_length-1 => chyba
		"LABEL $ord$err\n"
		"MOVE LF@%retval2 int@1\n" 
		"JUMP $ord$end\n"

		"LABEL $ord$not_err\n" //nenastala chyba
		"STRI2INT LF@%retval1 LF@%1 LF@%2\n"

		"LABEL $ord$end"
	);
	generateFuncEnd("ord");
}

void generateChr() {
	generateFuncStart("chr");
	printInstr( //LF@%1 - i int
		"DEFVAR LF@%retval1\n"
		"MOVE LF@%retval1 string@\n"

		"DEFVAR LF@%retval2\n"
		"MOVE LF@%retval2 int@0\n"

		"DEFVAR LF@%err\n"
		"LT LF@%err LF@%1 int@0\n"
		"JUMPIFEQ $chr$err LF@%err bool@true\n"  //je-li i < 0 => chyba
		"GT LF@%err LF@%1 int@255\n"
		"JUMPIFEQ $chr$err LF@%err bool@true\n"  //je-li i > 255 => chyba

		"INT2CHAR LF@%retval1 LF@%1\n"
		"JUMP $chr$end\n"

		"LABEL $chr$err\n"
		"MOVE LF@%retval2 int@1\n"

		"LABEL $chr$end"
	);
	generateFuncEnd("chr");
}

void generateSubstr() {
	generateFuncStart("substr");
	printInstr( //LF@%1 - s string, LF@%2 - i int, LF@%2 - n int
		"DEFVAR LF@%retval1\n"
		"MOVE LF@%retval1 string@\n"

		"DEFVAR LF@%retval2\n"
		"MOVE LF@%retval2 int@0\n"

		"CREATEFRAME\n" 
		"DEFVAR TF@%1\n"
		"MOVE TF@%1 LF@%1\n"
		"CALL $len\n"
		"DEFVAR LF@%string_length\n"
		"MOVE LF@%string_length TF@%retval1\n" //string_length = délka řetězce s

		"SUB LF@%string_length LF@%string_length int@1\n" //-1 => Korekce počítání od 0 kvůli i
		
		"DEFVAR LF@%err\n"
		"LT LF@%err LF@%2 int@0\n"
		"JUMPIFEQ $substr$err LF@%err bool@true\n"  //je-li i < 0 => chyba
		"LT LF@%err LF@%3 int@0\n" 
		"JUMPIFEQ $substr$err LF@%err bool@true\n" //je-li n < 0 => chyba
		"GT LF@%err LF@%2 LF@%string_length\n" 
		"JUMPIFEQ $substr$err LF@%err bool@true\n" //je-li i > len(s)-1 => chyba (i > maximální povolený index)

		"DEFVAR LF@%set_n_max\n" 
		"DEFVAR LF@%diff\n" 
		"SUB LF@%diff LF@%string_length LF@%2\n" //diff = len(s)-i
		"ADD LF@%diff LF@%diff int@1\n" //Zpětná korekce +1
		"GT LF@%set_n_max LF@%3 LF@%diff\n"
		"JUMPIFNEQ $substr$not_max LF@%set_n_max bool@true\n" //není-li n > len(s)-i => NEnastavit n na max
		"MOVE LF@%3 LF@%diff\n" //n = diff

		"LABEL $substr$not_max\n"

		"DEFVAR LF@%tmp\n" //Pomocná proměnná pro getchar
		"MOVE LF@%tmp string@\n"
		"DEFVAR LF@%end\n"

		"ADD LF@%3 LF@%3 LF@%2\n" 
		"SUB LF@%3 LF@%3 int@1\n" //n = n+i-1 ... index posledního znaku vraceného řetězce

		"GT LF@%end LF@%2 LF@%3\n" //je-li i > n => konec
		"JUMPIFEQ $substr$end LF@%end bool@true\n" //n = 0

		"LABEL $substr$loop_start\n"
		"GETCHAR LF@%tmp LF@%1 LF@%2\n"
		"CONCAT LF@%retval1 LF@%retval1 LF@%tmp\n"
		"ADD LF@%2 LF@%2 int@1\n" //i++
		"GT LF@%end LF@%2 LF@%3\n" //je-li i > n => konec
		"JUMPIFNEQ $substr$loop_start LF@%end bool@true\n"
		"JUMP $substr$end\n"

		"LABEL $substr$err\n" //chyba
		"MOVE LF@%retval2 int@1\n"

		"LABEL $substr$end"
	);	
	generateFuncEnd("substr");
}

void generateInputs() {
	generateFuncStart("inputs");
	printInstr(
		"DEFVAR LF@%retval1\n"
		"DEFVAR LF@%retval2\n"
		"MOVE LF@%retval2 int@0\n"
		"DEFVAR LF@%type\n"
		"DEFVAR LF@%err\n"

		"READ LF@%retval1 string\n"
		"TYPE LF@%type LF@%retval1\n" //zjistí typ načtené hodnoty
		"EQ LF@%err LF@%type string@nil\n" 
		"JUMPIFNEQ $inputs$ok LF@%err bool@true\n"
		"MOVE LF@%retval2 int@1\n" //pokud je type nil => chyba

		"LABEL $inputs$ok"
	);
	generateFuncEnd("inputs");
}

void generateInputi() {
	generateFuncStart("inputi");
	printInstr(
		"DEFVAR LF@%retval1\n"
		"DEFVAR LF@%retval2\n"
		"MOVE LF@%retval2 int@0\n"
		"DEFVAR LF@%type\n"
		"DEFVAR LF@%err\n"

		"READ LF@%retval1 int\n"
		"TYPE LF@%type LF@%retval1\n" //zjistí typ načtené hodnoty
		"EQ LF@%err LF@%type string@nil\n"
		"JUMPIFNEQ $inputi$ok LF@%err bool@true\n"
		"MOVE LF@%retval2 int@1\n" //pokud je type nil => chyba

		"LABEL $inputi$ok"
	);
	generateFuncEnd("inputi");
}

void generateInputf() {
	generateFuncStart("inputf");
	printInstr(
		"DEFVAR LF@%retval1\n"
		"DEFVAR LF@%retval2\n"
		"MOVE LF@%retval2 int@0\n"
		"DEFVAR LF@%type\n"
		"DEFVAR LF@%err\n"

		"READ LF@%retval1 float\n"
		"TYPE LF@%type LF@%retval1\n" //zjistí typ načtené hodnoty
		"EQ LF@%err LF@%type string@nil\n"
		"JUMPIFNEQ $inputf$ok LF@%err bool@true\n"
		"MOVE LF@%retval2 int@1\n" //pokud je type nil => chyba

		"LABEL $inputf$ok"
	);
	generateFuncEnd("inputf");
}

void generatePrintCall(int param) {

	for (int i = 1; i <= param; i++) {
		char suffixStr[getStrSize(i)];
		sprintf(suffixStr, "%i", i);
		printCode(4, "WRITE", " ", "TF@%", suffixStr);
	}
}
