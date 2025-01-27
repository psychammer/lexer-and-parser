#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "AST.h"
#include "token.h"
#include "scope.h"
#include "parser.h"

typedef struct PARSER_STRUCT
{
    lexer *lexer;
    token *current_token;
    token *prev_token;
    scope_T *scope;
    AST_T *input_expression;
} parser_T;

parser_T *init_parser(lexer *lexer);

void parser_eat(parser_T *parser, int token_type);

AST_T *parser_parse(parser_T *parser, scope_T *scope);

AST_T *parser_parse_statement(parser_T *parser, scope_T *scope);

AST_T *parser_parse_statements(parser_T *parser, scope_T *scope);

AST_T *parser_parse_expression(parser_T *parser, scope_T *scope);

AST_T *parser_parse_factor(parser_T *parser, scope_T *scope);

AST_T *parser_parse_power(parser_T *parser, scope_T *scope);

AST_T *parser_parse_expr(parser_T *parser, scope_T *scope);

AST_T *parser_parse_term(parser_T *parser, scope_T *scope);

AST_T *parser_parse_function_call(parser_T *parser, scope_T *scope);

AST_T *parser_parse_variable_definition(parser_T *parser, scope_T *scope);

AST_T *parser_parse_function_definition(parser_T *parser, scope_T *scope);

AST_T *parser_parse_variable(parser_T *parser, scope_T *scope);

AST_T *parser_parse_string(parser_T *parser, scope_T *scope);

AST_T *parser_parse_id(parser_T *parser, scope_T *scope);

AST_T *parser_parse_function_call(parser_T *parser, scope_T *scope);

AST_T *parser_parse_assignment(parser_T *parser, scope_T *scope);

AST_T *parser_parse_declaration(parser_T *parser, scope_T *scope);

AST_T *parser_parse_number(parser_T *parser, scope_T *scope);

AST_T *parser_parse_input_statement(parser_T *parser, scope_T *scope);

// Added
// AST_T *parser_parse_bool_expression(parser_T *parser, scope_T *scope);

// AST_T *parser_parse_bool_term(parser_T *parser, scope_T *scope);

// AST_T *parser_parse_bool_factor(parser_T *parser, scope_T *scope);

// AST_T *parser_parse_rel_expression(parser_T *parser, scope_T *scope);

parser_T *init_parser(lexer *lexer)
{
    parser_T *parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = token_buffer(lexer);
    parser->prev_token = parser->current_token;
    parser->scope = init_scope();
    return parser;
}

char peak(parser_T *parser)
{
    return parser->lexer->content[parser->lexer->i + 1];
}

void parser_eat(parser_T *parser, int token_type)
{

    if (parser->current_token->type == token_type)
    {
        parser->prev_token = parser->current_token;
        parser->current_token = token_buffer(parser->lexer);
    }
    else
    {
        printf("Unexpected token '%s', with type '%d'",
               parser->current_token->value,
               parser->current_token->type);
    }
}

AST_T *parser_parse(parser_T *parser, scope_T *scope) // returns the whole AST tree
{
    return parser_parse_statements(parser, scope);
}

AST_T *parser_parse_string(parser_T *parser, scope_T *scope)
{
    AST_T *ast_string = init_ast(AST_STRING);
    ast_string->string_value = parser->current_token->value;

    parser_eat(parser, TOKEN_STRING);

    ast_string->scope = scope;

    return ast_string;
}

AST_T *parser_parse_expr(parser_T *parser, scope_T *scope)
{
    char *variable_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);

    switch (parser->current_token->type)
    {
    case TOKEN_STRING:
        return parser_parse_string(parser, scope);
    case TOKEN_OPERATOR:
        if (*(parser->current_token->value) == '=')
        {
            parser_eat(parser, TOKEN_OPERATOR);
            AST_T *assingment_stmt = parser_parse_assignment(parser, scope);
            assingment_stmt->variable_name = variable_name;
            parser_eat(parser, TOKEN_SEMI);
            return assingment_stmt;
        }
    }

    return init_ast(AST_NOOP);
}

