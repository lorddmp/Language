#include "reading_tree.h"
#include "tech_func.h"

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#define SAVEFILE_NAME_FRONT "Middle_end/Saved_tree_middle.txt"

Node_t* Read_Node(int* position, char* massive);

Node_t* Obrabotka_Node(int* position, char* massive);

Node_t* Read_Tree(void)
{ 
    FILE* fp = fopen(SAVEFILE_NAME_FRONT, "r");//func get_file_size
    struct stat stat1 = {};
    int descriptor = fileno(fp);
    int position = 0;
    
    fstat(descriptor, &stat1);

    char* massive = (char*)calloc((size_t)stat1.st_size + 1, sizeof(char));
    IF_ERROR(massive);

    fread(massive, sizeof(char), (size_t)stat1.st_size, fp);

    Node_t* node = Read_Node(&position, massive);
    IF_ERROR(node);

    free(massive);
    fclose(fp);

    return(node);
}

Node_t* Read_Node(int* position, char* massive)
{
    Skip_Spaces(position, massive);

    if (massive[*position] == '(')
    {
        Node_t* node = Obrabotka_Node(position, massive);
        IF_ERROR(node);
        return node;
    }

    if (strncmp(&massive[*position], "nil", 3) == 0)
    {
        *position += 3;
        return NULL;
    }
    else
    {
        fprintf(stderr, "Error in reading file\n");
        return NULL;
    }
}

Node_t* Obrabotka_Node(int* position, char* massive)
{
    int skip = 0;
    double new_node_value_num = 0;
    oper_codes new_node_value_op = END_CODE;
    int new_node_value_var = 0;

    type_codes new_node_type = TREE_ROOT_CODE;
    (*position)++;

    Skip_Spaces(position, massive);
    sscanf(&massive[*position], "%d|%n", (int*)&new_node_type, &skip);
    if (skip == 0)
    {
        fprintf(stderr, "ERROR in file: %s, function: %s, line: %d", __FILE__, __func__, __LINE__);
        return NULL;
    }

    (*position) += skip;
    Skip_Spaces(position, massive);

    Node_t* node = NULL;
    switch (new_node_type)
    {
        case NUM_CODE: 
            sscanf(&massive[*position], "%lg%n", &new_node_value_num, &skip);
            node = Make_Node(new_node_type, {.num_t = new_node_value_num});
            break;
        case OPER_CODE: 
            sscanf(&massive[*position], "%d%n", (int*)&new_node_value_op, &skip);
            node = Make_Node(new_node_type, {.op_code_t = new_node_value_op});
            break;
        case VAR_CODE: 
            sscanf(&massive[*position], "%d%n", &new_node_value_var, &skip);
            node = Make_Node(new_node_type, {.var_ind = new_node_value_var});
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
    
    (*position) += skip;


    if ((node->left = Read_Node(position, massive)) != NULL)
        node->left->parent = node;
    if ((node->right = Read_Node(position, massive)) != NULL)
        node->right->parent = node;
    Skip_Spaces(position, massive);

    (*position)++;
    return node;
}

