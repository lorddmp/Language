#include <stdio.h>
#include <stdlib.h>

#include "tokenise.h"
#include "parsing.h"
#include "print_tree.h"
#include "tech_func.h"
#include "structs_defines_types.h"

int main()
{
    FILE* fp = fopen("Saved_tree.txt", "w");
    char** massive_var = (char**)calloc(100, sizeof(char*));

    Node_t** mas_tokens = Tokenize(massive_var);

    if (mas_tokens == NULL)
        return 1;

    tree_t tree = Parsing(mas_tokens);

    if (tree.root_node == NULL)
        return 1;

    Print_Tree(tree.root_node);
    Save_Tree(tree.root_node, fp);

    free(mas_tokens);
    Tree_Destructor(tree.root_node);

    free(massive_var);
    return 0;
}