#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "AST.h"
#include "visitor.h"


void print_ast_prefix(AST_T* node, visitor_T* visitor);

char* getContent(char* filePath);
char* getTokenType(int tokenTypeInt);

int main(int argc, char *argv[]){


    token* myToken = token_init(TOKEN_ID, "myVariable");

    char* filePath = argv[1];

    if(filePath[strlen(filePath)-2]!='r' && filePath[strlen(filePath)-1]!='i'){
        printf("Incorrect file format.\n");
        return 1;
    }

    char* content = getContent(filePath);

    if(content==NULL){
        printf("Fail to read file.\n");
        return 1;
    }

    lexer* myLexer = lexer_init(content);


    int size = 0;
    token* token_arr = malloc(sizeof(token));
    // while(myLexer->current_char!=EOF && myLexer->i < strlen(myLexer->content)){
    parser_T* parser = init_parser(myLexer);
    AST_T* root = parser_parse(parser, parser->scope);
    // print_ast_prefix(root);
    visitor_T* visitor = init_visitor(argv[2]);
    visitor_visit(visitor, root);

    fclose(visitor->filename);

        // print_ast_prefix(root);
        // token* token_instance = token_buffer(myLexer);        

        // if(token_instance->value != NULL){
        //     size++;

        //     // Resize token array
        //     token* temp = realloc(token_arr, size * sizeof(token));
        //     token_arr = temp;

        //     // Copy token instance data
        //     token_arr[size - 1].type = token_instance->type;
        //     token_arr[size - 1].value = strdup(token_instance->value);
        // }
        
    // }

    // printf("size of token: %d", size);

    // FILE *file = fopen(argv[2], "w");
    // if (file == NULL) {
    //     perror("Error opening file");
    //     return 1;
    // }


    // for(int i = 0; i < size; i++) {
    //     fprintf(file, "%s, %s\n", getTokenType(token_arr[i].type), token_arr[i].value);
    // }

    // fclose(file);
    
    
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
        case TOKEN_CONST: return "TOKEN_CONST"; break;
        case TOKEN_BLANK: return "TOKEN_BLANK"; break;
        default: return "UNKNOWN_TOKEN";break;
    }
}





char* getContent(char* filePath){
    FILE *fptr;

    char ch;

    fptr = fopen(filePath, "r");

    if(fptr == NULL){
        return NULL;
    }

    char* content = malloc(sizeof(char));
    while((ch=fgetc(fptr)) != EOF){
        char* char_as_string = malloc(sizeof(char)+1);
        char_as_string[0] = ch;
        char_as_string[1] = '\0';

        content = realloc(content, strlen(content)+ strlen(content) + 1);
        strcat(content, char_as_string);

        free(char_as_string);
    }

    return content;
}
