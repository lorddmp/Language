#include "semantika.h"

#include <stdio.h>

#define IF_ERROR_SEMANT(val)           \
if (val == false)               \
    return false;               \

bool Looking_for_init(Node_t* node, bool* mas_existing_var);

bool Sem_analysis(tree_t tree)
{
    bool mas_existing_var[100] = {false};

    return Looking_for_init(tree.root_node, mas_existing_var);
}

bool Looking_for_init(Node_t* node, bool* mas_existing_var)
{
    if (node->type == OPER_CODE && node->value.op_code_t == VAR_INIT_CODE)
        mas_existing_var[node->left->value.var_ind] = true;

    if (node->type == VAR_CODE)
    {
        if (mas_existing_var[node->value.var_ind] == false)
        {
            fprintf(stderr, "Error in semantic analysis\n");
            return false;
        }
    }

    if (node->right != NULL)
        IF_ERROR_SEMANT(Looking_for_init(node->right, mas_existing_var))

    if (node->left != NULL)
        IF_ERROR_SEMANT(Looking_for_init(node->left, mas_existing_var));

    return true;
}