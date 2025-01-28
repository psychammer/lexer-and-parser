#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "token.h"
#include "parser.h"

// Global variables to store the token list and the current token index

int current_token = 0;
int token_length = 0;
bool panic_mode = false;


int main() {
    token* myTokens = get_tokens("output.txt");

    
    return 0;
}

token *get_tokens(const char *filename) {
    FILE *fp;
    char line[256];
    token myToken;

    fp = fopen(filename, "r"); // Replace "tokens.txt" with the actual file name

    if (fp == NULL) {
        printf("Error: Could not open file.\n");
        return (void*)0;
    }

    token *token_list = NULL; 

    token_list = (token *)calloc(1, sizeof(token));
    if (token_list == NULL) {
        perror("Memory allocation failed");
        fclose(fp);
        return (void*)0;
    }


    fgets(line, sizeof(line), fp); // Skip the header line
    fgets(line, sizeof(line), fp); // Skip the header line

    token_length = 0;
    int token_capacity = 1;
    while (fgets(line, sizeof(line), fp) != NULL) {
        if(token_length>=1){
            token_capacity *= 2;
            token_list = (token *)realloc(token_list, token_capacity*sizeof(token));
        }

        // Parse the line using strtok
        char *ptr = strtok(line, "|"); 

        if (ptr != NULL) {
            ptr = strtok(NULL, "|");
        }

        if (ptr != NULL) {
            token_list[token_length].type = atoi(ptr);
            ptr = strtok(NULL, "|");
        }

        if (ptr != NULL) {
            token_list[token_length].value = strdup(ptr);
            ptr = strtok(NULL, "|");
        }

        if (ptr != NULL) {
            token_list[token_length].line = atoi(ptr);
        }

        // Print the extracted token information
        // printf("Token Code: %d\n", token_list[token_length].type);
        // printf("Token Value: %s\n", token_list[token_length].value);
        // printf("Token Line Number: %d\n\n", token_list[token_length].line);

        token_length++;
    }

    printf("%d\n", token_length);

    fclose(fp);

    return token_list;
}