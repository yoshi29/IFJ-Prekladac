/**
 * Typ tokenu
 */
typedef enum {
    KEYWORD,
    DATA_TYPE,
    ID,
    EXPR,
    L_BRACKET,
    R_BRACKET,
    LC_BRACKET,
    RC_BRACKET,
    OPERATOR,
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