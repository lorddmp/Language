#include "reading_tree.h"
#include "convert.h"
#include "tech_func.h"

#include <stdio.h>

int main()
{
    tree_t tree = {.root_node = Read_Tree()};

    Converting(tree);

    Tree_Destructor(tree.root_node);
    printf("\033[32mBACK_END ENDED SUCCESSFULLY\n");
    return 0;
}