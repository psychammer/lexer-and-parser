#ifndef VISITOR_H
#define VISITOR_H
#include "AST.h"
#include "scope.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct VISITOR_STRUCT
{
    FILE* filename;
} visitor_T;

visitor_T* init_visitor();

AST_T* visitor_visit(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_assignment(visitor_T* visitor, AST_T* node);

void print_ast_prefix(AST_T* node, visitor_T* visitor);

AST_T* visitor_visit_bool_expression(visitor_T* visitor, AST_T* node);
AST_T* visitor_visit_bool_term(visitor_T* visitor, AST_T* node);
AST_T* visitor_visit_bool_factor(visitor_T* visitor, AST_T* node);
AST_T* visitor_visit_comparison(visitor_T* visitor, AST_T* node);






static AST_T* builtin_function_print(visitor_T* visitor, AST_T** args, int args_size)
{
    for (int i = 0; i < args_size; i++)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[i]);

        switch (visited_ast->type)
        {
            case AST_STRING: printf("%s\n", visited_ast->string_value); break;
            default: printf("%p\n", visited_ast); break;
        }
    }

    return init_ast(AST_NOOP);
}

static AST_T* builtin_function_exit(visitor_T* visitor, AST_T** args, int args_size)
{
    for (int i = 0; i < args_size; i++)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[i]);

        switch (visited_ast->type)
        {
            case AST_NOOP: printf("You exited\n"); exit(0); break;
            default: printf("%p\n", visited_ast); break;
        }
    }

    return init_ast(AST_NOOP);
}

static AST_T* builtin_function_clear(visitor_T* visitor, AST_T** args, int args_size)
{
    for (int i = 0; i < args_size; i++)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[i]);

        switch (visited_ast->type)
        {
            case AST_NOOP: system("clear"); break;
            default: printf("%p\n", visited_ast); break;
        }
    }

    return init_ast(AST_NOOP);
}