// <assignment-stmt>
AST_T *parser_parse_assignment(parser_T *parser, scope_T *scope)
{
    AST_T *assignment_statement = init_ast(AST_ASSIGNMENT);

    assignment_statement->assignment_identifier = parser->prev_token->value; // get assignment identifier

    assignment_statement->assignment_expression = parser_parse_expression(parser, scope);

    return assignment_statement;
}

// <declaration-stmt>
AST_T *parser_parse_declaration(parser_T *parser, scope_T *scope)
{
    // data-type
    int declaration_statement_datatype = parser->current_token->type; // store the datatype
    parser_eat(parser, TOKEN_DATATYPE);                               // expect a datatype

    // identifier or <dec-assign-stmt>
    char *declaration_statement_identifier = parser->current_token->value; // store the identifier
    parser_eat(parser, TOKEN_ID);                                          // expect an identifier

    AST_T *declaration_statement = init_ast(AST_DECLARATION); //  make a node for declaration_statement
    // <dec-assign-stmt>
    if (parser->current_token->type == TOKEN_OPERATOR && strcmp(parser->current_token->value, "=") == 0)
    {
        AST_T *declaration_assignment_statement = parser_parse_assignment(parser, scope);

        declaration_statement->dec_assign_stmt = declaration_assignment_statement; // store the declaration_assignment_statement node into declaration_statement node
    }

    declaration_statement->datatype = declaration_statement_datatype;
    declaration_statement->identifier = declaration_statement_identifier;

    // [<ident-list>]
    if (parser->current_token->type == TOKEN_COMMA)
    {
        printf("hi");
    }

    return declaration_statement;
}

// Added
// /* bool-factor */
// AST_T *parser_parse_bool_factor(parser_T *parser, scope_T *scope)
// {
//     AST_T *node = init_ast(AST_BOOL_FACTOR);

//     // Handle true/false literals
//     if (parser->current_token->type == TOKEN_BOOL)
//     {
//         node->bool_literal_value = strcmp(parser->current_token->value, "true") == 0;
//         parser_eat(parser, TOKEN_BOOL);
//         return node;
//     }

//     // Handle comparisons (e.g., x > 5)
//     AST_T *left = parser_parse_rel_expression(parser, scope);

//     if (parser->current_token->type == TOKEN_OPERATOR)
//     {
//         AST_T *comparison = init_ast(AST_REL_EXPRESSION);
//         comparison->rel_expr_left = left;
//         comparison->rel_operator = parser->current_token->value;
//         parser_eat(parser, TOKEN_OPERATOR);
//         comparison->rel_expr_right = parser_parse_rel_expression(parser, scope);
//         return comparison;
//     }

//     return left;
// }

// // <bool-term>
// AST_T *parser_parse_bool_term(parser_T *parser, scope_T *scope)
// {
//     AST_T *node = parser_parse_bool_factor(parser, scope);
//     // AST_T *node = init_ast(AST_BOOL_TERM);
//     while (parser->current_token != (void *)0 &&
//            strcmp(parser->current_token->value, "and") == 0)
//     {
//         AST_T *bool_term = init_ast(AST_BOOL_TERM);
//         bool_term->bool_term_left = node;
//         bool_term->bool_term_operator = strdup("and"); // Use strdup to allocate new memory
//         parser_eat(parser, TOKEN_KEYWORD);             // "and" keyword
//         bool_term->bool_term_right = parser_parse_bool_factor(parser, scope);
//         node = bool_term;
//     }

//     return node;
// }

// // <bool-expression>
// AST_T *parser_parse_bool_expression(parser_T *parser, scope_T *scope)
// {
//     parser_eat(parser, TOKEN_LPAREN);
//     // AST_T *node = init_ast(AST_BOOL_EXPRESSION);
//     AST_T *node = parser_parse_bool_term(parser, scope);
//     while (parser->current_token != (void *)0 &&
//            strcmp(parser->current_token->value, "or") == 0)
//     // if (parser->current_token != (void *)0)
//     {
//         printf("here?");
//         AST_T *bool_expr = init_ast(AST_BOOL_EXPRESSION);
//         bool_expr->bool_expr_left = node;
//         bool_expr->bool_expr_operator = strdup("or"); // Use strdup to allocate new memory
//         parser_eat(parser, TOKEN_KEYWORD);            // "or" keyword
//         bool_expr->bool_expr_right = parser_parse_bool_term(parser, scope);
//         node = bool_expr;
//     }

