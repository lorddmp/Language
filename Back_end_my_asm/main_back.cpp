#include "reading_tree.h"
#include "convert.h"
#include "tech_func.h"

#include <stdio.h>

int main()
{
    Node_t* root_node = Read_Tree();

    Converting(root_node);

    Tree_Destructor(root_node);
    
    printf("\033[32mBACK_END ENDED SUCCESSFULLY\033[0m\n");
    return 0;
}