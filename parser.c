#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "token.h"
#include "parser.h"
#include <ctype.h>
// #include "parser.h"

char* getTokenType(int tokenTypeInt);
// Global variables to store the token list and the current token index
token *myTokens;
int current_token = 0;
int token_length = 0;
bool panic_mode = false;

FILE *output_file;

token *get_tokens(const char *filename) ;
char *trim_whitespace(char *str);

int main() {
    myTokens = get_tokens("output.txt");

    output_file = fopen("parse_tree_output.ebnf", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error opening output file.\n");
        return 1;
    }
    
    ParseTreeNode *root = parse_program();

    if (panic_mode) {
        printf("Parsing failed!\n");
        fclose(output_file);
        remove("parse_tree_output.ebnf");
    } else {
        printf("Parsing successful!\n");
        print_parse_tree(root, 0);
        fclose(output_file);
    }
    free_parse_tree(root);
    free(myTokens);
    
    return 0;
}

// <program> ::= { <declaration> }
ParseTreeNode *parse_program() {
    ParseTreeNode *node = create_program_node();
    while (current_token < token_length && myTokens[current_token].type != TOKEN_EOF) {
        ParseTreeNode *statements = parse_statements();
        if (statements == NULL) {
            recover();
            continue;
        }
        add_child(node, statements);
    }
    return node;
}


ParseTreeNode *parse_statements()
{   
    ParseTreeNode *node = create_statements_node();
    
    // CHECKPOINT TO DELETE TOKEN_RBRACE
    while (current_token < token_length && myTokens[current_token].type != TOKEN_EOF && myTokens[current_token].type != TOKEN_RBRACE) {
        ParseTreeNode *statement = parse_statement();
        if (statement == NULL) {
            recover();
            continue;
        }
        add_child(node, statement);
    }
    return node;

}

ParseTreeNode *parse_statement() {
    ParseTreeNode *node = create_statement_node();

    // Return NULL if not a valid declaration start
    if (current_token >= token_length || 
        (myTokens[current_token].type != TOKEN_ID) &&
        (myTokens[current_token].type != TOKEN_IF) &&
        (myTokens[current_token].type != TOKEN_RETURN) &&
        (myTokens[current_token].type != TOKEN_FUNCTION) &&
        (myTokens[current_token].type != TOKEN_DATATYPE)
        ) 
        {
        return NULL;
    }

    // TOKEN IF
    if (current_token + 1 < token_length && 
        myTokens[current_token].type == TOKEN_IF){

        ParseTreeNode *conditional = parse_conditional();
        add_child(node, conditional);
        if(!conditional){
            fprintf(stderr, "Error: parsing conditional statement %d\n", 
                myTokens[current_token].line);
            return NULL;
        }
        return node;
    }

    // TOKEN ASSIGNMENT
    if (current_token + 1 < token_length && 
        myTokens[current_token].type == TOKEN_ID &&
        myTokens[current_token+1].type == TOKEN_OPERATOR){
        ParseTreeNode *assignment = parse_assignment();
        add_child(node, assignment);
        if(!assignment){
            fprintf(stderr, "Error: parsing assignment statement %d\n", 
                myTokens[current_token].line);
            return NULL;
        }

        return node;
    } 

    // TOKEN PRINT
    if (current_token < token_length && 
        myTokens[current_token].type == TOKEN_ID &&
        strcmp(myTokens[current_token].value, "print")==0){
        ParseTreeNode *output = parse_output_statement();
        add_child(node, output);
        if(!output){
            fprintf(stderr, "Error: parsing output statement %d\n", 
                myTokens[current_token].line);
            return NULL;
        }

        return node;
    } 

    // TOKEN RETURN
    if (current_token < token_length && 
        myTokens[current_token].type == TOKEN_RETURN){
        ParseTreeNode *return_statement = parse_return_statement();
        add_child(node, return_statement);
        if(!return_statement){
            fprintf(stderr, "Error: parsing output statement %d\n", 
                myTokens[current_token].line);
            return NULL;
        }

        return node;
    } 

    // TOKEN FUNCTION
    if (current_token < token_length && 
        ((myTokens[current_token+1].type == TOKEN_FUNCTION &&
        myTokens[current_token].type == TOKEN_DATATYPE)||
        myTokens[current_token].type == TOKEN_FUNCTION)
        ){
        ParseTreeNode *function_statement = parse_function_statement();
        add_child(node, function_statement);
        if(!function_statement){
            fprintf(stderr, "Error: parsing output statement %d\n", 
                myTokens[current_token].line);
            return NULL;
        }

        return node;
    } 

    //TOKEN DECLARATION
    if (current_token < token_length && myTokens[current_token].type == TOKEN_DATATYPE) {
        ParseTreeNode *declaration = parse_declaration();
        add_child(node, declaration);
        if (!declaration) {
            fprintf(stderr, "Error: parsing declaration statement at line %d\n", 
                myTokens[current_token].line);
            return NULL;
        }
        return node;
    }

    // TOKEN INPUT
    if (current_token < token_length && 
        (myTokens[current_token].type == TOKEN_DATATYPE || 
        (myTokens[current_token].type == TOKEN_ID && strcmp(myTokens[current_token].value, "input") == 0))) {
        ParseTreeNode *input_stmt = parse_input_statement();
        add_child(node, input_stmt);
        if (!input_stmt) {
            fprintf(stderr, "Error: Parsing input statement at line %d\n", myTokens[current_token].line);
            return NULL;
        }
        return node;
    }

}


