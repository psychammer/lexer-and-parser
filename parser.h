#ifndef PARSER_H
#define PARSER_H

#include "token.h"


typedef struct AST {
    struct AST **children;
    int num_children;
    char *name;
    token *token;
} AST;


AST *parse_statements();
AST *parse_statement();
AST *parse_assignment();
AST *parse_identifier();
AST *parse_term();
AST *parse_power();
AST *parse_factor();
AST *parse_conditional();
AST *parse_output();
AST *parse_bool_expression();
AST *parse_bool_term();
AST *parse_bool_factor();
AST *parse_body();
AST *parse_output_statement(); 
AST *parse_print_expression();  
AST *parse_else();
AST *parse_function_statement();
AST *parse_datatype();
AST *parse_rel_expression();
AST *parse_constant();
AST *parse_function_call();
AST *parse_iterative_statement();
AST *parse_for_body();
AST *parse_it_assign_stmt();
AST *parse_while_body();
AST *parse_do_while_body();
AST *parse_increment();
AST *parse_input_statement();
AST *parse_type_cast();
AST *parse_ident_list();
AST *parse_dec_assign();
AST *parse_array();
AST *parse_variable_stmt();
AST *parse_declaration_stmt();
AST *parse_unary();
AST *parse_return_statement();
AST *parse_program();
AST *parse_identifier();
AST *parse_parameter_list();
AST *parse_output_statement();
AST *parse_exp();
AST *parse_factor();
void recover();
token *load_tokens(const char *filename, int *num_tokens);
void print_parse_tree(AST *node, int indent_level);
void print_indent(int indent_level);
void free_AST_memory(AST *node);
AST *check_create_advance(TokenType type, const char* node_name);
void add_child(AST *parent, AST *child);

AST *create_node(const char *name);

#endif // PARSER_H