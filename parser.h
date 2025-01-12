#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "AST.h"
#include "token.h"

typedef struct PARSER_STRUCT
{
    lexer* lexer;
    token* current_token;
    token* prev_token;
} parser_T;

parser_T* init_parser(lexer* lexer);

void parser_eat(parser_T* parser, int token_type);

AST_T* parser_parse(parser_T* parser); // returns the whole AST tree

AST_T* parser_parse_statement(parser_T* parser);

AST_T* parser_parse_statements(parser_T* parser);

AST_T* parser_parse_expression(parser_T* parser);

AST_T* parser_parse_factor(parser_T* parser);

AST_T* parser_parse_term(parser_T* parser);

AST_T* parser_parse_function_call(parser_T* parser);

AST_T* parser_parse_variable_definition(parser_T* parser);

AST_T* parser_parse_variable(parser_T* parser);

AST_T* parser_parse_string(parser_T* parser);

AST_T* parser_parse_id(parser_T* parser);

AST_T* parser_parse_number(parser_T* parser);



parser_T* init_parser(lexer* lexer){
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = token_buffer(lexer);
    parser->prev_token = parser->current_token;
    return parser;
}

char peak(parser_T* parser){
    return parser->lexer->content[parser->lexer->i+1];
}

void parser_eat(parser_T* parser, int token_type)
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
                parser->current_token->type
        );
        exit(1);
    }

}

AST_T* parser_parse(parser_T* parser) // returns the whole AST tree
{
    return parser_parse_statements(parser);
}

AST_T* parser_parse_string(parser_T* parser)
{
    AST_T* ast_string = init_ast(AST_STRING);
    ast_string->string_value = parser->current_token->value;

    parser_eat(parser, TOKEN_STRING);

    return ast_string;
}

AST_T* parser_parse_expr(parser_T* parser)
{
    switch(parser->current_token->type)
    {
        case TOKEN_STRING: return parser_parse_string(parser);
    }

    return init_ast(AST_NOOP);
}

// <dec-assign-stmt>
AST_T* parser_parse_assignment(parser_T* parser)
{
    AST_T* declaration_assignment_statement = init_ast(AST_DECLARATION_ASSIGNMENT_STATEMENT); // create the node for dec-assign-stmt
    if(parser->current_token->type == TOKEN_ID){ // still has not expected a TOKEN_ID 
        parser_eat(parser, TOKEN_ID);
        declaration_assignment_statement->declaration_assignment_statement_identifier = parser->prev_token->value;
    }

    parser_eat(parser, TOKEN_OPERATOR); // expect an equal sign
    AST_T* declaration_assignment_statement_value = parser_parse_expr(parser); 

    // then we populate it in the ast with the stuff that we had just parsed
    declaration_assignment_statement->declaration_assignment_statement_value = declaration_assignment_statement_value;


    return declaration_assignment_statement;
}

// <declaration-stmt>
AST_T* parser_parse_declaration(parser_T* parser)
{
    // data-type
    int declaration_statement_datatype = parser->current_token->type; // store the datatype    
    parser_eat(parser, TOKEN_DATATYPE); // expect a datatype
    
    // identifier or <dec-assign-stmt>
    char* declaration_statement_identifier = parser->current_token->value; // store the identifier
    parser_eat(parser, TOKEN_ID); // expect an identifier
    

    AST_T* declaration_statement = init_ast(AST_DECLARATION); //  make a node for declaration_statement
    // <dec-assign-stmt>
    if(parser->current_token->type == TOKEN_OPERATOR && strcmp(parser->current_token->value, "=")==0 ){
        AST_T* declaration_assignment_statement = parser_parse_assignment(parser);

        
        declaration_statement->dec_assign_stmt = declaration_assignment_statement; // store the declaration_assignment_statement node into declaration_statement node
    }

    declaration_statement->datatype = declaration_statement_datatype;
    declaration_statement->identifier = declaration_statement_identifier;


    // [<ident-list>]
    if(parser->current_token->type == TOKEN_COMMA){
        printf("hi");
    }

    return declaration_statement;
}

// <bool-factor
AST_T* parser_parse_bool_factor(parser_T* parser)
{
    
}

// <bool-term>
AST_T* parser_parse_bool_term(parser_T* parser)
{

}

// <bool-expression>
AST_T* parser_parse_bool_expression(parser_T* parser)
{

}