//     return node;
// }

// <conditional-stmt>
AST_T *parser_parse_conditional_statement(parser_T *parser, scope_T *scope)
{ /*
     AST_T* compound = init_ast(AST_COMPOUND);
     compound->scope = scope;
     compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));

     AST_T* conditional_stmt = init_ast(AST_CONDITIONAL);

     if (strcmp(parser->current_token->value, "if")==0) { // starting if stmt
         parser_eat(parser, TOKEN_KEYWORD);
         parser_eat(parser, TOKEN_LPAREN);

         conditional_stmt->conditional_condition = parser_parse_bool_expression(parser, scope);

         parser_eat(parser, TOKEN_RPAREN);

         parser_eat(parser, TOKEN_LBRACE);
         conditional_stmt->conditional_body = parser_parse_statements(parser, scope);
         parser_eat(parser, TOKEN_RBRACE);

         conditional_stmt->scope = scope;
         compound->compound_value[0] = conditional_stmt;
         compound->compound_size += 1;

         if (strcmp(parser->current_token->value, "else if")==0) { // if "if statement" is followed by else if
             while (strcmp(parser->current_token->value, "else if")==0){ // loop for multiple "else if"
                 parser_eat(parser, TOKEN_KEYWORD);
                 parser_eat(parser, TOKEN_LPAREN);

                 conditional_stmt->conditional_condition = parser_parse_bool_expression(parser, scope);

                 parser_eat(parser, TOKEN_RPAREN);

                 parser_eat(parser, TOKEN_LBRACE);
                 conditional_stmt->conditional_body = parser_parse_statements(parser, scope);
                 parser_eat(parser, TOKEN_RBRACE);

                 if (conditional_stmt){
                     compound->compound_size += 1;
                     compound->compound_value = realloc(compound->compound_value, compound->compound_size * sizeof(struct AST_STRUCT*));
                     compound->compound_value[compound->compound_size-1] = conditional_stmt;
                 }
             }
             if (strcmp(parser->current_token->value, "else")==0){ // if "else if" is followd by "else"
                 parser_eat(parser, TOKEN_KEYWORD);

                 parser_eat(parser, TOKEN_LBRACE);
                 conditional_stmt->conditional_body = parser_parse_statements(parser, scope);
                 parser_eat(parser, TOKEN_RBRACE);

                 if (conditional_stmt){
                     compound->compound_size += 1;
                     compound->compound_value = realloc(compound->compound_value, compound->compound_size * sizeof(struct AST_STRUCT*));
                     compound->compound_value[compound->compound_size-1] = conditional_stmt;
                 }
             }
         }
         else if (strcmp(parser->current_token->value, "else")==0){ // if "if statement" is followed by else
             parser_eat(parser, TOKEN_KEYWORD);

             parser_eat(parser, TOKEN_LBRACE);
             conditional_stmt->conditional_body = parser_parse_statements(parser, scope);
             parser_eat(parser, TOKEN_RBRACE);

             if (conditional_stmt){
                 compound->compound_size += 1;
                 compound->compound_value = realloc(compound->compound_value, compound->compound_size * sizeof(struct AST_STRUCT*));
                 compound->compound_value[compound->compound_size-1] = conditional_stmt;
             }
         }
     }
     return compound;
 */
}

AST_T *parser_parse_expression(parser_T *parser, scope_T *scope)
{
    // Make sure the returned expression is valid
    AST_T *expr = some_expression_parsing_logic(parser, scope);
    return expr;
}

// Function definitions
AST_T *parser_parse_input_statement(parser_T *parser, scope_T *scope)
{
    // Initialize input statement AST node
    AST_T *input_statement = init_ast(AST_INPUT);
    input_statement->scope = scope;

    // Eat the "input" keyword
    parser_eat(parser, TOKEN_KEYWORD); // "input"

    // Eat left parenthesis
    parser_eat(parser, TOKEN_LPAREN);

    // Parse the input expression (this could be a variable, function, etc.)
    input_statement->input_expression = parser_parse_expression(parser, scope);

    // Eat right parenthesis
    parser_eat(parser, TOKEN_RPAREN);

    // Eat semicolon
    parser_eat(parser, TOKEN_SEMI);

    return input_statement;
}

