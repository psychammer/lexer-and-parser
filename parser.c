#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "token.h"
#include "parser.h"
#include <ctype.h>
// #include "parser.h"

char* getTokenType(int tokenTypeInt);
token *myTokens;
int current_token = 0;
int token_length = 0;
bool inside_body = false;
bool panic_mode = false;

FILE *output_file;

token *get_tokens(const char *filename) ;
char *trim_whitespace(char *str);

int main() {
    myTokens = get_tokens("output.txt");

    output_file = fopen("parse_tree_diagrams.ebnf", "w");
    if (output_file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    if (current_token == 0) { // Print header once
        printf("----------------------------------------\n");
        printf("| Token Type       | Value   | Line No. |\n");
        printf("----------------------------------------\n");
    }
    
    AST *root = parse_program();

    if (panic_mode) {
        printf("Error parsing program\n");
        fclose(output_file);
        remove("parse_tree_diagrams.ebnf");
    } else {
        printf("Parsing successful\n");
        print_parse_tree(root, 0);
        fclose(output_file);
    }
    if (current_token == token_length - 1) { // Print footer once
        printf("----------------------------------------\n");
    }
    free_AST_memory(root);
    free(myTokens);
    
    return 0;
}

// <program> ::= { <declaration> }
AST *parse_program() {
    AST *node = create_program_node();
    while (current_token < token_length && myTokens[current_token].type != TOKEN_EOF) {
        AST *statements = parse_statements();
        if(current_token+1 == token_length){
            break;
        }
        if (statements == NULL) {
            recover();
            continue;
        }
        add_child(node, statements);
    }
    return node;
}

AST *parse_statements()
{   
    AST *node = create_statements_node();
    
    // CHECKPOINT TO DELETE TOKEN_RBRACE
    while (current_token < token_length && myTokens[current_token].type != TOKEN_EOF && myTokens[current_token].type != TOKEN_RBRACE) {
        
        AST *statement = parse_statement();

        if(statement == NULL && current_token+1 == token_length){
                break;
        }

        if (statement == NULL && myTokens[current_token].type == TOKEN_RBRACE) {
            return NULL;
        }

        if (statement == NULL) {
            recover();
            continue;
        }
        add_child(node, statement);
    }
    return node;

}

AST *parse_statement() {
    AST *node = create_statement_node();

    // Return NULL if not a valid declaration start
    if (current_token >= token_length || 
        (myTokens[current_token].type != TOKEN_ID) &&
        (myTokens[current_token].type != TOKEN_IF) &&
        (myTokens[current_token].type != TOKEN_RETURN) &&
        (myTokens[current_token].type != TOKEN_FUNCTION) &&
        (myTokens[current_token].type != TOKEN_DATATYPE) &&
        (myTokens[current_token].type != TOKEN_FOR) &&
        (myTokens[current_token].type != TOKEN_WHILE) &&
        (myTokens[current_token].type != TOKEN_DO)
        ) 
        {
                
        return NULL;
    }

    // TOKEN IF
    if (current_token + 1 < token_length && 
        myTokens[current_token].type == TOKEN_IF){

        AST *conditional = parse_conditional();
        add_child(node, conditional);
        if(!conditional){
            fprintf(stderr, "Error: parsing conditional statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }
        return node;
    }

    // TOKEN PRINT
    if (current_token < token_length && 
        myTokens[current_token].type == TOKEN_NOISE &&
        strcmp(myTokens[current_token].value, "print")==0){
        AST *output = parse_output_statement();
        add_child(node, output);
        if(!output){
            fprintf(stderr, "Error: parsing output statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }

        return node;
    }
    // TOKEN INPUT (without datatype)
    else if(current_token < token_length && 
        myTokens[current_token].type == TOKEN_ID &&
        // check if the next two token is an input type, or if the nex token is a comma (indicating it's a ident list)
        strcmp(myTokens[current_token+2].value, "input")==0 || myTokens[current_token+1].type == TOKEN_COMMA){
        AST *input_statement = parse_input_statement();
        add_child(node, input_statement);
        if(!input_statement){
            fprintf(stderr, "Error: parsing output statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }
        return node;

    } 

    // TOKEN ASSIGNMENT_STATEMENT
    if (current_token + 1 < token_length && 
        myTokens[current_token].type == TOKEN_ID &&
        myTokens[current_token+1].type == TOKEN_OPERATOR){
        AST *assignment = parse_assignment();
        add_child(node, assignment);
        if(!assignment){
            fprintf(stderr, "Error: parsing assignment statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }

        // Expect semicolon at end
        if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
            add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
        } else {
            fprintf(stderr, "Error: Expected semicolon at end of variable declaration at line %d\n", 
                    myTokens[current_token-1].line);
            return NULL;
        }    

        return node;
    } 

    // TOKEN RETURN
    if (current_token < token_length && 
        myTokens[current_token].type == TOKEN_RETURN){
        AST *return_statement = parse_return_statement();
        add_child(node, return_statement);
        if(!return_statement){
            fprintf(stderr, "Error: parsing output statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }

        return node;
    } 

    
    // TOKEN INPUT (with datatype)
    if(current_token < token_length && 
        (myTokens[current_token].type == TOKEN_DATATYPE) && (strcmp(myTokens[current_token+2].value, "input")==0 || myTokens[current_token+1].type == TOKEN_COMMA)
        ){

        AST *input_statement = parse_input_statement();
        add_child(node, input_statement);
        if(!input_statement){
            fprintf(stderr, "Error: parsing output statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }
        return node;
    }
    else if(current_token < token_length && (myTokens[current_token].type == TOKEN_DATATYPE) ||
            ((myTokens[current_token+1].type == TOKEN_FUNCTION &&
        myTokens[current_token].type == TOKEN_DATATYPE)||
        myTokens[current_token].type == TOKEN_FUNCTION)){

        AST *declaration_stmt = parse_declaration_stmt();
        add_child(node, declaration_stmt);
        if(!declaration_stmt){
            fprintf(stderr, "Error: parsing declaration statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }
        return node;
    }

    // TOKEN FOR, WHILE
    if (current_token < token_length && 
        (myTokens[current_token].type == TOKEN_FOR || (myTokens[current_token].type == TOKEN_WHILE) || (myTokens[current_token].type == TOKEN_DO))
        ){
        AST *iterative = parse_iterative_statement();
        add_child(node, iterative);
        if(!iterative){
            fprintf(stderr, "Error: parsing output statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }

        return node;
    } 

    return NULL;
}


AST *parse_assignment() {
    AST *node = create_assignment_node();

    // Parse first identifier
    AST *identifier_node = parse_identifier();
    add_child(node, identifier_node);

    if(current_token < token_length && myTokens[current_token].type==TOKEN_OPERATOR && (myTokens[current_token].value[0] == '=' && myTokens[current_token].value[1] != '=' || (myTokens[current_token].value[0] != '=' && myTokens[current_token].value[1] == '=')) )
        add_child(node, (check_create_advance(TOKEN_OPERATOR, "Equals")));
    else if(current_token < token_length && myTokens[current_token].type==TOKEN_OPERATOR && 
                                            (myTokens[current_token].value[1] == '=') && 
                                            (myTokens[current_token].value[0] == '+' || 
                                            myTokens[current_token].value[0] == '-'  || 
                                            myTokens[current_token].value[0] == '*' ||
                                            myTokens[current_token].value[0] == '/'))
                                            {
                                                printf("here?");
                                                add_child(node, (check_create_advance(TOKEN_OPERATOR, "Assignment op")));
                                            }
        
    
    // Parse expression
    AST *expression = parse_exp();
    add_child(node, expression);
    if(!expression){
        // fprintf(stderr, "Error: parsing expression %d\n", 
        //         myTokens[current_token].line);
        return NULL;
    }
    

    return node;
}

// <identifier> ::= identifier token
AST *parse_identifier() {
    AST *node = create_identifier_node();

    if (myTokens[current_token].type == TOKEN_ID) {
        add_child(node, check_create_advance(TOKEN_ID, "IDENTIFIER"));
    } else {
        fprintf(stderr, "Error: Expected identifier at line %d\n", myTokens[current_token-1].line);
        return NULL;
    }

    return node;
}

AST *parse_exp() {
    AST *node = create_exp_node();

    // Parse the first term
    AST *term_node = parse_term();
    if (!term_node) {
        // fprintf(stderr, "Error: Expected term at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }
    add_child(node, term_node);

    // Parse additional terms connected by "+" or "-"
    while (current_token < token_length && 
            (strlen(myTokens[current_token].value)<=1) &&
           (myTokens[current_token].type == TOKEN_OPERATOR) && 
           (myTokens[current_token].value[0] == '+' || myTokens[current_token].value[0] == '-')) {
        // Match the operator
        AST *operator_node = check_create_advance(TOKEN_OPERATOR, "Operator");
        add_child(node, operator_node);

        // Parse the next term
        term_node = parse_term();
        if (!term_node) {
            // fprintf(stderr, "Error: Expected term after operator at Line: %d\n", myTokens[current_token].line);
            return NULL;
        }
        add_child(node, term_node);
    }

    return node;
}


AST *parse_term() {
    AST *node = create_term_node();

    // Parse the first factor
    AST *power_node = parse_power();
    if (!power_node) {
        // fprintf(stderr, "Error: Expected factor at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }
    add_child(node, power_node);

    // Parse additional factors connected by "*" or "/"
    while (current_token < token_length && 
            (strlen(myTokens[current_token].value)<=1) && 
           (myTokens[current_token].type == TOKEN_OPERATOR) &&

           (myTokens[current_token].value[0] == '*' || myTokens[current_token].value[0] == '/')) {
        // Match the operator
        AST *operator_node = check_create_advance(TOKEN_OPERATOR, "Operator");
        add_child(node, operator_node);

        // Parse the next factor
        power_node = parse_power();
        if (!power_node) {
            // fprintf(stderr, "Error: Expected factor after operator at Line: %d\n", myTokens[current_token].line);
            return NULL;
        }
        add_child(node, power_node);
    }

    return node;
}

AST *parse_power() {
    AST *node = create_power_node();

    // Parse the first factor
    AST *factor_node = parse_factor();
    if (!factor_node) {
        // fprintf(stderr, "Error: Expected factor at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }
    add_child(node, factor_node);

    // Parse additional factors connected by "**"
    while (current_token < token_length && 
           (myTokens[current_token].type == TOKEN_OPERATOR) && 
           (myTokens[current_token].value[0] == '*' && myTokens[current_token].value[1] == '*')) {
        // Match the operator
        AST *operator_node = check_create_advance(TOKEN_OPERATOR, "Operator");
        add_child(node, operator_node);

        // Parse the next factor
        factor_node = parse_factor();
        if (!factor_node) {
            // fprintf(stderr, "Error: Expected factor after operator at Line: %d\n", myTokens[current_token].line);
            return NULL;
        }
        add_child(node, factor_node);
    }

    return node;
}

AST *parse_factor() {
    AST *node = create_factor_node();

    if (current_token >= token_length) {
        // fprintf(stderr, "Error: Unexpected end of input at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }

    // Check for "(" <expression> ")"
    if (myTokens[current_token].type == TOKEN_LPAREN) {
        add_child(node, check_create_advance(TOKEN_LPAREN, "Left Parenthesis"));

        AST *exp_node = parse_exp();
        if (!exp_node) {
            // fprintf(stderr, "Error: Expected expression inside parentheses at Line: %d\n", myTokens[current_token].line);
            return NULL;
        }
        add_child(node, exp_node);

        if (current_token < token_length && myTokens[current_token].type == TOKEN_RPAREN) {
            add_child(node, check_create_advance(TOKEN_RPAREN, "Right Parenthesis"));
        } 
        // else if(current_token < token_length && myTokens[current_token].type == TOKEN_COMMA) {
        //     add_child(node, check_create_advance(TOKEN_COMMA, "Comma"));
        // } 
        else {
            // fprintf(stderr, "Error: Expected closing parenthesis at Line: %d\n", myTokens[current_token].line);
            return NULL;
        }
    }
    // Check for a number
    else if (myTokens[current_token].type == TOKEN_NUMBER) {
        AST *constant = parse_constant();
        add_child(node, constant);
    }
    // Check for an identifier
    else if (myTokens[current_token].type == TOKEN_ID) {
        add_child(node, check_create_advance(TOKEN_ID, "Identifier"));
    }
    // Unexpected token
    else {
        fprintf(stderr, "Error: Unexpected token '%s' at Line: %d\n", 
                myTokens[current_token].value, myTokens[current_token-1].line);
        return NULL;
    }

    return node;
}


// conditional statement
AST *parse_conditional() {
    AST *node = create_conditional_node();

    // expect if token
    add_child(node, (check_create_advance(TOKEN_IF, "If")));

    // expect left parenthesis token
    add_child(node, (check_create_advance(TOKEN_LPAREN, "Left parenthesis")));

    // expect bool expression
    AST *bool_expression_node = parse_bool_expression();
    add_child(node, bool_expression_node);
    if(!bool_expression_node){
        fprintf(stderr, "Error: parsing expression %d\n", 
        myTokens[current_token-1].line);
        return NULL;
    }

    // expect a right parenthesis
    add_child(node, (check_create_advance(TOKEN_RPAREN, "Right parenthesis")));



    AST *if_body = parse_body();
    add_child(node, if_body);
    if(!if_body){
            fprintf(stderr, "Error: parsing body %d\n", 
            myTokens[current_token-1].line);
            recover();
            return NULL;
    }

    while (current_token < token_length && myTokens[current_token].type == TOKEN_ELSE) {
        printf("here?\n");
        AST *else_body = parse_else();
        add_child(node, else_body);
        if(!else_body){
            fprintf(stderr, "Error: parsing else %d\n", 
            myTokens[current_token-1].line);
            return NULL;
        }
    }

    


    // expect semicolon at end
    // if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
    //     add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    // } else {
    //     fprintf(stderr, "Error: Expected semicolon at end of variable declaration at line %d, current values is %s \n", 
    //             myTokens[current_token].line, myTokens[current_token].value);
    //     return NULL;
    // }   

    return node;

}

AST *parse_else(){
    AST *node = create_else_node();
    add_child(node, check_create_advance(TOKEN_ELSE, "Else"));

    if (current_token < token_length && myTokens[current_token].type == TOKEN_IF) {
        AST *if_statement = parse_conditional();
        if(!if_statement){
            fprintf(stderr, "Error: parsing conditional %d\n", 
            myTokens[current_token-1].line);
            recover();
            return NULL;
        }
        add_child(node, if_statement);
    } else {
        AST *else_block = parse_body();
        if(!else_block){
            fprintf(stderr, "Error: parsing else %d\n", 
            myTokens[current_token-1].line);
            recover();
            return NULL;
        }
        add_child(node, else_block);
    }
    return node;

}

// <bool-expression>
AST *parse_bool_expression()
{
    AST  *node = create_bool_expression_node();

    // Parse the first factor
    AST *bool_term_node = parse_bool_term();
    if (!bool_term_node) {
        // fprintf(stderr, "Error: Expected factor at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }
    add_child(node, bool_term_node);

    while(current_token < token_length && 
           (myTokens[current_token].type == TOKEN_OPERATOR) && 
           (myTokens[current_token].value[0] == 'o' && myTokens[current_token].value[1] == 'r'))
    {
        // Match the operator
        AST *operator_node =  check_create_advance(TOKEN_OPERATOR, "Or");
        add_child(node, operator_node);

        // Parse the next term
        bool_term_node = parse_bool_term();
        if (!bool_term_node) {
            // fprintf(stderr, "Error: Expected term after operator at Line: %d\n", myTokens[current_token].line);
            return NULL;
        }
        add_child(node, bool_term_node);
    }

    return node;
}

// <bool-term>
AST *parse_bool_term()
{
    AST  *node = create_bool_term_node();

    // Parse the first factor
    AST *bool_factor_node = parse_bool_factor();
    add_child(node, bool_factor_node);
    if (!bool_factor_node) {
        // fprintf(stderr, "Error: Expected factor at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }

    while(current_token < token_length && 
           (myTokens[current_token].type == TOKEN_OPERATOR) && 
           (myTokens[current_token].value[0] == 'a' && myTokens[current_token].value[1] == 'n' && myTokens[current_token].value[2] == 'd'))
    {
        // Match the operator
        AST *operator_node =  check_create_advance(TOKEN_OPERATOR, "And");
        add_child(node, operator_node);

        // Parse the next factor
        bool_factor_node = parse_bool_factor();
        if (!bool_factor_node) {
            // fprintf(stderr, "Error: Expected factor after operator at Line: %d\n", myTokens[current_token].line);
            return NULL;
        }
        add_child(node, bool_factor_node);
    }

    return node;
}

// <bool-factor>
AST *parse_bool_factor()
{
    AST  *node = create_bool_factor_node();

    if (current_token >= token_length) {
        // fprintf(stderr, "Error: Unexpected end of input at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }
    if (myTokens[current_token].type == TOKEN_OPERATOR && strcmp(myTokens[current_token].value, "not") == 0) {
        add_child(node, check_create_advance(TOKEN_OPERATOR, "Not"));
        
        AST *bool_factor_node = parse_bool_factor();
        if (!bool_factor_node) {
            fprintf(stderr, "Error: Expected boolean factor after 'not' at Line: %d\n", myTokens[current_token-1].line);
            return NULL;
        }
        add_child(node, bool_factor_node);
    }
    // Check for "(" <expression> ")"
    else if (myTokens[current_token].type == TOKEN_LPAREN) {
        add_child(node, check_create_advance(TOKEN_LPAREN, "Left Parenthesis"));

        AST *bool_expression_node = parse_bool_expression();
        if (!bool_expression_node) {
            // fprintf(stderr, "Error: Expected expression inside parentheses at Line: %d\n", myTokens[current_token].line);
            return NULL;
        }
        add_child(node, bool_expression_node);

        if (current_token < token_length && myTokens[current_token].type == TOKEN_RPAREN) {
            add_child(node, check_create_advance(TOKEN_RPAREN, "Right Parenthesis"));
        } else {
            // fprintf(stderr, "Error: Expected closing parenthesis at Line: %d\n", myTokens[current_token].line);
            return NULL;
        }
    }
    else if (current_token < token_length && myTokens[current_token+1].type == TOKEN_OPERATOR &&
        (strcmp(myTokens[current_token+1].value, "<")==0 ||
        strcmp(myTokens[current_token+1].value, ">")==0 ||
        strcmp(myTokens[current_token+1].value, "==")==0 ||
        strcmp(myTokens[current_token+1].value, ">=")==0 ||
        strcmp(myTokens[current_token+1].value, "<=")==0)
        ) {
        AST *rel_expression = parse_rel_expression();    
        add_child(node, rel_expression);
    }
    // Check for a number
    else if (myTokens[current_token].type == TOKEN_NUMBER) {
        add_child(node, check_create_advance(TOKEN_NUMBER, "Number"));
    }
    // Check for an identifier
    else if (myTokens[current_token].type == TOKEN_ID) {
        add_child(node, check_create_advance(TOKEN_ID, "Identifier"));
    }
    // Unexpected token
    else {
        fprintf(stderr, "Error: Unexpected token '%s' at Line: %d\n", 
                myTokens[current_token].value, myTokens[current_token-1].line);
        return NULL;
    }

    return node;
}


AST *parse_rel_expression()
{
    AST *node = create_rel_expression_node();
    
    // Parse first expression
    AST *left_expr_node = parse_exp();
    if (!left_expr_node) return NULL;
    add_child(node, left_expr_node);
    
    // Match relational operator
    if (current_token < token_length && myTokens[current_token].type == TOKEN_OPERATOR &&
        strcmp(myTokens[current_token].value, "<")==0 ||
        strcmp(myTokens[current_token].value, ">")==0 ||
        strcmp(myTokens[current_token].value, "==")==0 ||
        strcmp(myTokens[current_token].value, ">=")==0 ||
        strcmp(myTokens[current_token].value, "<=")==0 
        )
    {
        AST *rel_op_node = check_create_advance(TOKEN_OPERATOR, myTokens[current_token].value);
        add_child(node, rel_op_node);
    }
    else
    {
        return NULL; // Error: Expected relational operator
    }
    
    // Parse second expression
    AST *right_expr_node = parse_exp();
    if (!right_expr_node) return NULL;
    add_child(node, right_expr_node);
    
    return node;
}


//Output statement  [ADDED]
// Parse print statement: "print" "(" <print-expression> ")" ";"
AST *parse_output_statement() {
    AST *node = create_output_statement_node();
    
    // Expect "print" keyword
    if (current_token < token_length && myTokens[current_token].type == TOKEN_NOISE) {
        add_child(node, check_create_advance(TOKEN_NOISE, "Print"));
    } else {
        fprintf(stderr, "Error: Expected 'print' keyword at line %d\n", 
                myTokens[current_token-1].line);
        return NULL;
    }
    
    // Expect left parenthesis
    if (current_token < token_length && myTokens[current_token].type == TOKEN_LPAREN) {
        add_child(node, check_create_advance(TOKEN_LPAREN, "Left Parenthesis"));
    } else {
        fprintf(stderr, "Error: Expected '(' after print at line %d\n", 
                myTokens[current_token-1].line);
        return NULL;
    }
    
    // Parse print expression
    AST *print_expr = parse_print_expression();
    if (!print_expr) {
        fprintf(stderr, "Error: Invalid print expression at line %d\n", 
                myTokens[current_token-1].line);
        return NULL;
    }
    add_child(node, print_expr);
    
    // Expect right parenthesis
    if (current_token < token_length && myTokens[current_token].type == TOKEN_RPAREN) {
        add_child(node, check_create_advance(TOKEN_RPAREN, "Right Parenthesis"));
    } else {
        fprintf(stderr, "Error: Expected ')' after print expression at line %d\n", 
                myTokens[current_token-1].line);
        return NULL;
    }
    
    // Expect semicolon
    if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
        add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    } else {
        fprintf(stderr, "Error: Expected ';' at end of print statement at line %d\n", 
                myTokens[current_token-1].line);
        return NULL;
    }
    
    return node;
}

// Parse print expression: ["] <expression> ["] [ "," <print-expression>]*  [ADDED]
AST *parse_print_expression() {
    AST *node = create_node("Print Expression");
    
    // Check for opening quote
    if (current_token < token_length && myTokens[current_token].type == TOKEN_STRING) {
        AST *string = parse_constant();
        add_child(node, string);
    }
    else if(current_token < token_length && myTokens[current_token].type == TOKEN_ID && myTokens[current_token+1].type == TOKEN_LPAREN){
        AST *function_call = parse_function_call();
        add_child(node, function_call);
    }
    // Parse expression
    else{
        AST *expr = parse_exp();
        if (!expr) {
            return NULL;
        }
        add_child(node, expr);
    }
    
    // Check for comma and additional print expressions
    while (current_token < token_length && myTokens[current_token].type == TOKEN_COMMA) {
        add_child(node, check_create_advance(TOKEN_COMMA, "Comma"));
        
        AST *next_expr = parse_print_expression();
        if (!next_expr) {
            return NULL;
        }
        add_child(node, next_expr);
    }
    
    return node;
}



AST *parse_return_statement() {
    AST *node = create_return_statement_node();

    // Expect "return" keyword
    if (current_token < token_length && myTokens[current_token].type == TOKEN_RETURN) {
        add_child(node, check_create_advance(TOKEN_RETURN, "Return"));
    } else {
        fprintf(stderr, "Error: Expected 'print' keyword at line %d\n", 
                myTokens[current_token-1].line);
        return NULL;
    }

    AST *expression = parse_exp();
    add_child(node, expression);

    add_child(node, check_create_advance(TOKEN_SEMI, "Semi"));
    
    return node;
}

AST *parse_function_statement() {
    AST *node = create_function_statement_node();

    if(myTokens[current_token].type == TOKEN_FUNCTION){
        // expect a function
        add_child(node, check_create_advance(TOKEN_FUNCTION, "Function"));
    }
    else if(myTokens[current_token].type == TOKEN_DATATYPE){
        // expect a token datatype
        add_child(node, check_create_advance(TOKEN_DATATYPE, "Datatype"));

        // // expect a function
        add_child(node, check_create_advance(TOKEN_FUNCTION, "Function"));
    }
    
    // expect an identifier
    AST *identifier = parse_identifier();
    add_child(node, identifier);

    // expect a left parenthesis
    add_child(node, check_create_advance(TOKEN_LPAREN, "Left parenthesis"));

    // expect a parameter list
    AST *parameter_list = parse_parameter_list();
    add_child(node, parameter_list);

    add_child(node, check_create_advance(TOKEN_RPAREN, "Right_Parenthesis"));

    if (current_token < token_length && myTokens[current_token].type == TOKEN_LBRACE) {
        AST *body = parse_body();
        add_child(node, body);
    } else if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
        add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    } else {
        recover();
    }
    return node;

}

AST *parse_iterative_statement() {
    AST *node = create_node("Iterative Statement");

    if (current_token >= token_length) {
        fprintf(stderr, "Error: Unexpected end of input\n");
        return node;
    }

    switch (myTokens[current_token].type) {
        case TOKEN_FOR:
            add_child(node, check_create_advance(TOKEN_FOR, "For"));
            add_child(node, parse_for_body());
            break;

        case TOKEN_WHILE:
            add_child(node, check_create_advance(TOKEN_WHILE, "While"));
            add_child(node, parse_while_body());
            break;

        case TOKEN_DO:
            add_child(node, check_create_advance(TOKEN_DO, "Do"));
            add_child(node, parse_do_while_body());
            break;

        default:
            fprintf(stderr, "Error: Expected 'for', 'while' or 'do' at line %d\n", 
                    myTokens[current_token-1].line);
            recover();
    }

    return node;
}

AST *parse_for_body() {
    AST *node = create_node("For Statement");

    add_child(node, check_create_advance(TOKEN_LPAREN, "("));

    add_child(node, parse_it_assign_stmt());

    // already expected a semicolon afte parse_assingment
    add_child(node, check_create_advance(TOKEN_SEMI, ";"));

    add_child(node, parse_bool_expression());

    add_child(node, check_create_advance(TOKEN_SEMI, ";"));

    add_child(node, parse_increment());
   
    add_child(node, check_create_advance(TOKEN_RPAREN, ")"));
   
    add_child(node, parse_body());

    return node;
}

// <it-assign-stmt> ::= [data-type] data-type “=” <expression> 
AST *parse_it_assign_stmt(){
    AST *node = create_node("Iterative Assignment statement");

    if(myTokens[current_token].type == TOKEN_DATATYPE)
        add_child(node, parse_datatype());

    add_child(node, parse_identifier());

    add_child(node, check_create_advance(TOKEN_OPERATOR, "Equals"));

    add_child(node, parse_exp());

    return node;
}


AST *parse_while_body() {
    AST *node = create_node("While Statement");

    add_child(node, check_create_advance(TOKEN_LPAREN, "("));
    add_child(node, parse_bool_expression());
    add_child(node, check_create_advance(TOKEN_RPAREN, ")"));
    add_child(node, parse_body());

    return node;
}

AST *parse_do_while_body() {
    AST *node = create_node("Do While Statement");

    add_child(node, parse_body());
    add_child(node, check_create_advance(TOKEN_WHILE, "while"));
    add_child(node, check_create_advance(TOKEN_LPAREN, "("));
    add_child(node, parse_bool_expression());
    add_child(node, check_create_advance(TOKEN_RPAREN, ")"));
    add_child(node, check_create_advance(TOKEN_SEMI, ";"));

    return node;
}


AST *parse_input_statement() {
    AST *node = create_node("Input Statement");

    // opt datatype
    if (current_token < token_length && myTokens[current_token].type == TOKEN_DATATYPE) {
        add_child(node, parse_datatype());
    }

    // identifier
    AST *identifier_node = parse_ident_list();
    add_child(node, identifier_node);
    if (!identifier_node) {
        fprintf(stderr, "Error: Expected identifier in input statement at line %d\n", myTokens[current_token-1].line);
        return NULL;
    }

    // '='
    if (current_token < token_length && myTokens[current_token].type == TOKEN_OPERATOR &&
        strcmp(myTokens[current_token].value, "=") == 0) {
        add_child(node, check_create_advance(TOKEN_OPERATOR, "Equals"));
    } else {
        fprintf(stderr, "Error: Expected '=' in input statement at line %d\n", myTokens[current_token-1].line);
        return NULL;
    }

    AST *type_cast = parse_type_cast();
    if (type_cast) {
        add_child(node, type_cast);
    }

    // opt '('
    if (current_token < token_length && myTokens[current_token].type == TOKEN_LPAREN) {
        add_child(node, check_create_advance(TOKEN_LPAREN, "Left Parenthesis"));
    }

    // "input"
    if (current_token < token_length && myTokens[current_token].type == TOKEN_ID &&
        strcmp(myTokens[current_token].value, "input") == 0) {
        add_child(node, check_create_advance(TOKEN_ID, "Input"));
    } else {
        fprintf(stderr, "Error: Expected 'input' function call in input statement at line %d\n", myTokens[current_token-1].line);
        return NULL;
    }

    // (
    add_child(node, check_create_advance(TOKEN_LPAREN, "Left Parenthesis"));

    // string constant
    if (current_token < token_length && myTokens[current_token].type == TOKEN_STRING) {
        add_child(node, parse_constant());
    } else {
        fprintf(stderr, "Error: Expected string constant inside input function at line %d\n", myTokens[current_token].line);
        return NULL;
    }

    // )
    add_child(node, check_create_advance(TOKEN_RPAREN, "Right Parenthesis"));

    // Opt )
    if (current_token < token_length && myTokens[current_token].type == TOKEN_RPAREN) {
        add_child(node, check_create_advance(TOKEN_RPAREN, "Right Parenthesis"));
    }

    // ;
    if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
        add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    } else {
        fprintf(stderr, "Error: Missing semicolon at end of input statement at line %d\n", myTokens[current_token].line);
        return NULL;
    }

    return node;
}


AST *parse_type_cast() {
        AST *node = create_node("Type Cast");

        // int,float,bool
        if (current_token < token_length && myTokens[current_token].type == TOKEN_DATATYPE &&
            (strcmp(myTokens[current_token].value, "int") == 0 ||
            strcmp(myTokens[current_token].value, "float") == 0 ||
            strcmp(myTokens[current_token].value, "bool") == 0)) {
            add_child(node, check_create_advance(TOKEN_DATATYPE, "Type Cast"));
        } else {
            return NULL;
        }

        return node;
}

// <ident-list>
AST *parse_ident_list() {
    AST *node = create_node("Identifier List");

    // first ident
    AST *identifier = parse_identifier();
    add_child(node, identifier);
    if (!identifier) {
        fprintf(stderr, "Error: Expected identifier in identifier list at line %d\n", myTokens[current_token].line);
        return NULL;
    }

    // handles multiple idents
    while (current_token < token_length && myTokens[current_token].type == TOKEN_COMMA) {
        add_child(node, check_create_advance(TOKEN_COMMA, "Comma"));

        AST *next_identifier = parse_identifier();
        add_child(node, next_identifier);
        if (!next_identifier) {
            fprintf(stderr, "Error: Expected identifier after comma in identifier list at line %d\n", myTokens[current_token].line);
            return NULL;
        }
    }

    return node;
}

// <dec-assign-stmt> 
AST *parse_dec_assign(){
    AST *node = create_node("Declaration assignment statement");
    // expect data type
    add_child(node, parse_datatype());

    /* expect identifier --------------------------------------------connect to <ident-list> parsing
    AST *identifier_list_node = parse_identifier_list();
    add_child(node, identifier_list_node);
    */ 

    AST *identifier_list_node = parse_ident_list();
    add_child(node, identifier_list_node);
    
    if(current_token < token_length && myTokens[current_token].type==TOKEN_OPERATOR && (myTokens[current_token].value[0] == '=' && myTokens[current_token].value[1] != '=' || (myTokens[current_token].value[0] != '=' && myTokens[current_token].value[1] == '=')) )
        add_child(node, (check_create_advance(TOKEN_OPERATOR, "Equals")));

    // Parse expression
    AST *expression = parse_exp();
    add_child(node, expression);
    if(!expression){
        fprintf(stderr, "Error: parsing expression %d\n", 
            myTokens[current_token].line);
        return NULL;
    }

    // semicolon
    if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
        add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    } else {
        fprintf(stderr, "Error: Expected semicolon at end of variable declaration at line %d current value is %s\n", 
                myTokens[current_token-1].line, myTokens[current_token].value);
        recover();
    }       

    return node; 
}

// <array-stmt> ::= [data-type] identifier (“[“ (int-const | identifier)* “]”)+ [“=” “{“ <expression> “}”] “;”
AST *parse_array(){
    AST *node = create_node("Array statement");

    if (current_token < token_length && myTokens[current_token].type == TOKEN_DATATYPE) 
        add_child(node, parse_datatype());

    // expected identifier
    AST *identifier_node = parse_identifier();
    add_child(node, identifier_node);

    //expected left bracket
    add_child(node, check_create_advance(TOKEN_LBRACKET, "Left bracket"));
    if(current_token < token_length && myTokens[current_token].type==TOKEN_ID){
        AST *identifier_node = parse_identifier(); // array identifier dimension
        add_child(node, identifier_node);
    } else if (current_token < token_length && myTokens[current_token].type==TOKEN_NUMBER){
        add_child(node, parse_constant());
    }
    //expected right bracket
    add_child(node, check_create_advance(TOKEN_RBRACKET, "Right bracket"));

    //kleene star for array dimension (kleene star cause we already expect the array identifier to have 1 dimension)
    while(current_token < token_length && myTokens[current_token].type==TOKEN_LBRACKET){
        add_child(node, check_create_advance(TOKEN_LBRACKET, "Left bracket"));
        if(current_token < token_length && myTokens[current_token].type==TOKEN_ID){
            AST *identifier_node = parse_identifier(); // array identifier dimension
            add_child(node, identifier_node);
        } else if (current_token < token_length && myTokens[current_token].type==TOKEN_NUMBER){
            add_child(node, check_create_advance(TOKEN_NUMBER, "Number"));
        }
        //expected right bracket
        add_child(node, check_create_advance(TOKEN_RBRACKET, "Right bracket"));
    }

    //if array has assigned values
    if(current_token < token_length && myTokens[current_token].type==TOKEN_OPERATOR && (myTokens[current_token].value[0] == '=' && myTokens[current_token].value[1] != '=' || (myTokens[current_token].value[0] != '=' && myTokens[current_token].value[1] == '=')) ){
        add_child(node, (check_create_advance(TOKEN_OPERATOR, "Equals")));

        //expected left brace
        add_child(node, check_create_advance(TOKEN_LBRACE, "Left_Brace"));

        //expected expression
        AST *expression = parse_exp();
        add_child(node, expression);
        if(!expression){
            fprintf(stderr, "Error: parsing expression %d\n", 
                myTokens[current_token].line);
            return NULL;
        }

        while(myTokens[current_token].type == TOKEN_COMMA){
            add_child(node, check_create_advance(TOKEN_COMMA, "Comma"));

            //expected multiple expression
            AST *expression = parse_exp();
            add_child(node, expression);
            if(!expression){
                fprintf(stderr, "Error: parsing expression %d\n", 
                    myTokens[current_token].line);
                return NULL;
            }
        }

        //expected right brace
        add_child(node, check_create_advance(TOKEN_RBRACE, "Right_Brace"));
    }

    // Expect semicolon at end
    if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
        add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    } else {
        fprintf(stderr, "Error: Expected semicolon at end of variable declaration at line %d\n", 
                myTokens[current_token-1].line);
        return NULL;
    }        

    return node;
}


AST *parse_variable_stmt(){
    AST *node = create_node("Variable statement");

    AST *datatype = parse_datatype();
    add_child(node, datatype);

    AST *identifier = parse_identifier();
    add_child(node, identifier);


    // Expect semicolon at end
    if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
        add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    } else {
        fprintf(stderr, "Error: Expected semicolon at end of variable declaration at line %d\n", 
                myTokens[current_token-1].line);
        return NULL;
    }     

    return node;
}



AST *parse_declaration_stmt(){
    AST *node = create_node("Declaration statement");

    // <function-stmt>
    if (current_token < token_length && 
        ((myTokens[current_token+1].type == TOKEN_FUNCTION &&
        myTokens[current_token].type == TOKEN_DATATYPE)||
        myTokens[current_token].type == TOKEN_FUNCTION)
        ){
        AST *function_statement = parse_function_statement();
        add_child(node, function_statement);
        if(!function_statement){
            fprintf(stderr, "Error: parsing output statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }

        return node;
    }
    // <variable-stmt>
    else if(current_token < token_length && 
        (myTokens[current_token].type == TOKEN_DATATYPE) && (myTokens[current_token+1].type == TOKEN_ID) && (myTokens[current_token+2].type == TOKEN_SEMI)){
            AST *variable_stmt = parse_variable_stmt();
            add_child(node, variable_stmt);
            if(!variable_stmt){
                fprintf(stderr, "Error: parsing output statement %d\n", 
                    myTokens[current_token-1].line);
                return NULL;
            }
            return node;
    }
    // <dec-assign-stmt>
    else if(current_token < token_length && 
        (myTokens[current_token].type == TOKEN_DATATYPE) && (myTokens[current_token+1].type == TOKEN_ID) && (myTokens[current_token+2].type == TOKEN_OPERATOR) &&(strcmp(myTokens[current_token+3].value, "input")!=0)){
        AST *dec_assign = parse_dec_assign();
        add_child(node, dec_assign);
        if(!dec_assign){
            fprintf(stderr, "Error: parsing Declaration assignment statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }
        return node;
    }
    // <array-stmt>
    else if(current_token < token_length && 
        (myTokens[current_token].type == TOKEN_DATATYPE) && (myTokens[current_token+2].type == TOKEN_LBRACKET)){
        AST *array = parse_array();
        add_child(node, array);
        if(!array){
            fprintf(stderr, "Error: parsing output statement %d\n", 
                myTokens[current_token-1].line);
            return NULL;
        }
        return node;
    }

    return node;
}





// <increment> 
AST *parse_increment() {
    AST *node = create_node("Increment");

    //  identifier (“=” | “+=” | “-=” | “/=” | “*=” “//=” )<expression>
    if(myTokens[current_token].type==TOKEN_ID && (myTokens[current_token+1].value[0]=='=' || 
                                                // +=, -=, etc.
                                                (myTokens[current_token+1].value[0]!='=' && 
                                                myTokens[current_token+1].value[1]=='=')))
    {
        add_child(node, parse_identifier());

        add_child(node, check_create_advance(TOKEN_OPERATOR, "Increment operator"));

        AST *expression = parse_exp();
        add_child(node, expression);
    }
    // <expression>("++" | "--")
    else{
        AST *expression = parse_exp();
        add_child(node, expression);

        add_child(node, check_create_advance(TOKEN_OPERATOR, "Increment operator"));
    }
    


    if (current_token >= token_length) {
        fprintf(stderr, "Error: Unexpected end of input\n");
        return NULL;
    }


    return node;
}


AST *parse_function_call()
{
    AST *node = create_function_call_node();

    // Expect identifier
    AST *identifier = parse_identifier();
    add_child(node, identifier);

    // expect a left parenthesis
    add_child(node, check_create_advance(TOKEN_LPAREN, "Left parenthesis"));

    // expect a expression
    AST *expression = parse_exp();
    add_child(node, expression);

    while (myTokens[current_token].type == TOKEN_COMMA)
    {
        add_child(node, check_create_advance(TOKEN_COMMA, "Comma"));

        expression = parse_exp();
        add_child(node, expression);
    }
    add_child(node, check_create_advance(TOKEN_RPAREN, "Right_Parenthesis"));

    return node;
}

AST *parse_parameter_list() {
    AST *node = create_parameter_list_node();

    
    if (current_token < token_length && myTokens[current_token].type == TOKEN_RPAREN) {
        // Empty parameter list
    } else if (current_token < token_length && myTokens[current_token].type == TOKEN_DATATYPE && myTokens[current_token].value[0] == 'v') {
            add_child(node, check_create_advance(TOKEN_VOID, "VOID"));
    } else {
        if (current_token < token_length && (myTokens[current_token].type == TOKEN_DATATYPE)) {
            AST *datatype = parse_datatype();
            add_child(node, datatype);

            AST *identifier_node = parse_identifier();
            add_child(node, identifier_node);

            while (current_token < token_length && myTokens[current_token].type == TOKEN_COMMA) {
                add_child(node, check_create_advance(TOKEN_COMMA, "Comma"));

                if (current_token < token_length && (myTokens[current_token].type == TOKEN_DATATYPE)) {
                    add_child(node, check_create_advance(TOKEN_DATATYPE, "Datatype"));

                    AST *identifier_node = parse_identifier();
                    add_child(node, identifier_node);

                } else {
                    fprintf(stderr, "Error: Expected data type after comma in parameter list at line %d\n", myTokens[current_token].line);
                    recover();
                }
            }
        } else {
            fprintf(stderr, "Error: Expected data type or ')' at the start of parameter list at line %d\n", myTokens[current_token].line);
            recover();
        }
    }
    return node;
}

AST *parse_datatype() {
    AST *node = create_datatype_node();
    if (current_token < token_length) {
        if (myTokens[current_token].value[0] == 'i') {
            add_child(node, check_create_advance(TOKEN_DATATYPE, "Int"));
        } else if (myTokens[current_token].value[0] == 'f') {
            add_child(node, check_create_advance(TOKEN_DATATYPE, "float"));
        } else if (myTokens[current_token].value[0] == 'c') {
            add_child(node, check_create_advance(TOKEN_DATATYPE, "char"));
        } else if (myTokens[current_token].value[0] == 'b') {
            add_child(node, check_create_advance(TOKEN_DATATYPE, "bool"));
        } else {
            fprintf(stderr, "Error: Expected data type at line %d\n", myTokens[current_token].line);
            recover();
        }
    }
    return node;
}

// checkpoint to check for errors
AST *parse_body(){
    inside_body = true;
    AST *node = create_body_node();
    add_child(node, check_create_advance(TOKEN_LBRACE, "Left Brace"));

    while (current_token < token_length && myTokens[current_token].type != TOKEN_RBRACE) {
        AST *statements = parse_statements();
        if (statements != NULL) {
            add_child(node, statements);
        } else {
            recover();
            if (current_token >= token_length || 
                myTokens[current_token].type == TOKEN_RBRACE) {
                break;
            }
            return NULL;
        }
    }

    if (current_token < token_length && myTokens[current_token].type == TOKEN_RBRACE) {
        add_child(node, check_create_advance(TOKEN_RBRACE, "Right_Brace"));
    } else {
        fprintf(stderr, "Error: Missing closing brace at line %d\n", 
                myTokens[current_token-1].line);
        recover();
    }

    return node;
}


AST *parse_constant(){
    AST *node = create_constant_node();
    
    if(myTokens[current_token].type==TOKEN_STRING){
        add_child(node, check_create_advance(TOKEN_STRING, "String"));
    }
    else if(myTokens[current_token].type==TOKEN_NUMBER){
        add_child(node, check_create_advance(TOKEN_NUMBER, "Number"));
    }

    return node;
}



// Implement create functions for each non-terminal
AST *create_program_node() {
    return create_node("Program");
}

AST *create_statements_node() {
    return create_node("Statements");
}

AST *create_statement_node() {
    return create_node("Statement");
}

AST *create_assignment_node() {
    return create_node("Assignment");
}

AST *create_identifier_node() {
    return create_node("Identifier");
}

AST *create_exp_node() {
    return create_node("Expression");
}

AST *create_term_node() {
    return create_node("Term");
}

AST *create_power_node() {
    return create_node("Power");
}

AST *create_factor_node() {
    return create_node("Factor");
}

AST *create_conditional_node() {
    return create_node("Conditional");
}

AST *create_bool_expression_node() {
    return create_node("Bool Expression");
}

AST *create_bool_term_node() {
    return create_node("Bool Term");
}

AST *create_bool_factor_node() {
    return create_node("Bool Factor");
}

AST *create_body_node() {
    return create_node("Body");
}

AST *create_output_statement_node() {
    return create_node("Output");
}

AST *create_parse_print_expression_node()
{
    return create_node("Print");
}

AST *create_return_statement_node()
{
    return create_node("Return");
}

AST *create_else_node()
{
    return create_node("Else");
}

AST *create_function_statement_node()
{
    return create_node("Function");
}

AST *create_parameter_list_node()
{
    return create_node("Parameter list");
}

AST *create_datatype_node()
{
    return create_node("Datatype");
}

AST *create_rel_expression_node()
{
    return create_node("Relational Expression");
}

AST *create_constant_node()
{
    return create_node("Constant");
}

AST * create_function_call_node()
{
    return create_node("Function Call");
}

// Function to allocate and initialize a new AST
AST *create_node(const char *name) {
    AST *node = malloc(sizeof(AST));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed in create_node\n");
        recover();
    }
    node->name = strdup(name);
    if (!node->name) {
        fprintf(stderr, "Error: Memory allocation failed in create_node\n");
        recover();
    }
    node->token = NULL;
    node->children = NULL;
    node->num_children = 0;
    return node;
}

// Helper function to match the current token with the expected type and create a node for it
AST *check_create_advance(TokenType type, const char* node_name) {
    printf("| %-15s | %-7s | %-7d |\n", getTokenType(myTokens[current_token].type), myTokens[current_token].value, myTokens[current_token].line);
    AST *node = create_node(node_name);
    node->token = malloc(sizeof(token));
    if (!node->token) {
        fprintf(stderr, "Error: Memory allocation failed in match_and_create_node\n");
        recover();
    }
    *node->token = myTokens[current_token];

    if (current_token < token_length && myTokens[current_token].type == type) {
        current_token++;
    } else {
        printf("Unexpected token %s at line %d \n", getTokenType(myTokens[current_token].type), myTokens[current_token].line);
        recover();
    }
    return node;
}


void recover() {
    panic_mode = true;
    while (current_token < token_length) {
        // // checkpoint to be deleted
        // if (myTokens[current_token].type == TOKEN_SEMI){
        //     current_token++;            
        //     return;
        // }
        if(current_token+1 == token_length){
            return;
        }

        if (myTokens[current_token].type == TOKEN_SEMI ||
            myTokens[current_token].type == TOKEN_RBRACE ||
            myTokens[current_token].type == TOKEN_BOOL ||
            myTokens[current_token].type == TOKEN_FOR ||
            myTokens[current_token].type == TOKEN_WHILE ||
            myTokens[current_token].type == TOKEN_IF ||
            myTokens[current_token].type == TOKEN_ELSE ||
            myTokens[current_token].type == TOKEN_DATATYPE ||
            strcmp(myTokens[current_token].value, "fun")==0
            ) {
            
            // checkpoint to be deleted
            current_token++;
            return;
        }
        current_token++;
    }
}

void add_child(AST *parent, AST *child) {
    parent->num_children++;
    AST **new_children = realloc(parent->children,  parent->num_children * sizeof(AST *));
    if (!new_children) {
        fprintf(stderr, "Error: Memory allocation failed in add_child\n");
        recover(); 
    }
    parent->children = new_children;
    parent->children[parent->num_children - 1] = child;
}


void print_parse_tree(AST *node, int tree_height) {
    if (node == NULL) {
        return;
    }

    print_indent(tree_height);

    // Terminal node
    if (node->token != NULL) {
        // Print constants
        if (node->token->type == TOKEN_NUMBER ||
            node->token->type == TOKEN_ID ||
            node->token->type == TOKEN_STRING ||
            node->token->type == TOKEN_OPERATOR ||
            node->token->type == TOKEN_DATATYPE) {
            
            // Only have a single set of quotations for string literals
            if (node->token->type == TOKEN_STRING) {
                fprintf(output_file, "%s: %s", 
                        getTokenType(node->token->type), 
                        node->token->value);
            } else {
                fprintf(output_file, "%s: \"%s\"", 
                        getTokenType(node->token->type), 
                        node->token->value);
            }
        } 
        // Otherwise, just print the token type
        else {
            fprintf(output_file, "%s", getTokenType(node->token->type));
        }
    } 
    // Non-terminal node
    else {
        fprintf(output_file, "%s(", node->name);

        // Traverse the tree / recursively print the children
        if (node->num_children > 0) {
            fprintf(output_file, "\n");
            for (int i = 0; i < node->num_children; i++) {
                print_parse_tree(node->children[i], tree_height + 1);
                if (i < node->num_children - 1) {
                    fprintf(output_file, ",\n");
                }
            }
            fprintf(output_file, "\n");
            print_indent(tree_height);
        }
        fprintf(output_file, ")");
    }
}


void free_AST_memory(AST *node) {
    if (!node) return;
    
    for (int i = 0; i < node->num_children; i++) {
        free_AST_memory(node->children[i]);
    }
    
    free(node->token);
    free(node->name);
    free(node->children);
    free(node);
}

void print_indent(int tree_height) {
    for (int i = 0; i < tree_height; i++) {
        fprintf(output_file, "  ");
    }
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
        case TOKEN_NOISE: return "TOKEN_NOISE"; break;


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

token *get_tokens(const char *filename) {
    FILE *fp;
    char line[256];
    int token_capacity = 1;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Could not open file.\n");
        return NULL;
    }

    // Allocate initial memory for token list
    token *token_list = (token *)malloc(token_capacity * sizeof(token));
    if (token_list == NULL) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    // Skip header lines
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp) != NULL) {
        // Ensure capacity before adding a new token
        if (token_length >= token_capacity) {
            token_capacity *= 2;
            token *temp = (token *)realloc(token_list, token_capacity * sizeof(token));
            if (temp == NULL) {
                perror("Memory reallocation failed");
                free(token_list);
                fclose(fp);
                return NULL;
            }
            token_list = temp;
        }

        // Parse the line
        char *ptr = strtok(line, "|");
        if (ptr != NULL) ptr = strtok(NULL, "|");  // Skip token name

        if (ptr != NULL) {
            token_list[token_length].type = atoi(ptr);
            ptr = strtok(NULL, "|");
        }

        if (ptr != NULL) {
            char *trimmed_value = trim_whitespace(ptr);
            token_list[token_length].value = strdup(trimmed_value);
            ptr = strtok(NULL, "|");
        }

        if (ptr != NULL) {
            token_list[token_length].line = atoi(ptr);
        }

        // printf("Token Code: %d\n", token_list[token_length].type);
        // printf("Token Value: %s\n", token_list[token_length].value);
        // printf("Token Line Number: %d\n\n", token_list[token_length].line);

        token_length++;
    }

    fclose(fp);
    return token_list;
}

char *trim_whitespace(char *str) {
    char *end;

    // Trim leading spaces
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing spaces
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
    return str;
}

