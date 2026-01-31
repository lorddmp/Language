#include "calculator.h"
#include "reading_tree.h"
#include "print_tree.h"
#include "tech_func.h"

#include "structs_defines_types.h"

#include <stdio.h>

#define SAVEFILE_NAME_MIDDLE "Middle_end/Saved_tree_middle.txt"

int main()
{
    FILE* fp = fopen(SAVEFILE_NAME_MIDDLE, "w");
    tree_t tree = {
        .root_node = Read_Tree(),
    };

    errors err = NO_ERRORS;

    Calculate(&tree, tree.root_node, &err);

    if (err != NO_ERRORS)
    {
        printf("ERROR ERR\n");
        return 1;
    }

    Save_Tree(tree.root_node, fp);
    Print_Tree(tree.root_node);

    Tree_Destructor(tree.root_node);

    fclose(fp);
    printf("\033[32mMIDDLE_END ENDED SUCCESSFULLY\n");
    return 0;
}