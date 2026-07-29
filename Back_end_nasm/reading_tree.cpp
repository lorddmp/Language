#include "reading_tree.h"
#include "tech_func.h"

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#define SAVEFILE_NAME_FRONT "Middle_end/Saved_tree_middle.txt"

Node_t* Read_Node(int* pos_tree_array, char* tree_array, int* num_name);

Node_t* Obrabotka_Node(int* pos_tree_array, char* tree_array, int* num_name);

Node_t* Read_Tree(int* num_name)
{ 
    FILE* fp = fopen(SAVEFILE_NAME_FRONT, "r");//func get_file_size
    struct stat stat1 = {};
    int descriptor = fileno(fp);
    int pos_tree_array = 0;
    
    fstat(descriptor, &stat1);

    char* tree_array = (char*)calloc((size_t)stat1.st_size + 1, sizeof(char));
    IF_ERROR(tree_array);

    fread(tree_array, sizeof(char), (size_t)stat1.st_size, fp);

    Node_t* node = Read_Node(&pos_tree_array, tree_array, num_name);
    IF_ERROR(node);

    free(tree_array);
    fclose(fp);

    return(node);
}

Node_t* Read_Node(int* pos_tree_array, char* tree_array, int* num_name)
{
    Skip_Spaces(pos_tree_array, tree_array);

    if (tree_array[*pos_tree_array] == '(')
    {
        Node_t* node = Obrabotka_Node(pos_tree_array, tree_array, num_name);
        IF_ERROR(node);
        return node;
    }

    if (strncmp(&tree_array[*pos_tree_array], "nil", 3) == 0)
    {
        *pos_tree_array += 3;
        return NULL;
    }
    else
    {
        fprintf(stderr, "Error in reading file\n");
        return NULL;
    }
}

Node_t* Obrabotka_Node(int* pos_tree_array, char* tree_array, int* num_name)
{
    int skip = 0;
    double new_node_value_num = 0;
    oper_codes new_node_value_op = END_CODE;
    int new_node_value_var = 0;

    type_codes new_node_type = TREE_ROOT_CODE;
    (*pos_tree_array)++;

    Skip_Spaces(pos_tree_array, tree_array);
    sscanf(&tree_array[*pos_tree_array], "%d|%n", (int*)&new_node_type, &skip);
    if (skip == 0)
    {
        fprintf(stderr, "ERROR in file: %s, function: %s, line: %d", __FILE__, __func__, __LINE__);
        return NULL;
    }

    (*pos_tree_array) += skip;
    Skip_Spaces(pos_tree_array, tree_array);

    Node_t* node = NULL;
    switch (new_node_type)
    {
        case NUM_CODE: 
            sscanf(&tree_array[*pos_tree_array], "%lg%n", &new_node_value_num, &skip);
            node = Make_Node(new_node_type, {.num_t = new_node_value_num});
            break;
        case OPER_CODE: 
            sscanf(&tree_array[*pos_tree_array], "%d%n", (int*)&new_node_value_op, &skip);
            node = Make_Node(new_node_type, {.op_code_t = new_node_value_op});

            if (new_node_value_op == VAR_INIT_CODE || new_node_value_op == FUNC_INIT_CODE)
                (*num_name)++;

            break;
        case NAME_CODE: 
            sscanf(&tree_array[*pos_tree_array], "%d%n", &new_node_value_var, &skip);
            node = Make_Node(new_node_type, {.name_ind = new_node_value_var});
            break;
        case BODY_CODE: 
            node = Make_Node(new_node_type, {});
            skip = 1;
            break;
        case TREE_ROOT_CODE: 
            node = Make_Node(new_node_type, {});
            skip = 1;
            break;
        default:
        {
            fprintf(stderr, "ERROR IN READING FILE!\n");
            return NULL;
        }
    }
    
    (*pos_tree_array) += skip;


    if ((node->left = Read_Node(pos_tree_array, tree_array, num_name)) != NULL)
        node->left->parent = node;
    if ((node->right = Read_Node(pos_tree_array, tree_array, num_name)) != NULL)
        node->right->parent = node;
    Skip_Spaces(pos_tree_array, tree_array);

    (*pos_tree_array)++;
    return node;
}