ParseTreeNode *parse_assignment() {
    ParseTreeNode *node = create_assignment_node();

    // Parse first identifier
    ParseTreeNode *identifier_node = parse_identifier();
    add_child(node, identifier_node);

    if(current_token < token_length && myTokens[current_token].type==TOKEN_OPERATOR && (myTokens[current_token].value[0] == '=' && myTokens[current_token].value[1] != '=' || (myTokens[current_token].value[0] != '=' && myTokens[current_token].value[1] == '=')) )
        add_child(node, (check_create_advance(TOKEN_OPERATOR, "Equals")));
    
    // Parse expression
    ParseTreeNode *expression = parse_exp();
    add_child(node, expression);
    if(!expression){
        // fprintf(stderr, "Error: parsing expression %d\n", 
        //         myTokens[current_token].line);
        return NULL;
    }

    // Expect semicolon at end
    if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
        add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    } else {
        fprintf(stderr, "Error: Expected semicolon at end of variable declaration at line %d\n", 
                myTokens[current_token].line);
        return NULL;
    }        

    return node;
}

// <identifier> ::= identifier token
ParseTreeNode *parse_identifier() {
    ParseTreeNode *node = create_identifier_node();

    if (myTokens[current_token].type == TOKEN_ID) {
        add_child(node, check_create_advance(TOKEN_ID, "IDENTIFIER"));
    } else {
        fprintf(stderr, "Error: Expected data type at line %d\n", myTokens[current_token].line);
        return NULL;
    }

    return node;
}

