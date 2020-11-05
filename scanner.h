/**
 * Typ tokenu
 */
typedef enum {
	STRING_T,
	FLOAT_T,
	INT_T,
    KEYWORD,
    DATA_TYPE,
    ID,
    UNDERSCORE,
    L_BRACKET,
    R_BRACKET,
    LC_BRACKET,
    RC_BRACKET,
	EQ_T, // ==
	NE_T, // !=
	GT_T, // >
	LT_T, // <
	GE_T, // >=
	LE_T, // <=
    PLUS,
    MINUS,
	MUL,
	DIV,
    EQUALS,
    COMMA,
    SEMICOLON,
    ASSIGN,
    DEF,
    EOL_T,
    EOF_T,
} tokenType;

/**
 * Token
 */
typedef struct token {
    tokenType type;
    char* value;
} Token;