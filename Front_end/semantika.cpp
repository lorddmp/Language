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

bool Looking_for_init(Node_t* node, var_func_t* exist_name_array);

// bool Var_Func_Check(Node_t* node, var_func_t* exist_name_array);

bool Sem_analysis(Node_t* root_node, int num_name)
{
    var_func_t* exist_name_array = (var_func_t*)calloc((size_t)num_name, sizeof(var_func_t));

    if (exist_name_array == NULL)
    {
        fprintf (stderr, "CALLOC ERROR\n");
        return false;
    }

    bool is_there_no_errors = Looking_for_init(root_node, exist_name_array);

    free(exist_name_array);

    return is_there_no_errors;
}

bool Looking_for_init(Node_t* node, var_func_t* exist_name_array)
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
            fprintf(stderr, "Error in semantic analysis\n");
            return false;
    }

    if (node->right != NULL)
        IF_ERROR_SEMANT(Looking_for_init(node->right, exist_name_array))

    if (node->left != NULL)
        IF_ERROR_SEMANT(Looking_for_init(node->left, exist_name_array));

    if (node->right != NULL && node->right->type == OPER_CODE && node->right->value.op_code_t == VAR_INIT_CODE) //{x = 8}
        exist_name_array[node->right->left->value.name_ind].existence = false;                                             //x += 5;

    return true;
}

// bool Var_Func_Check(Node_t* node, var_func_t* exist_name_array)
// {

// }