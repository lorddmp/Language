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
    Node_t* root_node = Read_Tree();

    errors err = NO_ERRORS;

    Calculate(root_node, &err);

    if (err != NO_ERRORS)
    {
        printf("ERROR\n");
        return 1;
    }

    Save_Tree(root_node, fp);
    Print_Tree(root_node);

    Tree_Destructor(root_node);

    fclose(fp);
    printf("\033[32mMIDDLE_END ENDED SUCCESSFULLY\033[0m\n");
    return 0;
}