//<output-stmt>
AST_T *parser_parse_output_statement(parser_T *parser, scope_T *scope)
{
    // Initialize output statement AST node
    AST_T *output_statement = init_ast(AST_OUTPUT);
    output_statement->scope = scope;

    // Eat the "show" keyword
    parser_eat(parser, TOKEN_KEYWORD); // "show"

    // Eat left parenthesis
    parser_eat(parser, TOKEN_LPAREN);

    // Initialize array to store expressions
    output_statement->output_expressions = calloc(1, sizeof(struct AST_STRUCT *));
    output_statement->output_expressions_size = 0;

    // Parse first expression
    AST_T *expression = parser_parse_expression(parser, scope);
    output_statement->output_expressions[0] = expression;
    output_statement->output_expressions_size += 1;

    // Handle multiple expressions separated by commas
    while (parser->current_token->type == TOKEN_COMMA)
    {
        parser_eat(parser, TOKEN_COMMA);

        // Reallocate array to make space for new expression
        output_statement->output_expressions_size += 1;
        output_statement->output_expressions = realloc(
            output_statement->output_expressions,
            output_statement->output_expressions_size * sizeof(struct AST_STRUCT *));

        // Parse next expression and add it to array
        expression = parser_parse_expression(parser, scope);
        output_statement->output_expressions[output_statement->output_expressions_size - 1] = expression;
    }

    // Eat right parenthesis
    parser_eat(parser, TOKEN_RPAREN);

    // Eat semicolon
    parser_eat(parser, TOKEN_SEMI);

    return output_statement;
}

AST_T *parser_parse_keyword(parser_T *parser, scope_T *scope)
{
    if (parser->current_token->type == TOKEN_IF)
    {
        parser_eat(parser, TOKEN_KEYWORD);
        parser_parse_conditional_statement(parser, scope);
    }
}

// <stmt>
AST_T *parser_parse_statement(parser_T *parser, scope_T *scope)
{
    switch (parser->current_token->type)
    {
    case TOKEN_FUNCTION:
        return parser_parse_function_definition(parser, scope);
        break;
    case TOKEN_DATATYPE:
        return parser_parse_function_definition(parser, scope);
        break;
    case TOKEN_ID:
        return parser_parse_expr(parser, scope);
        break;
    case TOKEN_KEYWORD:
        return parser_parse_keyword(parser, scope);
        break;
    case TOKEN_NUMBER:
        return parser_parse_number(parser, scope);
        break;
    }

    return init_ast(AST_NOOP);
}

// <stmt-list> ... ROOT NODE
AST_T *parser_parse_statements(parser_T *parser, scope_T *scope)
{
    AST_T *compound = init_ast(AST_COMPOUND);
    compound->scope = scope;
    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT *));

    AST_T *ast_statement = parser_parse_statement(parser, scope);
    ast_statement->scope = scope;
    compound->compound_value[0] = ast_statement;
    compound->compound_size += 1;

    while (parser->current_token != (void *)0 && parser->current_token->type == TOKEN_SEMI)
    {
        parser_eat(parser, TOKEN_SEMI);

        AST_T *ast_statement = parser_parse_statement(parser, scope);

        if (ast_statement)
        {
            compound->compound_size += 1;
            compound->compound_value = realloc(
                compound->compound_value,
                compound->compound_size * sizeof(struct AST_STRUCT *));
            compound->compound_value[compound->compound_size - 1] = ast_statement;
        }
    }

    return compound;
}

