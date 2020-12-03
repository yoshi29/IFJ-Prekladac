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

void generateFuncStart(char* funcName) {
	printCode(4, "LABEL $", funcName, " # ---------- Start of function ", funcName);
	printInstr("PUSHFRAME");
}

void generateFuncEnd() {
	printCode(2, "POPFRAME", " # ---------- End of function");
	printInstr("RETURN");
}

void generateRetVal(int retValPos, tokenType type) {
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
		default:
			//TODO: Jaká chyba?
			break;
	}
}

void generateVarFromParam(int paramPos) {
	char pos[getStrSize(paramPos)];
	sprintf(pos, "%i", paramPos);

	printCode(2, "DEFVAR LF@%param", pos);
	printCode(5, "MOVE LF@%param", pos, " ", "LF@%", pos);
}

void generateParamPass(int paramPos, Token* token) {
	char pos[getStrSize(paramPos)];
	sprintf(pos, "%i", paramPos);

	printCode(2, "DEFVAR TF@%", pos);
	if (token->type == FLOAT_T) {
		char floatStr[getStrSize(token->floatNumber)];
		sprintf(floatStr, "%a", token->floatNumber);
		printCode(5, "MOVE TF@%", pos, " ", "float@", floatStr); //TODO: Vyøešit konverze na char*
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
		char scopeStr[getStrSize(scope)];
		sprintf(scopeStr, "%i", scope);
		printCode(6, "MOVE TF@%", pos, " ", "LF@%", idNode->key, scopeStr);
	}
	else {
		//TODO: Jaká chyba?
	}
}

void generateFuncCall(char* funcName) {
	if (strcmp(funcName, "main") == 0)
		printCode(4, "CALL $$", funcName, " # ---------- Calling function ", funcName);

	printCode(4, "CALL $", funcName, " # ---------- Calling function ", funcName);
}

void generateBeforeParamPass() {
	printCode(2, "CREATEFRAME", " # ---------- Passing params to function");
}