visitor_T* init_visitor(char* output_file)
{
    visitor_T* visitor = calloc(1, sizeof(struct VISITOR_STRUCT));

    FILE* file = fopen(output_file, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    visitor->filename = file;

    fprintf(visitor->filename, "Program -> statements\n");

    return visitor;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* node)
{
    switch (node->type)
    {
        case AST_VARIABLE_DEFINITION: return visitor_visit_variable_definition(visitor, node); break;
        case AST_FUNCTION_DEFINITION: return visitor_visit_function_definition(visitor, node); break;
        case AST_VARIABLE: return visitor_visit_variable(visitor, node); break;
        case AST_FUNCTION_CALL: return visitor_visit_function_call(visitor, node); break;
        case AST_STRING: return visitor_visit_string(visitor, node); break;
        case AST_COMPOUND: return visitor_visit_compound(visitor, node); break;
        case AST_ASSIGNMENT: return visitor_visit_assignment(visitor, node); break;
        
        //changes
        case AST_BOOL_EXPRESSION: return visitor_visit_bool_expression(visitor, node); break;
        case AST_BOOL_TERM: return visitor_visit_bool_term(visitor, node); break;
        case AST_BOOL_FACTOR: return visitor_visit_bool_factor(visitor, node); break;
        case AST_COMPARISON: return visitor_visit_comparison(visitor, node); break;

        case AST_NOOP: return node; break;

        
    }

    printf("Uncaught statement of type `%d`\n", node->type);
    exit(1);

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node)
{
    scope_add_variable_definition(
        node->scope,
        node        
    ); 

    return node;
}

AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node)
{
    fprintf(visitor->filename,"Function_definition -> <datatype> <identifier> <parameter-list> <statements>\n");
    scope_add_function_definition(
        node->scope,
        node        
    );

    if(node->function_definition_datatype != (void*)0){
        fprintf(visitor->filename,"datatype -> %s\n", node->function_definition_datatype);
    }
    fprintf(visitor->filename,"identifier -> %s\n", node->function_definition_name);

    fprintf(visitor->filename,"parameter-list -> ");
    for(int i=0; i<node->function_definition_args_size; i++){
        fprintf(visitor->filename,"%s %s, ", node->function_definition_args[i]->function_definition_arg_datatype,  node->function_definition_args[i]->variable_name);
    }
    fprintf(visitor->filename, "\n");

    visitor_visit(visitor, node->function_definition_body);

    fprintf(visitor->filename,"\n");



    return node;
}

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node)
{
    AST_T* vdef = scope_get_variable_definition(
        node->scope,
        node->variable_name
    );
    
    if (vdef != (void*) 0)
        return visitor_visit(visitor, vdef->variable_definition_value);

    printf("Undefined variable `%s`\n", node->variable_name);
    exit(1);
}

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node)
{
    if (strcmp(node->function_call_name, "print") == 0)
    {
        return builtin_function_print(visitor, node->function_call_arguments, node->function_call_arguments_size);
    }

    if (strcmp(node->function_call_name, "exit") == 0)
    {
        return builtin_function_exit(visitor, node->function_call_arguments, node->function_call_arguments_size);
    }

    if (strcmp(node->function_call_name, "clear") == 0)
    {
        return builtin_function_clear(visitor, node->function_call_arguments, node->function_call_arguments_size);
    }

    AST_T* fdef = scope_get_function_definition(
        node->scope,
        node->function_call_name
    );

    if (fdef == (void*)0)
    {
        printf("Undefined method `%s`\n", node->function_call_name);
        exit(1);
    }

    for (int i = 0; i < (int) node->function_call_arguments_size; i++)
    {
        // grab the variable from the function definition arguments
        AST_T* ast_var = (AST_T*) fdef->function_definition_args[i];

        // grab the value from the function call arguments
        AST_T* ast_value = (AST_T*) node->function_call_arguments[i];

        // create a new variable definition with the value of the argument
        // in the function call.
        AST_T* ast_vardef = init_ast(AST_VARIABLE_DEFINITION);
        ast_vardef->variable_definition_value = ast_value;

        // copy the name from the function definition argument into the new
        // variable definition
        ast_vardef->variable_definition_variable_name = (char*) calloc(strlen(ast_var->variable_name) + 1, sizeof(char));
        strcpy(ast_vardef->variable_definition_variable_name, ast_var->variable_name);

        // push our variable definition into the function body scope.
        scope_add_variable_definition(fdef->function_definition_body->scope, ast_vardef);
    }
    
    return visitor_visit(visitor, fdef->function_definition_body);
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node)
{
    fprintf(visitor->filename, "statements -> ");
    for (int i = 0; i < node->compound_size; i++)
    {
        if(node->compound_value[i]->type==AST_ASSIGNMENT)
            fprintf(visitor->filename, "<assign-stmt> | ");
        else if (node->compound_value[i]->type==AST_FUNCTION_DEFINITION)
            fprintf(visitor->filename, "<function-definition-stmt> | ");
    }
    fprintf(visitor->filename, "\n");

    for (int i = 0; i < node->compound_size; i++)
    {
        visitor_visit(visitor, node->compound_value[i]);
    }

    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_assignment(visitor_T* visitor, AST_T* node)
{
    // AST_T* vassign = scope_get_variable_definition(
    //     node->scope,
    //     node->variable_name
    // );

    // if (vassign != (void*) 0)
    //     return visitor_visit(visitor, vassign->variable_definition_value);

    fprintf(visitor->filename, "<assignment-stmt> -> identifier \"=\" <expression> “;”\n");
    fprintf(visitor->filename, "<identifier> -> %s\n", node->variable_name);
    fprintf(visitor->filename, "<expression> -> ");
    // printf("%f\n", node->assignment_expression->first_term->number);
    // printf("%d\n", node->assignment_expression->type);

    print_ast_prefix(node->assignment_expression, visitor);
    fprintf(visitor->filename,"\n");
}



AST_T* visitor_visit_bool_expression(visitor_T* visitor, AST_T* node)
{
    fprintf(visitor->filename, "bool_expression -> ");
    
    // Visit left operand
    print_ast_prefix(node->bool_expr_left, visitor);
    
    // Print operator
    if (node->bool_expr_operator) {
        fprintf(visitor->filename, " or ");
    }
    
    // Visit right operand if it exists
    if (node->bool_expr_right) {
        print_ast_prefix(node->bool_expr_right, visitor);
    }
    
    fprintf(visitor->filename, "\n");
    
    // Add to scope
    scope_add_bool_expression(node->scope, node);
    
    return node;
}

AST_T* visitor_visit_bool_term(visitor_T* visitor, AST_T* node)
{
    fprintf(visitor->filename, "bool_term -> ");
    
    // Visit left operand
    print_ast_prefix(node->bool_term_left, visitor);
    
    // Print operator
    if (node->bool_term_operator) {
        fprintf(visitor->filename, " and ");
    }
    
    // Visit right operand if it exists
    if (node->bool_term_right) {
        print_ast_prefix(node->bool_term_right, visitor);
    }
    
    fprintf(visitor->filename, "\n");
    
    // Add to scope
    scope_add_bool_term(node->scope, node);
    
    return node;
}

AST_T* visitor_visit_bool_factor(visitor_T* visitor, AST_T* node)
{
    fprintf(visitor->filename, "bool_factor -> ");
    
    if (node->is_not) {  // Handle NOT operator
        fprintf(visitor->filename, "not ");
    }
    
    if (node->bool_literal_value >= 0) {  // It's a boolean literal
        fprintf(visitor->filename, "%s", node->bool_literal_value ? "true" : "false");
    } else if (node->bool_factor_expr) {  // Nested expression
        print_ast_prefix(node->bool_factor_expr, visitor);
    } else if (node->comparison) {  // Comparison
        print_ast_prefix(node->comparison, visitor);
    }
    
    fprintf(visitor->filename, "\n");
    
    // Add to scope
    scope_add_bool_factor(node->scope, node);
    
    return node;
}

AST_T* visitor_visit_comparison(visitor_T* visitor, AST_T* node)
{
    fprintf(visitor->filename, "comparison -> ");
    
    // Visit left operand
    if (node->comparison_left) {
        print_ast_prefix(node->comparison_left, visitor);
    }
    
    // Print comparison operator
    if (node->comparison_operator) {
        fprintf(visitor->filename, " %s ", node->comparison_operator);
    }
    
    // Visit right operand
    if (node->comparison_right) {
        print_ast_prefix(node->comparison_right, visitor);
    }
    
    fprintf(visitor->filename, "\n");
    
    return node;
}


void print_ast_prefix(AST_T* node, visitor_T* visitor) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        case AST_EXPRESSION:
            fprintf(visitor->filename,"%s ", node->as_operator); // Operator first
            print_ast_prefix(node->first_term, visitor);
            fprintf(visitor->filename," "); // Space between operands
            print_ast_prefix(node->second_term, visitor);
            break;
        case AST_TERM:
            fprintf(visitor->filename,"%s ", node->md_operator); // Operator first
            print_ast_prefix(node->first_power, visitor);
            fprintf(visitor->filename, " "); // Space between operands
            print_ast_prefix(node->second_power, visitor);
            break;
        case AST_POWER:
            fprintf(visitor->filename,"%s ", node->p_operator); // Operator first
            print_ast_prefix(node->first_factor, visitor);
            fprintf(visitor->filename," "); // Space between operands
            print_ast_prefix(node->second_factor, visitor);
            break;
        case AST_FACTOR:
            fprintf(visitor->filename,"%f", node->number);
            break;
        case AST_COMPOUND:
            printf("{ ");
            for (size_t i = 0; i < node->compound_size; i++) {
                print_ast_prefix(node->compound_value[i], visitor);
                if (i < node->compound_size - 1) {
                    printf(", ");
                }
            }
            printf(" }");
            break;

         case AST_COMPARISON:
            print_ast_prefix(node->comparison_left, visitor);
            fprintf(visitor->filename, " %s ", node->comparison_operator);
            print_ast_prefix(node->comparison_right, visitor);
            break;

        case AST_BOOL_EXPRESSION:
            print_ast_prefix(node->bool_expr_left, visitor);
            fprintf(visitor->filename, " %s ", node->bool_expr_operator);
            print_ast_prefix(node->bool_expr_right, visitor);
            break;

        case AST_BOOL_TERM:
            print_ast_prefix(node->bool_term_left, visitor);
            fprintf(visitor->filename, " %s ", node->bool_term_operator);
            print_ast_prefix(node->bool_term_right, visitor);
            break;

        case AST_BOOL_FACTOR:
            if (node->bool_literal_value) {
                fprintf(visitor->filename, "true");
            } else {
                fprintf(visitor->filename, "false");
            }
            break;


        case AST_NOOP:
            printf("NOOP");
            break;
        default:
            printf("UNKNOWN");
            break;
    }
}
// Add boolean handling to print_ast_prefix


#endif