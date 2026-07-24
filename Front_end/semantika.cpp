#include "semantika.h"

#include <stdio.h>
#include <stdlib.h>

#define IF_ERROR_SEMANT(val)    \
if (val == false)               \
    return false;               \

bool Looking_for_init(Node_t* node, bool* if_var_exist_array);

bool Sem_analysis(Node_t* root_node, int num_name)
{
    bool* if_var_exist_array = (bool*)calloc(num_name, sizeof(bool));

    if (if_var_exist_array == NULL)
        printf ("CALLOC ERROR\n");
        return false;

    bool is_there_no_errors = Looking_for_init(root_node, if_var_exist_array);

    free(if_var_exist_array);

    return is_there_no_errors;
}

bool Looking_for_init(Node_t* node, bool* if_var_exist_array)
{
    if (node->type == OPER_CODE && (node->value.op_code_t == VAR_INIT_CODE || node->value.op_code_t == FUNC_INIT_CODE))
        if_var_exist_array[node->left->value.name_ind] = true;

    else if (node->type == NAME_CODE && if_var_exist_array[node->value.name_ind] == false)
    {
            fprintf(stderr, "Error in semantic analysis\n");
            return false;
    }

    if (node->right != NULL)
        IF_ERROR_SEMANT(Looking_for_init(node->right, if_var_exist_array))

    if (node->left != NULL)
        IF_ERROR_SEMANT(Looking_for_init(node->left, if_var_exist_array));

    if (node->right != NULL && node->right->type == OPER_CODE && node->right->value.op_code_t == VAR_INIT_CODE) //{x = 8}
        if_var_exist_array[node->right->left->value.name_ind] = false;                                             //x += 5;

    return true;
}