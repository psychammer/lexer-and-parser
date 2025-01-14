#ifndef AST_H
#define AST_H
#include <stdlib.h>

typedef struct AST_STRUCT
{
    enum{
        AST_DECLARATION_ASSIGNMENT_STATEMENT,
        AST_ASSIGNMENT,
        AST_DECLARATION,
        AST_VARIABLE_DEFINITION,
        AST_FUNCTION_DEFINITION,
        AST_VARIABLE,
        AST_FUNCTION_CALL,
        AST_STRING,
        AST_EXPRESSION,
        AST_TERM,
        AST_POWER,
        AST_FACTOR,
        AST_COMPOUND,
        AST_NOOP // NULL OPERATION
    }type;

    struct SCOPE_STRUCT* scope;

    /* AST_DECLARATION_STATEMENT */
    int datatype;
    char* identifier;
    struct AST_STRUCT* dec_assign_stmt;
    struct AST_STRUCT* ident_list;

    /* AST_DECLARATION_ASSIGNMENT_STATEMENT */
    char* declaration_assignment_statement_identifier; 
    struct AST_STRUCT* declaration_assignment_statement_value;

    /* AST_ASSIGNMENT */
    char* assignment_identifier;
    struct AST_STRUCT* assignment_expression;

    /* AST_FUNCTION_DEFINITION */
    struct AST_STRUCT* function_definition_body;
    char* function_definition_name;
    char* function_definition_datatype;
    struct AST_STRUCT** function_definition_args;
    char* function_definition_arg_datatype;
    size_t function_definition_args_size;

    /* AST_VARIABLE_DEFINITION*/
    char* variable_definition_variable_name; 
    struct AST_STRUCT* variable_definition_value;

    /* AST_VARIABLE */
    char* variable_name;

     /* AST_FUNCTION_CALL */
    char* function_call_name;
    struct AST_STRUCT** function_call_arguments;
    size_t function_call_arguments_size;

    /* AST_STRING */
    char* string_value;


    /* AST_EXPRESSION */
    struct AST_STRUCT* first_term;
    char* md_operator; 
    struct AST_STRUCT* second_term;;

    /* AST_TERM */
    struct AST_STRUCT* first_power;
    char* as_operator;
    struct AST_STRUCT* second_power;

    /* AST_POWER */
    struct AST_STRUCT* first_factor;
    char* p_operator;
    struct AST_STRUCT* second_factor;

    /* AST_FACTOR */
    double number;
    struct AST_STRUCT* expression;

    /* AST_COMPOUND */
    struct AST_STRUCT** compound_value;
    size_t compound_size; 
} AST_T;






AST_T* init_ast(int type)
{
    AST_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->type = type;

    /* AST_DECLARATION_STATEMENT */
    ast->datatype = -1;
    ast->identifier = (void*)0;
    ast->dec_assign_stmt = (void*)0;
    ast->ident_list = (void*)0;

    /* AST_DECLARATION_ASSIGNMENT_STATEMENT */
    ast->declaration_assignment_statement_identifier = (void*)0 ; 
    ast->declaration_assignment_statement_value = (void*)0;

    /* AST_VARIABLE_DEFINITION*/
    ast->variable_definition_variable_name = (void*)0; 
    ast->variable_definition_value = (void*)0;

    /* AST_VARIABLE */
    ast->variable_name = (void*)0;

     /* AST_FUNCTION_CALL */
    ast->function_call_name = (void*)0;
    ast->function_call_arguments = (void*)0; 
    ast->function_call_arguments_size = 0;

    /* AST_STRING */
    ast->string_value = (void*)0;

    /* AST_COMPOUND */
    ast->compound_value = (void*)0;;
    ast->compound_size = 0; 


    /* AST_ASSIGNMENT */
    char* assignment_identifier;
    struct AST_STRUCT* assignment_expression;

    return ast;

}
#endif