#include "semantika.h"

#include <stdio.h>
#include <stdlib.h>

#define IF_ERROR_SEMANT(val)    \
if (val == false)               \
    return false;               \

struct var_func_t {
    bool existence;
    char type;
};

bool Existance_Name_Check(Node_t* node, var_func_t* exist_name_array);
bool Var_Func_Check(Node_t* node, var_func_t* exist_name_array);
bool Func_in_Func_Check(Node_t* node, bool* inside_func);

bool Sem_analysis(Node_t* root_node, int num_name)
{
    var_func_t* exist_name_array = (var_func_t*)calloc((size_t)num_name, sizeof(var_func_t));
    bool inside_func = false;

    if (exist_name_array == NULL)
    {
        fprintf (stderr, "CALLOC ERROR\n");
        return false;
    }

    bool is_there_no_errors =   Existance_Name_Check(root_node, exist_name_array) && 
                                Var_Func_Check(root_node, exist_name_array) &&
                                Func_in_Func_Check(root_node, &inside_func);

    free(exist_name_array);

    return is_there_no_errors;
}

bool Existance_Name_Check(Node_t* node, var_func_t* exist_name_array)
{
    if (node->type == OPER_CODE && node->value.op_code_t == VAR_INIT_CODE)
    {
        exist_name_array[node->left->value.name_ind].existence = true;
        exist_name_array[node->left->value.name_ind].type = 'v';
    }

    else if (node->type == OPER_CODE && node->value.op_code_t == FUNC_INIT_CODE)
    {
        exist_name_array[node->left->value.name_ind].existence = true;
        exist_name_array[node->left->value.name_ind].type = 'f';
    }

    else if (node->type == NAME_CODE && exist_name_array[node->value.name_ind].existence == false)
    {
            fprintf(stderr, "Error in semantic analysis: a name that doesn't exist is being used.\n");
            return false;
    }

    if (node->right != NULL)
        IF_ERROR_SEMANT(Existance_Name_Check(node->right, exist_name_array))

    if (node->left != NULL)
        IF_ERROR_SEMANT(Existance_Name_Check(node->left, exist_name_array));

    if (node->right != NULL && node->right->type == OPER_CODE && node->right->value.op_code_t == VAR_INIT_CODE) //{x = 8}
        exist_name_array[node->right->left->value.name_ind].existence = false;                                             //x += 5;

    return true;
}

bool Var_Func_Check(Node_t* node, var_func_t* exist_name_array)
{
    if (node->type == NAME_CODE && 
        exist_name_array[node->value.name_ind].type == 'f' && 
        node->parent->value.op_code_t != FUNC_INIT_CODE && 
        node->parent->value.op_code_t != FUNC_CALL_CODE)
    {
        fprintf(stderr, "Error in semantic analysis: the function name is not being used for its intended purpose\n");
        return false;
    }

    else if ((node->value.op_code_t == FUNC_INIT_CODE || node->value.op_code_t == FUNC_CALL_CODE) &&
            exist_name_array[node->left->value.name_ind].type == 'v')
    {
        fprintf(stderr, "Error in semantic analysis: the variable name is not being used for its intended purpose\n");
        return false;
    }

    if (node->right != NULL)
        IF_ERROR_SEMANT(Var_Func_Check(node->right, exist_name_array))

    if (node->left != NULL)
        IF_ERROR_SEMANT(Var_Func_Check(node->left, exist_name_array));

    return true;
}

bool Func_in_Func_Check(Node_t* node, bool* inside_func)
{
    if (node->value.op_code_t == FUNC_INIT_CODE)
    {
        if (*inside_func == false)
            *inside_func = true;
        else
        {
            fprintf(stderr, "Error in semantic analysis: initializing a function within an initializing function\n");
            return false;
        }
    }

    if (node->right != NULL)
        IF_ERROR_SEMANT(Func_in_Func_Check(node->right, inside_func))

    if (node->left != NULL)
        IF_ERROR_SEMANT(Func_in_Func_Check(node->left, inside_func));

    if (node->value.op_code_t == FUNC_INIT_CODE)
        *inside_func = false;

    return true;
    
}
