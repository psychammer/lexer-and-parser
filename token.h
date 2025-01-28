#ifndef token_h
#define token_h

typedef struct token_struct{
    enum {
        TOKEN_ID,

        TOKEN_EQUALS,

        TOKEN_STRING,
        TOKEN_SYMBOL,
        TOKEN_SEMI,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_RBRACE,
        TOKEN_LBRACE,
        TOKEN_LBRACKET,
        TOKEN_RBRACKET,
        TOKEN_COMMA,
        TOKEN_COLON,
        TOKEN_TILDE,
        TOKEN_QUESTION,
        TOKEN_EXCLAMATION,
        TOKEN_AMPERSAND,
        TOKEN_DOT,
        TOKEN_EOF,
        TOKEN_OPERATOR,

        TOKEN_KEYWORD,
        TOKEN_IF,
        TOKEN_ELSE,
        TOKEN_WHILE,
        TOKEN_DO,
        TOKEN_RETURN,
        TOKEN_FOR,
        TOKEN_CASE,
        TOKEN_BREAK,
        TOKEN_TRY,
        TOKEN_CATCH,


        TOKEN_RESERVEDWORDS,

        TOKEN_DATATYPE,
        TOKEN_INT,
        TOKEN_FLOAT,
        TOKEN_DOUBLE,
        TOKEN_CHAR,
        TOKEN_VOID,
        TOKEN_BOOL,

        TOKEN_SINGLECOMMENT,
        TOKEN_MULTICOMMENT,

        TOKEN_NUMBER,

        TOKEN_UNKNOWN,

        TOKEN_CHECK,
        TOKEN_ELIF,
        TOKEN_ENDLOOP,
        TOKEN_FUNCTION,
        TOKEN_LOOP,
        TOKEN_OTHERWISE,
        TOKEN_PRINTOUT,
        TOKEN_SWITCH,
        TOKEN_TERMINATE,
        TOKEN_TERMINATEALL,
        TOKEN_CONST,
        TOKEN_BLANK,


    }type;

    char* value;
    int line;
}token;

token* token_init(int type, char* value){
    token* token_instance = malloc(sizeof(token));

    token_instance->type = type;
    token_instance->value = value;
}


#endif