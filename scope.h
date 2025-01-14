#ifndef SCOPE_H
#define SCOPE_H
#include "AST.h"
#include <string.h>

typedef struct SCOPE_STRUCT
{
    AST_T** function_definitions;
    size_t function_definitions_size;

    AST_T** variable_definitions;
    size_t variable_definitions_size;
} scope_T;


scope_T* init_scope();

AST_T* scope_add_function_definition(scope_T* scope, AST_T* fdef);

AST_T* scope_get_function_definition(scope_T* scope, const char* fname);

AST_T* scope_add_variable_definition(scope_T* scope, AST_T* vdef);

AST_T* scope_get_variable_definition(scope_T* scope, const char* name);


scope_T* init_scope()
{
    scope_T* scope = calloc(1, sizeof(struct SCOPE_STRUCT));

    scope->function_definitions = (void*) 0;
    scope->function_definitions_size = 0;

    scope->variable_definitions = (void*) 0;
    scope->variable_definitions_size = 0;

    return scope;
}

AST_T* scope_add_function_definition(scope_T* scope, AST_T* fdef)
{
    
    scope->function_definitions_size += 1;

    if (scope->function_definitions == (void*)0)
    {
        
        scope->function_definitions = calloc(1, sizeof(struct AST_STRUCT*));
    }
    else
    {
        scope->function_definitions =
            realloc(
                scope->function_definitions,
                scope->function_definitions_size * sizeof(struct AST_STRUCT**)
            );
    }

    scope->function_definitions[scope->function_definitions_size-1] =
        fdef;

    return fdef;
}

AST_T* scope_get_function_definition(scope_T* scope, const char* fname)
{
    for (int i = 0; i < scope->function_definitions_size; i++)
    {
        AST_T* fdef = scope->function_definitions[i];

        if (strcmp(fdef->function_definition_name, fname) == 0)
        {
            return fdef;
        }
    }

    return (void*)0;
}

AST_T* scope_add_variable_definition(scope_T* scope, AST_T* vdef)
{
    if (scope->variable_definitions == (void*) 0)
    {
        scope->variable_definitions = calloc(1, sizeof(struct AST_STRUCT*));
        scope->variable_definitions[0] = vdef;
        scope->variable_definitions_size += 1;
    }
    else
    {
        scope->variable_definitions_size += 1;
        scope->variable_definitions = realloc(
            scope->variable_definitions,
            scope->variable_definitions_size * sizeof(struct AST_STRUCT*)  
        );
        scope->variable_definitions[scope->variable_definitions_size-1] = vdef;
    }

    return vdef;
}

AST_T* scope_get_variable_definition(scope_T* scope, const char* name)
{
    for (int i = 0; i < scope->variable_definitions_size; i++)
    {
        AST_T* vdef = scope->variable_definitions[i];

        if (strcmp(vdef->variable_definition_variable_name, name) == 0)
        {
            return vdef;
        }
    }

    return (void*)0;
}

#endif