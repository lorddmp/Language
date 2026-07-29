#include <stdio.h>
#include <stdlib.h>

#include "tokenise.h"
#include "parsing.h"
#include "semantika.h"
#include "print_tree.h"
#include "tech_func.h"
#include "structs_defines_types.h"

#define SAVEFILE_NAME_FRONT "Front_end/Saved_tree_front.txt"

int main()
{
    FILE* fp = fopen(SAVEFILE_NAME_FRONT, "w");

    int size_name_array = 100, num_name = 0, num_token = 0;
    char** name_array = (char**)calloc((size_t)size_name_array, sizeof(char*));

    Node_t** token_array = Tokenize(name_array, size_name_array, &num_name, &num_token);

    if (token_array == NULL)
        return 1;

    Node_t* root_node = Parsing(token_array);

    if (root_node == NULL)
        return 1;

    if (Sem_analysis(root_node, num_name) == false)
        return 1;

    Print_Tree(root_node);
    Save_Tree(root_node, fp);

    ARRAY_DESTRUCTOR(token_array, num_token)
    ARRAY_DESTRUCTOR(name_array, num_name)
    free(token_array);
    Tree_Destructor(root_node);
    free(name_array);
    fclose(fp);

    printf("\033[32mFRONT_END ENDED SUCCESSFULLY\033[0m\n");
    return 0;
}