ParseTreeNode *parse_exp() {
    ParseTreeNode *node = create_exp_node();

    // Parse the first term
    ParseTreeNode *term_node = parse_term();
    if (!term_node) {
        // fprintf(stderr, "Error: Expected term at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }
    add_child(node, term_node);

    // Parse additional terms connected by "+" or "-"
    while (current_token < token_length && 
           (myTokens[current_token].type == TOKEN_OPERATOR) && 
           (myTokens[current_token].value[0] == '+' || myTokens[current_token].value[0] == '-')) {
        // Match the operator
        ParseTreeNode *operator_node = check_create_advance(TOKEN_OPERATOR, "Operator");
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


ParseTreeNode *parse_term() {
    ParseTreeNode *node = create_term_node();

    // Parse the first factor
    ParseTreeNode *power_node = parse_power();
    if (!power_node) {
        // fprintf(stderr, "Error: Expected factor at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }
    add_child(node, power_node);

    // Parse additional factors connected by "*" or "/"
    while (current_token < token_length && 
           (myTokens[current_token].type == TOKEN_OPERATOR) && 
           (myTokens[current_token].value[0] == '*' || myTokens[current_token].value[0] == '/')) {
        // Match the operator
        ParseTreeNode *operator_node = check_create_advance(TOKEN_OPERATOR, "Operator");
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

ParseTreeNode *parse_power() {
    ParseTreeNode *node = create_power_node();

    // Parse the first factor
    ParseTreeNode *factor_node = parse_factor();
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
        ParseTreeNode *operator_node = check_create_advance(TOKEN_OPERATOR, "Operator");
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

ParseTreeNode *parse_factor() {
    ParseTreeNode *node = create_factor_node();

    if (current_token >= token_length) {
        // fprintf(stderr, "Error: Unexpected end of input at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }

    // Check for "(" <expression> ")"
    if (myTokens[current_token].type == TOKEN_LPAREN) {
        add_child(node, check_create_advance(TOKEN_LPAREN, "Left Parenthesis"));

        ParseTreeNode *exp_node = parse_exp();
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
        ParseTreeNode *constant = parse_constant();
        add_child(node, constant);
    }
    // Check for an identifier
    else if (myTokens[current_token].type == TOKEN_ID) {
        add_child(node, check_create_advance(TOKEN_ID, "Identifier"));
    }
    // Unexpected token
    else {
        fprintf(stderr, "Error: Unexpected token '%s' at Line: %d\n", 
                myTokens[current_token].value, myTokens[current_token].line);
        return NULL;
    }

    return node;
}


// conditional statement
ParseTreeNode *parse_conditional() {
    ParseTreeNode *node = create_conditional_node();

    // expect if token
    add_child(node, (check_create_advance(TOKEN_IF, "If")));

    // expect left parenthesis token
    add_child(node, (check_create_advance(TOKEN_LPAREN, "Left parenthesis")));

    // expect bool expression
    ParseTreeNode *bool_expression_node = parse_bool_expression();
    add_child(node, bool_expression_node);
    if(!bool_expression_node){
        fprintf(stderr, "Error: parsing expression %d\n", 
        myTokens[current_token].line);
        return NULL;
    }

    // expect a right parenthesis
    add_child(node, (check_create_advance(TOKEN_RPAREN, "Right parenthesis")));



    ParseTreeNode *if_body = parse_body();
    add_child(node, if_body);

    while (current_token < token_length && myTokens[current_token].type == TOKEN_ELSE) {
        ParseTreeNode *else_body = parse_else();
        add_child(node, else_body);
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

ParseTreeNode *parse_else(){
    ParseTreeNode *node = create_else_node();
    add_child(node, check_create_advance(TOKEN_ELSE, "Else"));

    if (current_token < token_length && myTokens[current_token].type == TOKEN_IF) {
        ParseTreeNode *if_statement = parse_conditional();
        add_child(node, if_statement);
    } else {
        ParseTreeNode *else_block = parse_body();
        add_child(node, else_block);
    }
    return node;

}

// <bool-expression>
ParseTreeNode *parse_bool_expression()
{
    ParseTreeNode  *node = create_bool_expression_node();

    // Parse the first factor
    ParseTreeNode *bool_term_node = parse_bool_term();
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
        ParseTreeNode *operator_node =  check_create_advance(TOKEN_OPERATOR, "Or");
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
ParseTreeNode *parse_bool_term()
{
    ParseTreeNode  *node = create_bool_term_node();

    // Parse the first factor
    ParseTreeNode *bool_factor_node = parse_bool_factor();
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
        ParseTreeNode *operator_node =  check_create_advance(TOKEN_OPERATOR, "And");
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
ParseTreeNode *parse_bool_factor()
{
    ParseTreeNode  *node = create_bool_factor_node();

    if (current_token >= token_length) {
        // fprintf(stderr, "Error: Unexpected end of input at Line: %d\n", myTokens[current_token].line);
        return NULL;
    }
    if (myTokens[current_token].type == TOKEN_OPERATOR && strcmp(myTokens[current_token].value, "not") == 0) {
        add_child(node, check_create_advance(TOKEN_OPERATOR, "Not"));
        
        ParseTreeNode *bool_factor_node = parse_bool_factor();
        if (!bool_factor_node) {
            fprintf(stderr, "Error: Expected boolean factor after 'not' at Line: %d\n", myTokens[current_token].line);
            return NULL;
        }
        add_child(node, bool_factor_node);
    }
    // Check for "(" <expression> ")"
    else if (myTokens[current_token].type == TOKEN_LPAREN) {
        add_child(node, check_create_advance(TOKEN_LPAREN, "Left Parenthesis"));

        ParseTreeNode *bool_expression_node = parse_bool_expression();
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
        ParseTreeNode *rel_expression = parse_rel_expression();    
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
                myTokens[current_token].value, myTokens[current_token].line);
        return NULL;
    }

    return node;
}


ParseTreeNode *parse_rel_expression()
{
    ParseTreeNode *node = create_rel_expression_node();
    
    // Parse first expression
    ParseTreeNode *left_expr_node = parse_exp();
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
        ParseTreeNode *rel_op_node = check_create_advance(TOKEN_OPERATOR, myTokens[current_token].value);
        add_child(node, rel_op_node);
    }
    else
    {
        return NULL; // Error: Expected relational operator
    }
    
    // Parse second expression
    ParseTreeNode *right_expr_node = parse_exp();
    if (!right_expr_node) return NULL;
    add_child(node, right_expr_node);
    
    return node;
}


//Output statement  [ADDED]
// Parse print statement: "print" "(" <print-expression> ")" ";"
ParseTreeNode *parse_output_statement() {
    ParseTreeNode *node = create_output_statement_node();
    
    // Expect "print" keyword
    if (current_token < token_length && myTokens[current_token].type == TOKEN_ID) {
        add_child(node, check_create_advance(TOKEN_ID, "Print"));
    } else {
        fprintf(stderr, "Error: Expected 'print' keyword at line %d\n", 
                myTokens[current_token].line);
        return NULL;
    }
    
    // Expect left parenthesis
    if (current_token < token_length && myTokens[current_token].type == TOKEN_LPAREN) {
        add_child(node, check_create_advance(TOKEN_LPAREN, "Left Parenthesis"));
    } else {
        fprintf(stderr, "Error: Expected '(' after print at line %d\n", 
                myTokens[current_token].line);
        return NULL;
    }
    
    // Parse print expression
    ParseTreeNode *print_expr = parse_print_expression();
    if (!print_expr) {
        fprintf(stderr, "Error: Invalid print expression at line %d\n", 
                myTokens[current_token].line);
        return NULL;
    }
    add_child(node, print_expr);
    
    // Expect right parenthesis
    if (current_token < token_length && myTokens[current_token].type == TOKEN_RPAREN) {
        add_child(node, check_create_advance(TOKEN_RPAREN, "Right Parenthesis"));
    } else {
        fprintf(stderr, "Error: Expected ')' after print expression at line %d\n", 
                myTokens[current_token].line);
        return NULL;
    }
    
    // Expect semicolon
    if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
        add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    } else {
        fprintf(stderr, "Error: Expected ';' at end of print statement at line %d\n", 
                myTokens[current_token].line);
        return NULL;
    }
    
    return node;
}

// Parse print expression: ["] <expression> ["] [ "," <print-expression>]*  [ADDED]
ParseTreeNode *parse_print_expression() {
    ParseTreeNode *node = create_node("Print Expression");
    
    // Check for opening quote
    if (current_token < token_length && myTokens[current_token].type == TOKEN_STRING) {
        ParseTreeNode *string = parse_constant();
        add_child(node, string);
    }
    else if(current_token < token_length && myTokens[current_token].type == TOKEN_ID && myTokens[current_token+1].type == TOKEN_LPAREN){
        ParseTreeNode *function_call = parse_function_call();
        add_child(node, function_call);
    }
    // Parse expression
    else{
        ParseTreeNode *expr = parse_exp();
        if (!expr) {
            return NULL;
        }
        add_child(node, expr);
    }
    
    // Check for comma and additional print expressions
    while (current_token < token_length && myTokens[current_token].type == TOKEN_COMMA) {
        add_child(node, check_create_advance(TOKEN_COMMA, "Comma"));
        
        ParseTreeNode *next_expr = parse_print_expression();
        if (!next_expr) {
            return NULL;
        }
        add_child(node, next_expr);
    }
    
    return node;
}



ParseTreeNode *parse_return_statement() {
    ParseTreeNode *node = create_return_statement_node();

    // Expect "return" keyword
    if (current_token < token_length && myTokens[current_token].type == TOKEN_RETURN) {
        add_child(node, check_create_advance(TOKEN_RETURN, "Return"));
    } else {
        fprintf(stderr, "Error: Expected 'print' keyword at line %d\n", 
                myTokens[current_token].line);
        return NULL;
    }

    ParseTreeNode *expression = parse_exp();
    add_child(node, expression);

    add_child(node, check_create_advance(TOKEN_SEMI, "Semi"));
    
    return node;
}

ParseTreeNode *parse_function_statement() {
    ParseTreeNode *node = create_function_statement_node();

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
    ParseTreeNode *identifier = parse_identifier();
    add_child(node, identifier);

    // expect a left parenthesis
    add_child(node, check_create_advance(TOKEN_LPAREN, "Left parenthesis"));

    // expect a parameter list
    ParseTreeNode *parameter_list = parse_parameter_list();
    add_child(node, parameter_list);

    add_child(node, check_create_advance(TOKEN_RPAREN, "Right_Parenthesis"));

    if (current_token < token_length && myTokens[current_token].type == TOKEN_LBRACE) {
        ParseTreeNode *body = parse_body();
        add_child(node, body);
    } else if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
        add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    } else {
        recover();
    }
    return node;

}

ParseTreeNode *parse_function_call()
{
    ParseTreeNode *node = create_function_call_node();

    // Expect identifier
    ParseTreeNode *identifier = parse_identifier();
    add_child(node, identifier);

    // expect a left parenthesis
    add_child(node, check_create_advance(TOKEN_LPAREN, "Left parenthesis"));

    // expect a expression
    ParseTreeNode *expression = parse_exp();
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

ParseTreeNode *parse_parameter_list() {
    ParseTreeNode *node = create_parameter_list_node();

    
    if (current_token < token_length && myTokens[current_token].type == TOKEN_RPAREN) {
        // Empty parameter list
    } else if (current_token < token_length && myTokens[current_token].type == TOKEN_DATATYPE && myTokens[current_token].value[0] == 'v') {
            add_child(node, check_create_advance(TOKEN_VOID, "VOID"));
    } else {
        if (current_token < token_length && (myTokens[current_token].type == TOKEN_DATATYPE)) {
            ParseTreeNode *datatype = parse_datatype();
            add_child(node, datatype);

            ParseTreeNode *identifier_node = parse_identifier();
            add_child(node, identifier_node);

            while (current_token < token_length && myTokens[current_token].type == TOKEN_COMMA) {
                add_child(node, check_create_advance(TOKEN_COMMA, "Comma"));

                if (current_token < token_length && (myTokens[current_token].type == TOKEN_DATATYPE)) {
                    add_child(node, check_create_advance(TOKEN_DATATYPE, "Datatype"));

                    ParseTreeNode *identifier_node = parse_identifier();
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

ParseTreeNode *parse_datatype() {
    ParseTreeNode *node = create_datatype_node();
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

ParseTreeNode *parse_body(){
    ParseTreeNode *node = create_body_node();
    add_child(node, check_create_advance(TOKEN_LBRACE, "Left Brace"));

    while (current_token < token_length && myTokens[current_token].type != TOKEN_RBRACE) {
        ParseTreeNode *statements = parse_statements();
        if (statements != NULL) {
            add_child(node, statements);
        } else {
            recover();
            if (current_token >= token_length || 
                myTokens[current_token].type == TOKEN_RBRACE) {
                break;
            }
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


ParseTreeNode *parse_constant(){
    ParseTreeNode *node = create_constant_node();
    
    if(myTokens[current_token].type==TOKEN_STRING){
        add_child(node, check_create_advance(TOKEN_STRING, "String"));
    }
    else if(myTokens[current_token].type==TOKEN_NUMBER){
        add_child(node, check_create_advance(TOKEN_NUMBER, "Number"));
    }

    return node;
}



// Implement create functions for each non-terminal
ParseTreeNode *create_program_node() {
    return create_node("Program");
}

ParseTreeNode *create_statements_node() {
    return create_node("Statements");
}

ParseTreeNode *create_statement_node() {
    return create_node("Statement");
}

ParseTreeNode *create_assignment_node() {
    return create_node("Assignment");
}

ParseTreeNode *create_identifier_node() {
    return create_node("Identifier");
}

ParseTreeNode *create_exp_node() {
    return create_node("Expression");
}

ParseTreeNode *create_term_node() {
    return create_node("Term");
}

ParseTreeNode *create_power_node() {
    return create_node("Power");
}

ParseTreeNode *create_factor_node() {
    return create_node("Factor");
}

ParseTreeNode *create_conditional_node() {
    return create_node("Conditional");
}

ParseTreeNode *create_bool_expression_node() {
    return create_node("Bool Expression");
}

ParseTreeNode *create_bool_term_node() {
    return create_node("Bool Term");
}

ParseTreeNode *create_bool_factor_node() {
    return create_node("Bool Factor");
}

ParseTreeNode *create_body_node() {
    return create_node("Body");
}

ParseTreeNode *create_output_statement_node() {
    return create_node("Output");
}

ParseTreeNode *create_parse_print_expression_node()
{
    return create_node("Print");
}

ParseTreeNode *create_return_statement_node()
{
    return create_node("Return");
}

ParseTreeNode *create_else_node()
{
    return create_node("Else");
}

ParseTreeNode *create_function_statement_node()
{
    return create_node("Function");
}

ParseTreeNode *create_parameter_list_node()
{
    return create_node("Parameter list");
}

ParseTreeNode *create_datatype_node()
{
    return create_node("Datatype");
}

ParseTreeNode *create_rel_expression_node()
{
    return create_node("Relational Expression");
}

ParseTreeNode *create_constant_node()
{
    return create_node("Constant");
}

ParseTreeNode * create_function_call_node()
{
    return create_node("Function Call");
}

// Function to allocate and initialize a new ParseTreeNode
ParseTreeNode *create_node(const char *name) {
    ParseTreeNode *node = malloc(sizeof(ParseTreeNode));
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
ParseTreeNode *check_create_advance(TokenType type, const char* node_name) {
    printf("Parsing token: %-10s %-10s Line: %d\n", getTokenType(myTokens[current_token].type), myTokens[current_token].value , myTokens[current_token].line);
    ParseTreeNode *node = create_node(node_name);
    node->token = malloc(sizeof(token));
    if (!node->token) {
        fprintf(stderr, "Error: Memory allocation failed in match_and_create_node\n");
        recover();
    }
    *node->token = myTokens[current_token];

    if (current_token < token_length && myTokens[current_token].type == type) {
        current_token++;
    } else {
        printf("Unexpected token %s\n", getTokenType(type));
        recover();
    }
    return node;
}

//added
ParseTreeNode *create_declaration_node() {
    return create_node("Declaration Statement");
}

ParseTreeNode *parse_declaration() {
    ParseTreeNode *node = create_declaration_node();

    // Expect datatype
    if (current_token < token_length && myTokens[current_token].type == TOKEN_DATATYPE) {
        add_child(node, check_create_advance(TOKEN_DATATYPE, "Datatype"));
    } else {
        fprintf(stderr, "Error: Expected datatype at line %d\n", myTokens[current_token].line);
        return NULL;
    }

    // 🔹 Replace single identifier parsing with `parse_ident_list()`
    ParseTreeNode *ident_list = parse_ident_list();
    add_child(node, ident_list);
    if (!ident_list) {
        fprintf(stderr, "Error: Expected identifier list at line %d\n", myTokens[current_token].line);
        return NULL;
    }

    // Check for assignment (`=`) in `dec-assign-stmt`
    if (current_token < token_length && myTokens[current_token].type == TOKEN_OPERATOR &&
        strcmp(myTokens[current_token].value, "=") == 0) {
        add_child(node, check_create_advance(TOKEN_OPERATOR, "Equals"));

        ParseTreeNode *expression = parse_exp();
        add_child(node, expression);
        if (!expression) {
            fprintf(stderr, "Error: Invalid expression in declaration at line %d\n", myTokens[current_token].line);
            return NULL;
        }
    }

    // Expect semicolon (`;`)
    if (current_token < token_length && myTokens[current_token].type == TOKEN_SEMI) {
        add_child(node, check_create_advance(TOKEN_SEMI, "Semicolon"));
    } else {
        fprintf(stderr, "Error: Missing semicolon at end of declaration at line %d\n", myTokens[current_token].line);
        return NULL;
    }

    return node;
}


void recover() {
    panic_mode = true;
    while (current_token < token_length) {

        // checkpoint to be deleted
        if (myTokens[current_token].type == TOKEN_SEMI){
            current_token++;            
            return;
        }

        // Synchronize on statement/declaration boundaries
        if (myTokens[current_token].type == TOKEN_SEMI ||
            myTokens[current_token].type == TOKEN_RBRACE ||
            myTokens[current_token].type == TOKEN_BOOL ||
            myTokens[current_token].type == TOKEN_FOR ||
            myTokens[current_token].type == TOKEN_WHILE ||
            myTokens[current_token].type == TOKEN_IF) {
                
            // checkpoint to be deleted
            current_token++;
            return;
        }
        current_token++;
    }
}

void add_child(ParseTreeNode *parent, ParseTreeNode *child) {
    parent->num_children++;
    ParseTreeNode **new_children = realloc(parent->children,  parent->num_children * sizeof(ParseTreeNode *));
    if (!new_children) {
        fprintf(stderr, "Error: Memory allocation failed in add_child\n");
        recover(); 
    }
    parent->children = new_children;
    parent->children[parent->num_children - 1] = child;
}


void print_parse_tree(ParseTreeNode *node, int indent_level) {
    if (node == NULL) {
        return;
    }

    print_indent(indent_level);

    // Check if the node is a terminal node (has a token)
    if (node->token != NULL) {
        // If it's a literal, print the token type and lexeme
        if (node->token->type == TOKEN_NUMBER ||
            node->token->type == TOKEN_ID ||
            node->token->type == TOKEN_STRING ||
            node->token->type == TOKEN_OPERATOR ||
            node->token->type == TOKEN_DATATYPE) {
                // Only have a single set of quotations for String literals
                if (node->token->type == TOKEN_STRING) {
                    fprintf(output_file, "%s: %s", getTokenType(node->token->type), node->token->value);
                } else {
                    fprintf(output_file, "%s: \"%s\"", getTokenType(node->token->type), node->token->value);
                }
        }
        // Otherwise, just print the token type
        else {
            fprintf(output_file, "%s", getTokenType(node->token->type));
        }
    }
    // If it's not a terminal node, print the node name and recurse
    else {
        fprintf(output_file, "%s(", node->name);

        // Recursively print the children
        if (node->num_children > 0) {
            fprintf(output_file, "\n");
            for (int i = 0; i < node->num_children; i++) {
                print_parse_tree(node->children[i], indent_level + 1);
                if (i < node->num_children - 1) {
                    fprintf(output_file, ",\n");
                }
            }
            fprintf(output_file, "\n");
            print_indent(indent_level);
        }
        fprintf(output_file, ")");
    }
}

void free_parse_tree(ParseTreeNode *node) {
    if (!node) return;
    
    for (int i = 0; i < node->num_children; i++) {
        free_parse_tree(node->children[i]);
    }
    
    free(node->token);
    free(node->name);
    free(node->children);
    free(node);
}

void print_indent(int indent_level) {
    for (int i = 0; i < indent_level; i++) {
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


    