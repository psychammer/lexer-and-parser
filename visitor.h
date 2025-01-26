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
        case AST_DO_WHILE: return visitor_visit_do_while(visitor, node); break;
        case AST_ASSIGNMENT: return visitor_visit_assignment(visitor, node); break;
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

AST_T* visitor_visit_do_while(visitor_T* visitor, AST_T* node) {
    printf("Do-While Loop\n");
    do {
        for (size_t i = 0; i < node->stmt_list_size; i++) {
            visitor_visit(visitor, node->stmt_list[i]);
        }
    } while (evaluate_bool_expression(visitor, node->bool_expr)); // Implement evaluate_bool_expression
    return node;
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
        case AST_DO_WHILE:
            printf("Do-While Loop\n");
            print_ast_prefix(node->stmt_list, visitor);
            print_ast_prefix(node->bool_expr, visitor);
            break;
        case AST_NOOP:
            printf("NOOP");
            break;
        default:
            printf("UNKNOWN");
            break;
    }
}


#endif