#ifndef PARSER_H
#define PARSER_H

#include "token.h"

// Data structure for the parse tree
typedef struct ParseTreeNode {
    char *name;
    token *token;
    struct ParseTreeNode **children;
    int num_children;
} ParseTreeNode;

// Utility functions, tree printing, error
void recover();
token *load_tokens(const char *filename, int *num_tokens);
void add_child(ParseTreeNode *parent, ParseTreeNode *child);
ParseTreeNode *match_and_create_node(TokenType type, const char* node_name);
void match(TokenType type);
void report_error(const char *message, TokenType expected);
void synchronize();
void print_parse_tree(ParseTreeNode *node, int indent_level);
void print_indent(int indent_level);
void free_parse_tree(ParseTreeNode *node);
ParseTreeNode *check_create_advance(TokenType type, const char* node_name);

// Parsing
ParseTreeNode *parse_statements();
ParseTreeNode *parse_statement();
ParseTreeNode *parse_assignment();
ParseTreeNode *parse_identifier();
ParseTreeNode *parse_term();
ParseTreeNode *parse_power();
ParseTreeNode *parse_factor();


ParseTreeNode *parse_program();
ParseTreeNode *parse_declaration();
ParseTreeNode *parse_function_declaration();
ParseTreeNode *parse_variable_declaration();
ParseTreeNode *parse_array_declaration();
ParseTreeNode *parse_data_type();
ParseTreeNode *parse_identifier();
ParseTreeNode *parse_parameter_list();
ParseTreeNode *parse_argument_list();
ParseTreeNode *parse_block();
ParseTreeNode *parse_block_item_list();
ParseTreeNode *parse_block_item();
ParseTreeNode *parse_statement();
ParseTreeNode *parse_return_statement();
ParseTreeNode *parse_expression_statement();
ParseTreeNode *parse_factor_statement();
ParseTreeNode *parse_const_statement();
ParseTreeNode *parse_while_statement();
ParseTreeNode *parse_for_statement();
ParseTreeNode *parse_input_statement();
ParseTreeNode *parse_output_statement();
ParseTreeNode *parse_if_statement();
ParseTreeNode *parse_else_clause();
ParseTreeNode *parse_exp();
ParseTreeNode *parse_factor();
ParseTreeNode *parse_const();
ParseTreeNode *parse_int_literal();
ParseTreeNode *parse_float_literal();
ParseTreeNode *parse_char_literal();
ParseTreeNode *parse_bool_literal();
ParseTreeNode *parse_assignment();
ParseTreeNode *parse_logical_or_exp();
ParseTreeNode *parse_logical_and_exp();
ParseTreeNode *parse_equality_exp();
ParseTreeNode *parse_relational_exp();
ParseTreeNode *parse_additive_exp();
ParseTreeNode *parse_multiplicative_exp();
ParseTreeNode *parse_power_exp();
ParseTreeNode *parse_unary_exp();

// Creating parse tree nodes
ParseTreeNode *create_statements_node(); 
ParseTreeNode *create_statement_node(); 
ParseTreeNode *create_assignment_node();
ParseTreeNode *create_identifier_node();
ParseTreeNode *create_term_node();
ParseTreeNode *create_power_node();
ParseTreeNode *create_factor_node();


ParseTreeNode *create_program_node();
ParseTreeNode *create_declaration_node();
ParseTreeNode *create_function_declaration_node();
ParseTreeNode *create_variable_declaration_node();
ParseTreeNode *create_array_declaration_node();
ParseTreeNode *create_data_type_node();
ParseTreeNode *create_identifier_node();
ParseTreeNode *create_parameter_list_node();
ParseTreeNode *create_argument_list_node();
ParseTreeNode *create_block_node();
ParseTreeNode *create_block_item_list_node();
ParseTreeNode *create_block_item_node();
ParseTreeNode *create_statement_node();
ParseTreeNode *create_return_statement_node();
ParseTreeNode *create_expression_statement_node();
ParseTreeNode *create_factor_statement_node();
ParseTreeNode *create_const_statement_node();
ParseTreeNode *create_while_statement_node();
ParseTreeNode *create_for_statement_node();
ParseTreeNode *create_if_statement_node();
ParseTreeNode *create_input_statement_node();
ParseTreeNode *create_output_statement_node();
ParseTreeNode *create_exp_node();
ParseTreeNode *create_logical_or_exp_node();
ParseTreeNode *create_logical_and_exp_node();
ParseTreeNode *create_power_exp_node();
ParseTreeNode *create_factor_node();
ParseTreeNode *create_const_node();
ParseTreeNode *create_int_literal_node();
ParseTreeNode *create_float_literal_node();
ParseTreeNode *create_char_literal_node();
ParseTreeNode *create_bool_literal_node();
ParseTreeNode *create_node(const char *name);

#endif // PARSER_H