#include "reading_tree.h"
#include "convert.h"
#include "tech_func.h"

#include <stdio.h>

int main()
{
    int num_name = 0, num_const_num = 0;
    Node_t* root_node = Read_Tree(&num_name, &num_const_num);

    Converting(root_node, num_name, num_const_num);

    Tree_Destructor(root_node);
    printf("\033[32mBACK_END ENDED SUCCESSFULLY\033[0m\n");
    return 0;
}