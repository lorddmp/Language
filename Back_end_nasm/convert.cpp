#include "convert.h"

#include <stdlib.h>

#define NUM_REGS 16

struct var_info {
    char scope;
    int reg;
    int func;
};

void Set_Scope_Array(Node_t* node, var_info* name_array, bool* inside_func);
void Set_Data(var_info* name_array, int num_name, FILE* fp);

int Free_Reg_Search(bool* reg_free_array);
int Node_Processing(Node_t* node, bool* reg_free_array, var_info* name_array, FILE* fp);

void Converting(Node_t* root_node, int num_name)
{
    FILE* fp = fopen("Back_end_nasm/Commands.asm", "w");

    bool inside_func = false;
    var_info* name_array = (var_info*)calloc((size_t)num_name, sizeof(var_info));

    Set_Scope_Array(root_node, name_array, &inside_func);
    Set_Data(name_array, num_name, fp);

    fprintf(fp, "section .text\n\n");

    bool* reg_free_array = (bool*)calloc(NUM_REGS, sizeof(bool));

    // for (int i = 0; i < num_name; i++)
    //     putchar(name_array[i]);

    // putchar('\n');

    free(reg_free_array);
    free(name_array);
    fclose(fp);
}

void Set_Scope_Array(Node_t* node, var_info* name_array, bool* inside_func)
{
    if (node->value.op_code_t == FUNC_INIT_CODE)
        *inside_func = true;

    else if (node->type == NAME_CODE && node->parent->value.op_code_t != FUNC_INIT_CODE && node->parent->value.op_code_t != FUNC_CALL_CODE)
    {
        if (*inside_func == true)
            name_array[node->value.name_ind].scope = 'l'; //local
        else
            name_array[node->value.name_ind].scope = 'g'; //global

    }

    if (node->left != NULL)
        Set_Scope_Array(node->left, name_array, inside_func);

    if (node->right != NULL)
        Set_Scope_Array(node->right, name_array, inside_func);

    if (node->value.op_code_t == FUNC_INIT_CODE)
        *inside_func = false;
}

void Set_Data(var_info* name_array, int num_name, FILE* fp)
{
    fprintf(fp, "section .data\n\n");

    for (int pos_name_array = 0; pos_name_array < num_name; pos_name_array++)
        if (name_array[pos_name_array].scope == 'g')
            fprintf(fp, "var%d dq 0\n", pos_name_array);

    fprintf(fp, "\n");
}

int Free_Reg_Search(bool* reg_free_array)
{
    for (int i = 0; i < NUM_REGS; i++)
    {
        if (reg_free_array[i] == true)
            return i;
    }

    return -1;
}

int Node_Processing(Node_t* node, bool* reg_free_array, var_info* name_array, FILE* fp)
{
    if (node->type == BODY_CODE || node->type == TREE_ROOT_CODE)
        Node_Processing(node->right, reg_free_array, name_array, fp);
}