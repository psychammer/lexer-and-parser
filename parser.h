#ifndef PARSER_H
#define PARSER_H

#include "token.h"

// Data structure for the parse tree
typedef struct AST {
    char *name;
    token *token;
    struct AST **children;
    int num_children;
} AST;

// Utility functions, tree printing, error
void recover();
token *load_tokens(const char *filename, int *num_tokens);
void add_child(AST *parent, AST *child);
AST *match_and_create_node(TokenType type, const char* node_name);
void match(TokenType type);
void report_error(const char *message, TokenType expected);
void synchronize();
void print_parse_tree(AST *node, int indent_level);
void print_indent(int indent_level);
void free_AST_memory(AST *node);
AST *check_create_advance(TokenType type, const char* node_name);

// Parsing
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

AST *parse_program();
AST *parse_declaration();
AST *parse_function_declaration();
AST *parse_variable_declaration();
AST *parse_array_declaration();
AST *parse_data_type();
AST *parse_identifier();
AST *parse_parameter_list();
AST *parse_argument_list();
AST *parse_block();
AST *parse_block_item_list();
AST *parse_block_item();
AST *parse_statement();
AST *parse_return_statement();
AST *parse_expression_statement();
AST *parse_factor_statement();
AST *parse_const_statement();
AST *parse_while_statement();
AST *parse_for_statement();
AST *parse_input_statement();
AST *parse_output_statement();
AST *parse_if_statement();
AST *parse_else_clause();
AST *parse_exp();
AST *parse_factor();
AST *parse_const();
AST *parse_int_literal();
AST *parse_float_literal();
AST *parse_char_literal();
AST *parse_bool_literal();
AST *parse_assignment();
AST *parse_logical_or_exp();
AST *parse_logical_and_exp();
AST *parse_equality_exp();
AST *parse_relational_exp();
AST *parse_additive_exp();
AST *parse_multiplicative_exp();
AST *parse_power_exp();
AST *parse_unary_exp();

// Creating parse tree nodes
AST *create_statements_node(); 
AST *create_statement_node(); 
AST *create_assignment_node();
AST *create_identifier_node();
AST *create_term_node();
AST *create_power_node();
AST *create_factor_node();
AST *create_conditional_node();
AST *create_bool_expression_node();
AST *create_bool_term_node();
AST *create_bool_factor_node();
AST *create_body_node();
AST *create_output_statement_node();
AST *create_parse_print_expression_node(); 
AST *create_else_node(); 
AST *create_function_statement_node();
AST *create_datatype_node();
AST *create_rel_expression_node();
AST *create_constant_node();
AST *create_function_call_node();


AST *create_program_node();
AST *create_declaration_node();
AST *create_function_declaration_node();
AST *create_variable_declaration_node();
AST *create_array_declaration_node();
AST *create_data_type_node();
AST *create_identifier_node();
AST *create_parameter_list_node();
AST *create_argument_list_node();
AST *create_block_node();
AST *create_block_item_list_node();
AST *create_block_item_node();
AST *create_statement_node();
AST *create_return_statement_node();
AST *create_expression_statement_node();
AST *create_factor_statement_node();
AST *create_const_statement_node();
AST *create_while_statement_node();
AST *create_for_statement_node();
AST *create_if_statement_node();
AST *create_input_statement_node();
AST *create_output_statement_node();
AST *create_exp_node();
AST *create_logical_or_exp_node();
AST *create_logical_and_exp_node();
AST *create_power_exp_node();
AST *create_factor_node();
AST *create_const_node();
AST *create_int_literal_node();
AST *create_float_literal_node();
AST *create_char_literal_node();
AST *create_bool_literal_node();
AST *create_node(const char *name);

#endif // PARSER_H