// <conditional-stmt>
AST_T* parser_parse_conditional_statement(parser_T* parser)
{
    parser_eat(parser, TOKEN_LPAREN);
    
    parser_eat(parser, TOKEN_RPAREN);
    // if(strcmp(parser->current_token->value, "else")==0)
    // {
    //     parser_eat(parser, TOKEN_KEYWORD);
       
    // }

}

AST_T* parser_parse_keyword(parser_T* parser)
{
    if(parser->current_token->type==TOKEN_IF)
    {
        parser_eat(parser, TOKEN_KEYWORD);
        parser_parse_conditional_statement(parser);
    }
}

// <stmt>
AST_T* parser_parse_statement(parser_T* parser)
{
    switch(parser->current_token->type)
    {
        case TOKEN_DATATYPE: return parser_parse_declaration(parser); break;
        case TOKEN_ID:  return parser_parse_assignment(parser); break;
        case TOKEN_KEYWORD: return parser_parse_keyword(parser); break;
        case TOKEN_NUMBER: return parser_parse_number(parser); break;
    }

    return init_ast(AST_NOOP);
}

// <stmt-list> ... ROOT NODE
AST_T* parser_parse_statements(parser_T* parser)
{
    AST_T* compound = init_ast(AST_COMPOUND); // create a node for statements
    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*)); // allocate memory for an array of statements

    AST_T* ast_statement = parser_parse_statement(parser); // create a node for individual statement
    compound->compound_value[0] = ast_statement; // assign the single statement to the array of statements
    compound->compound_size += 1; // increase the size/number of array statements

    while (parser->current_token != (void*)0 && parser->current_token->type == TOKEN_SEMI){ // continue doing so after encountering a semi. Meaning there might be another statements
        parser_eat(parser, TOKEN_SEMI);

        AST_T* ast_statement = parser_parse_statement(parser);

        if (ast_statement)
        {
            compound->compound_size += 1; 
            compound->compound_value = realloc(compound->compound_value, 
                                            compound->compound_size *sizeof(struct AST_STRUCT*));
            compound->compound_value[compound->compound_size-1] = ast_statement;
        }
       
    }
    return compound;
}


AST_T* parser_parse_factor(parser_T* parser) {
    AST_T* node = calloc(1, sizeof(AST_T));

    if (parser->prev_token != (void*)0 && parser->prev_token->type == TOKEN_NUMBER) {
        node->type = AST_FACTOR;
        node->number = atof(parser->prev_token->value);
    } else if (parser->current_token != (void*)0 && parser->current_token->type == TOKEN_LPAREN) {
        parser_eat(parser, TOKEN_LPAREN);
        node = parser_parse_expression(parser);
        parser_eat(parser, TOKEN_RPAREN);
    } else if (parser->prev_token != (void*)0 && parser->current_token->type == TOKEN_NUMBER) {
        node->type = AST_FACTOR;
        node->number = atof(parser->current_token->value);
        parser_eat(parser, TOKEN_NUMBER);
    } 
    else {
        printf("Unexpected token in factor: '%s'\n", parser->current_token->value);
        exit(1);
    }
    return node;
}

AST_T* parser_parse_term(parser_T* parser) {
    AST_T* node = parser_parse_factor(parser);

    while (parser->current_token != (void*)0 && ((*parser->current_token->value) == '*' || (*parser->current_token->value) == '/')) {
        AST_T* new_node = calloc(1, sizeof(AST_T));
        new_node->type = AST_TERM;
        new_node->first_factor = node;
        new_node->md_operator = parser->current_token->value; // Store operator string
        parser_eat(parser, parser->current_token->type);
        new_node->second_factor = parser_parse_factor(parser);
        node = new_node;
    }
    return node;
}

AST_T* parser_parse_expression(parser_T* parser) {
    AST_T* node = parser_parse_term(parser);

    printf("%d", parser->lexer->i);

    while (parser->current_token != (void*)0 && ((*parser->current_token->value) == '+' || (*parser->current_token->value) == '-')) {
        AST_T* new_node = calloc(1, sizeof(AST_T));
        new_node->type = AST_EXPRESSION;
        new_node->first_term = node;
        new_node->as_operator = parser->current_token->value; // Store operator string
        parser_eat(parser, parser->current_token->type);
        new_node->second_term = parser_parse_term(parser);
        node = new_node;
    }
    return node;
    
}

AST_T* parser_parse_number(parser_T* parser) {
    parser_eat(parser, TOKEN_NUMBER);

    AST_T* node = init_ast(AST_NOOP);
    if(parser->current_token->type == TOKEN_OPERATOR){
        node = parser_parse_expression(parser);
    }
    
    
    return node;
}

#endif