// <iterative-stmt>
AST_T *parser_parse_iterative_stmt(parser_T *parser, scope_T *scope)
{
    AST_T *iterative_stmt = init_ast(AST_ITERATIVE);

    if (strcmp(parser->current_token->value, "while") == 0)
    {
        // Parse while loop
        parser_eat(parser, TOKEN_KEYWORD); // Consume while
        parser_eat(parser, TOKEN_LPAREN);  // Consume (

        // Parse condition
        iterative_stmt->iterative_condition = parser_parse_expression(parser, scope);

        parser_eat(parser, TOKEN_RPAREN); // Consume )

        // Parse loop bodyh
        parser_eat(parser, TOKEN_LBRACE); // Consume {
        iterative_stmt->iterative_body = parser_parse_statements(parser, scope);
        parser_eat(parser, TOKEN_RBRACE); // Consume }
    }
    else if (strcmp(parser->current_token->value, "for") == 0)
    {
        // Parse for loop
        parser_eat(parser, TOKEN_KEYWORD); // Consume for
        parser_eat(parser, TOKEN_LPAREN);  // Consume (

        // Parse init
        iterative_stmt->for_init = parser_parse_statement(parser, scope);
        parser_eat(parser, TOKEN_SEMI); // Consume ;

        // Parse condition
        iterative_stmt->iterative_condition = parser_parse_expression(parser, scope);
        parser_eat(parser, TOKEN_SEMI); // Consume ;

        // Parse increment
        iterative_stmt->for_increment = parser_parse_statement(parser, scope);
        parser_eat(parser, TOKEN_RPAREN); // Consume )

        // Parse loop body
        parser_eat(parser, TOKEN_LBRACE); // Consume {
        iterative_stmt->iterative_body = parser_parse_statements(parser, scope);
        parser_eat(parser, TOKEN_RBRACE); // Consume }
    }
    else
    {
        printf("Unexpected token in iterative statement: '%s'\n", parser->current_token->value);
        return init_ast(AST_NOOP);
    }

    iterative_stmt->scope = scope;

    return iterative_stmt;
}

AST_T *parser_parse_do_while(parser_T *parser, scope_T *scope)
{
    parser_eat(parser, TOKEN_DO); // Eat 'do'

    if (parser->current_token->type != TOKEN_LBRACE)
    {
        printf("Error: Expected '{'\n");
        exit(1);
    }
    parser_eat(parser, TOKEN_LBRACE); // Eat '{'

    AST_T *stmt_list = parser_parse_statements(parser, scope); // Parse the statement list

    if (parser->current_token->type != TOKEN_RBRACE)
    {
        printf("Error: Expected '}'\n");
        exit(1);
    }
    parser_eat(parser, TOKEN_RBRACE); // Eat '}'

    if (parser->current_token->type != TOKEN_WHILE)
    {
        printf("Error: Expected 'while'\n");
        exit(1);
    }
    parser_eat(parser, TOKEN_WHILE); // Eat 'while'

    if (parser->current_token->type != TOKEN_LPAREN)
    {
        printf("Error: Expected '('\n");
        exit(1);
    }
    parser_eat(parser, TOKEN_LPAREN); // Eat '('

    AST_T *bool_expr = parser_parse_bool_expression(parser, scope); // Parse the boolean expression

    if (parser->current_token->type != TOKEN_RPAREN)
    {
        printf("Error: Expected ')'\n");
        exit(1);
    }
    parser_eat(parser, TOKEN_RPAREN); // Eat ')'

    if (parser->current_token->type != TOKEN_SEMI)
    {
        printf("Error: Expected ';'\n");
        exit(1);
    }
    parser_eat(parser, TOKEN_SEMI); // Eat ';'

    AST_T *do_while_node = init_ast(AST_DO_WHILE);
    do_while_node->stmt_list = stmt_list;
    do_while_node->stmt_list_size = stmt_list->compound_size;
    do_while_node->bool_expr = bool_expr;

    return do_while_node;
}

