#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"
#include "lexer.h"

char* getContent(char* filePath);
char* getTokenType(int tokenTypeInt);
int ends_with(const char *str, const char *suffix);

int main(int argc, char *argv[]){


    token* myToken = token_init(TOKEN_ID, "myVariable");

    char* filePath = argv[1];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file.ri> <output_file.txt>\n", argv[0]);
        return 1; // Indicate an error
    }

    if (!ends_with(filePath, ".ri")) {
        fprintf(stderr, "Error: Input file must have the .ri extension.\n");
        return 1;
    }

    char* content = getContent(filePath);

    lexer* myLexer = lexer_init(content);

    int size = 0;

    token* token_arr = calloc(1, sizeof(token));

    while(myLexer->current_char!=EOF && myLexer->i < strlen(myLexer->content)){

        token* token_instance = token_buffer(myLexer);        

        if(token_instance->value != NULL){
            size++;

            // Resize token array
            token* temp = realloc(token_arr, size * sizeof(token));
            token_arr = temp;

            // Copy token instance data
            token_arr[size - 1].type = token_instance->type;
            token_arr[size - 1].value = strdup(token_instance->value);
            token_arr[size - 1].line = token_instance->line;
        }
        
    }

    // printf("%s\n", content);

    FILE *file = fopen(argv[2], "w");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fprintf(file, "%-20s | %-15s | %-30s | %-20s\n", 
        "TOKEN_NAME", "TOKEN_CODE", "TOKEN_VALUE", "TOKEN_LINE_NUMBER");
    fprintf(file, "---------------------|-----------------|--------------------------------|---------------------\n");

    for (int i = 0; i < size; i++) {
        fprintf(file, "%-20s | %-15d | %-30s | %-20d\n", 
                getTokenType(token_arr[i].type), 
                token_arr[i].type, 
                token_arr[i].value, 
                token_arr[i].line+1);
    }

    fclose(file);

    
    return 0;
}




 char* getTokenType(int tokenTypeInt) {
    switch (tokenTypeInt) {
        case 0: return "TOKEN_ID"; break;
        case 1: return "TOKEN_EQUALS"; break;
        case 2: return "TOKEN_STRING"; break;
        case 3: return "TOKEN_SYMBOL"; break;
        case 4: return "TOKEN_SEMI"; break;
        case 5: return "TOKEN_LPAREN"; break;
        case 6: return "TOKEN_RPAREN"; break;
        case 7: return "TOKEN_RBRACE"; break;
        case 8: return "TOKEN_LBRACE"; break;
        case 9: return "TOKEN_LBRACKET"; break;
        case 10: return "TOKEN_RBRACKET"; break;
        case 11: return "TOKEN_COMMA"; break;
        case 12: return "TOKEN_COLON"; break;
        case 13: return "TOKEN_TILDE"; break;
        case 14: return "TOKEN_QUESTION";  break;
        case 15: return "TOKEN_EXCLAMATION"; break;
        case 16: return "TOKEN_AMPERSAND"; break;
        case 17: return "TOKEN_DOT"; break;
        case 18: return "TOKEN_EOF";break;
        case 19: return "TOKEN_OPERATOR";break;

        case 20: return "TOKEN_KEYWORD";break;
        case TOKEN_IF: return "TOKEN_IF"; break;
        case TOKEN_ELSE: return "TOKEN_ELSE"; break;
        case TOKEN_WHILE: return "TOKEN_WHILE"; break;
        case TOKEN_DO: return "TOKEN_DO"; break;
        case TOKEN_RETURN: return "TOKEN_RETURN"; break;
        case TOKEN_FOR: return "TOKEN_FOR"; break;
        case TOKEN_CASE: return "TOKEN_CASE"; break;
        case TOKEN_BREAK: return "TOKEN_BREAK"; break;
        case TOKEN_TRY: return "TOKEN_TRY"; break;
        case TOKEN_CATCH: return "TOKEN_CATCH"; break;


        case 31: return "TOKEN_RESERVEDWORDS";break;
        case 32: return "TOKEN_DATATYPE";break;
        case 33: return "TOKEN_INT";break;
        case 34: return "TOKEN_FLOAT";break;
        case 35: return "TOKEN_DOUBLE";break;
        case 36: return "TOKEN_CHAR";break;
        case 37: return "TOKEN_VOID";break;
        case 38: return "TOKEN_BOOL";break;
        case 39: return "TOKEN_SINGLECOMMENT";break;
        case 40: return "TOKEN_MULTICOMMENT";break;
        case 41: return "TOKEN_NUMBER";break;
        case TOKEN_CHECK: return "TOKEN_CHECK"; break;
        case TOKEN_ELIF: return "TOKEN_ELIF"; break;
        case TOKEN_ENDLOOP: return "TOKEN_ENDLOOP"; break;
        case TOKEN_FUNCTION: return "TOKEN_FUNCTION"; break;
        case TOKEN_LOOP: return "TOKEN_LOOP"; break;
        case TOKEN_OTHERWISE: return "TOKEN_OTHERWISE"; break;
        case TOKEN_PRINTOUT: return "TOKEN_PRINTOUT"; break;
        case TOKEN_SWITCH: return "TOKEN_SWITCH"; break;
        case TOKEN_TERMINATE: return "TOKEN_TERMINATE"; break;
        case TOKEN_TERMINATEALL: return "TOKEN_TERMINATEALL"; break; 
        case TOKEN_BLANK: return "TOKEN_BLANK"; break;
        case TOKEN_CONST: return "TOKEN_CONST"; break;
        default: return "UNKNOWN_TOKEN";break;
    }
}





char* getContent(char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Seek to the end to determine file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);  // Reset pointer to the start of the file

    // Allocate memory for the string (+1 for null terminator)
    char* content = (char*)malloc((fileSize + 1) * sizeof(char));
    if (content == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    fread(content, sizeof(char), fileSize, file);
    content[fileSize] = '\0';  // Null-terminate the string

    fclose(file);
    return content;
}

int ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) {
        return 0;
    }
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len) {
        return 0;
    }
    return 0 == strncmp(str + str_len - suffix_len, suffix, suffix_len);
}