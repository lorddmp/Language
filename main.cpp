#include <stdio.h>
#include <stdlib.h>

#include "tokenise.h"
#include "parsing.h"
#include "print_tree.h"
#include "tech_func.h"
#include "structs_defines_types.h"

int main()
{
    char* massive_var[100] = {};

    Node_t** mas_tokens = Tokenize(massive_var);

    if (mas_tokens == NULL)
        return 1;

    printf("\n\n\n NOW TREE:\n");

    for (int i = 0; i < 5; i++)
        printf("i) op_code = %d, val_num = %lg, val_op = %d, val_var = %d\n", mas_tokens[i]->type, mas_tokens[i]->value.num_t, mas_tokens[i]->value.op_code_t, mas_tokens[i]->value.var_ind);

    tree_t tree = Parsing(mas_tokens);

    if (tree.root_node == NULL)
        return 1;

    Print_Tree(tree.root_node);

    free(mas_tokens);
    Tree_Destructor(tree.root_node);

    return 0;
}