AST_T *parser_parse_factor(parser_T *parser, scope_T *scope)
{
    AST_T *node = init_ast(AST_FACTOR);

    if (parser->prev_token != (void *)0 && parser->prev_token->type == TOKEN_NUMBER && parser->current_token->type == TOKEN_OPERATOR)
    {
        node->type = AST_FACTOR;
        node->number = atof(parser->prev_token->value);
    }
    else if (parser->current_token != (void *)0 && parser->current_token->type == TOKEN_LPAREN)
    {
        parser_eat(parser, TOKEN_LPAREN);
        node = parser_parse_expression(parser, scope);
        parser_eat(parser, TOKEN_RPAREN);
    }
    else if (parser->current_token != (void *)0 && parser->current_token->type == TOKEN_NUMBER)
    {
        node->type = AST_FACTOR;
        node->number = atof(parser->current_token->value);
        if (parser->current_token != (void *)0 && *(parser->current_token->value) != 5)
            ;
        parser_eat(parser, TOKEN_NUMBER);
    }
    else
    {
        printf("Unexpected token in factor: '%s'\n", parser->current_token->value);
    }
    return node;
}

AST_T *parser_parse_power(parser_T *parser, scope_T *scope)
{

    AST_T *node = parser_parse_factor(parser, scope);

    while (parser->current_token != (void *)0 && strlen(parser->current_token->value) == 2 && parser->current_token->value[0] == '*' && parser->current_token->value[1] == '*')
    {
        AST_T *new_node = calloc(1, sizeof(AST_T));
        new_node->type = AST_POWER;
        new_node->first_factor = node;
        new_node->p_operator = parser->current_token->value;
        parser_eat(parser, TOKEN_OPERATOR);
        new_node->second_factor = parser_parse_factor(parser, scope);
        node = new_node;
    }
    return node;
}

AST_T *parser_parse_term(parser_T *parser, scope_T *scope)
{
    AST_T *node = parser_parse_power(parser, scope);

    while (parser->current_token != (void *)0 && ((*parser->current_token->value) == '*' || (*parser->current_token->value) == '/'))
    {
        AST_T *new_node = calloc(1, sizeof(AST_T));
        new_node->type = AST_TERM;
        new_node->first_power = node;
        new_node->md_operator = parser->current_token->value; // Store operator string
        parser_eat(parser, parser->current_token->type);
        new_node->second_power = parser_parse_power(parser, scope);
        node = new_node;
    }
    return node;
}

AST_T *parser_parse_expression(parser_T *parser, scope_T *scope)
{
    AST_T *node = parser_parse_term(parser, scope);

    while (parser->current_token != (void *)0 && ((*parser->current_token->value) == '+' || (*parser->current_token->value) == '-'))
    {
        AST_T *new_node = calloc(1, sizeof(AST_T));
        new_node->type = AST_EXPRESSION;
        new_node->first_term = node;
        new_node->as_operator = parser->current_token->value; // Store operator string
        parser_eat(parser, parser->current_token->type);
        new_node->second_term = parser_parse_term(parser, scope);
        node = new_node;
    }
    return node;
}

AST_T *parser_parse_function_call(parser_T *parser, scope_T *scope)
{
    AST_T *function_call = init_ast(AST_FUNCTION_CALL);

    function_call->function_call_name = parser->prev_token->value;
    parser_eat(parser, TOKEN_LPAREN);

    function_call->function_call_arguments = calloc(1, sizeof(struct AST_STRUCT *));

    AST_T *ast_expr = parser_parse_expr(parser, scope);
    function_call->function_call_arguments[0] = ast_expr;
    function_call->function_call_arguments_size += 1;

    while (parser->current_token->type == TOKEN_COMMA)
    {
        parser_eat(parser, TOKEN_COMMA);

        AST_T *ast_expr = parser_parse_expr(parser, scope);
        function_call->function_call_arguments_size += 1;
        function_call->function_call_arguments = realloc(
            function_call->function_call_arguments,
            function_call->function_call_arguments_size * sizeof(struct AST_STRUCT *));
        function_call->function_call_arguments[function_call->function_call_arguments_size - 1] = ast_expr;
    }
    parser_eat(parser, TOKEN_RPAREN);

    function_call->scope = scope;

    return function_call;
}

AST_T *parser_parse_variable_definition(parser_T *parser, scope_T *scope)
{
    parser_eat(parser, TOKEN_ID); // var
    char *variable_definition_variable_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID); // var name
    parser_eat(parser, TOKEN_EQUALS);
    AST_T *variable_definition_value = parser_parse_expr(parser, scope);

    AST_T *variable_definition = init_ast(AST_VARIABLE_DEFINITION);
    variable_definition->variable_definition_variable_name = variable_definition_variable_name;
    variable_definition->variable_definition_value = variable_definition_value;

    variable_definition->scope = scope;

    return variable_definition;
}

