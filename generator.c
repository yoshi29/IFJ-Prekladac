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
		printCode(2, "POPFRAME", " # ---------- End of function");
		printInstr("RETURN");
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

	printCode(5, "MOVE LF@%param", pos, " ", "LF@*E", valueSuffixStr);
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
		char scopeStr[getStrSize(scope)];
		sprintf(scopeStr, "%i", scope);
		printCode(6, "MOVE TF@%", pos, " ", "LF@", idNode->key, scopeStr);
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
	char suffixStr[getStrSize(suffix)];
	sprintf(suffixStr, "%i", suffix);
	char valueSuffixStr[getStrSize(valueSuffix)];
	sprintf(valueSuffixStr, "%i", valueSuffix);

	printCode(6, "MOVE LF@", name, suffixStr, " ", "LF@*E", valueSuffixStr);
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
