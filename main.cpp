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

    tree_t tree = Parsing(mas_tokens);

    if (tree.root_node == NULL)
        return 1;

    Print_Tree(tree.root_node);

    free(mas_tokens);
    Tree_Destructor(tree.root_node);

    return 0;
}