AST_T *parser_parse_function_definition(parser_T *parser, scope_T *scope)
{
    AST_T *ast = init_ast(AST_FUNCTION_DEFINITION);

    if (parser->current_token->type == TOKEN_DATATYPE)
    {
        char *datatype = parser->current_token->value;
        ast->function_definition_datatype = calloc(
            strlen(datatype) + 1, sizeof(char));
        strcpy(ast->function_definition_datatype, datatype);

        parser_eat(parser, TOKEN_DATATYPE); // <datatype>
    }

    if (parser->current_token->type == TOKEN_FUNCTION)
    {
        printf("here?");
        parser_eat(parser, TOKEN_FUNCTION);
    } // TOKEN_FUNCTION

    char *function_name = parser->current_token->value;
    ast->function_definition_name = calloc(
        strlen(function_name) + 1, sizeof(char));
    strcpy(ast->function_definition_name, function_name);

    parser_eat(parser, TOKEN_ID); // function TOKEN_ID

    parser_eat(parser, TOKEN_LPAREN);

    char *function_definition_arg_datatype = parser->current_token->value; // arg datatype
    parser_eat(parser, TOKEN_DATATYPE);
    ast->function_definition_args =
        calloc(1, sizeof(struct AST_STRUCT *));

    AST_T *arg = parser_parse_variable(parser, scope);
    ast->function_definition_args_size += 1;
    ast->function_definition_args[ast->function_definition_args_size - 1] = arg;
    ast->function_definition_args[ast->function_definition_args_size - 1]->function_definition_arg_datatype = function_definition_arg_datatype;

    while (parser->current_token != (void *)0 && parser->current_token->type == TOKEN_COMMA)
    {
        parser_eat(parser, TOKEN_COMMA);

        function_definition_arg_datatype = parser->current_token->value;
        parser_eat(parser, TOKEN_DATATYPE);

        ast->function_definition_args_size += 1;

        ast->function_definition_args =
            realloc(
                ast->function_definition_args,
                ast->function_definition_args_size * sizeof(struct AST_STRUCT *));

        AST_T *arg = parser_parse_variable(parser, scope);
        ast->function_definition_args[ast->function_definition_args_size - 1] = arg;
        ast->function_definition_args[ast->function_definition_args_size - 1]->function_definition_arg_datatype = function_definition_arg_datatype;
    }

    parser_eat(parser, TOKEN_RPAREN);

    parser_eat(parser, TOKEN_LBRACE);

    ast->function_definition_body = parser_parse_statements(parser, scope);

    parser_eat(parser, TOKEN_RBRACE);

    ast->scope = scope;

    return ast;
}

AST_T *parser_parse_variable(parser_T *parser, scope_T *scope)
{
    char *token_value = parser->current_token->value;
    parser_eat(parser, TOKEN_ID); // var name or function call name

    if (parser->current_token->type == TOKEN_LPAREN)
        return parser_parse_function_call(parser, scope);

    AST_T *ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    ast_variable->scope = scope;

    return ast_variable;
}

AST_T *parser_parse_number(parser_T *parser, scope_T *scope)
{
    parser_eat(parser, TOKEN_NUMBER);

    AST_T *node = init_ast(AST_NOOP);
    if (parser->current_token->type == TOKEN_OPERATOR)
    {
        node = parser_parse_expression(parser, scope);
    }

    return node;
}

AST_T *parser_parse_id(parser_T *parser, scope_T *scope)
{
    if (strcmp(parser->current_token->value, "var") == 0)
    {
        return parser_parse_variable_definition(parser, scope);
    }
    else if (strcmp(parser->current_token->value, "function") == 0)
    {
        return parser_parse_function_definition(parser, scope);
    }
    else
    {
        return parser_parse_variable(parser, scope);
    }
}

AST_T *parser_parse_declaration_stmt(parser_T *parser, scope_T *scope)
{
}

#endif
