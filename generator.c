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
	printCode(2, "LABEL $", funcName);
	printInstr("PUSHFRAME");
}

void generateFuncEnd() {
	printInstr("POPFRAME");
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

void generateParam(int paramPos) {
	char pos[getStrSize(paramPos)];
	sprintf(pos, "%i", paramPos);

	printCode(2, "DEFVAR LF@%param", pos);
	printCode(5, "MOVE LF@%param", pos, " ", "LF@%", pos);
}