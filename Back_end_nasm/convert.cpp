#include "convert.h"

#include <stdlib.h>
#include <tech_func.h>

#define NUM_AVAILABLE_REGS 15

struct var_info {
    char scope;
    int reg;
    int func;
};

struct num_info {
    bool already_exist;
    data_t num;
};

void Set_Var_Num_Array(Node_t* node, var_info* name_array, int* func_ind, num_info* num_array, int* free_index_num_array);
void Set_Data(var_info* name_array, int num_name, num_info* num_array, int num_const_num, FILE* fp);

int Free_Reg_Search(bool* reg_free_array);
int Node_Processing(Node_t* node, bool* reg_free_array, var_info* name_array, FILE* fp);

void Converting(Node_t* root_node, int num_name, int num_const_num)
{
    FILE* fp = fopen("Back_end_nasm/Commands.asm", "w");

    int func_ind = -1, free_index_num_array = 0;
    var_info* name_array = (var_info*)calloc((size_t)num_name, sizeof(var_info));
    num_info* num_array = (num_info*)calloc((size_t)num_const_num, sizeof(num_info));

    Set_Var_Num_Array(root_node, name_array, &func_ind, num_array, &free_index_num_array);
    Set_Data(name_array, num_name, num_array, num_const_num, fp);

    fprintf(fp, "section .text\n\n");
    fprintf(fp, "_Start:\n\n");

    bool* reg_free_array = (bool*)calloc(NUM_AVAILABLE_REGS, sizeof(bool));

    // Node_Processing(root_node, reg_free_array, name_array, fp);

    // for (int i = 0; i < num_name; i++)
    //     putchar(name_array[i]);

    // putchar('\n');

    free(reg_free_array);
    free(name_array);
    free(num_array);
    fclose(fp);
}

void Set_Var_Num_Array(Node_t* node, var_info* name_array, int* func_ind, num_info* num_array, int* free_index_num_array)
{
    if (node->value.op_code_t == FUNC_INIT_CODE)
        *func_ind = node->left->value.name_ind;

    else if (node->type == NAME_CODE && node->parent->value.op_code_t != FUNC_INIT_CODE && node->parent->value.op_code_t != FUNC_CALL_CODE)
    {
        if (*func_ind == -1)
            name_array[node->value.name_ind].scope = 'g'; //global var
        else
        {
            name_array[node->value.name_ind].func = *func_ind;
            name_array[node->value.name_ind].scope = 'l'; //local var
        }
    }

    else if (node->type == NUM_CODE)
    {
        num_array[*free_index_num_array].num = node->value.num_t;

        for (int i = 0; i < *free_index_num_array; i++)
            if (Is_Zero(num_array[i].num - node->value.num_t))
                num_array[*free_index_num_array].already_exist = true;

        (*free_index_num_array)++;
    }

    if (node->left != NULL)
        Set_Var_Num_Array(node->left, name_array, func_ind, num_array, free_index_num_array);

    if (node->right != NULL)
        Set_Var_Num_Array(node->right, name_array, func_ind, num_array, free_index_num_array);

    if (node->value.op_code_t == FUNC_INIT_CODE)
        *func_ind = -1;
}

void Set_Data(var_info* name_array, int num_name, num_info* num_array, int num_const_num, FILE* fp)
{ 
    fprintf(fp, "section .data\n\n");

    for (int pos_name_array = 0; pos_name_array < num_name; pos_name_array++)
        if (name_array[pos_name_array].scope == 'g')
            fprintf(fp, "var%d dq 0\n", pos_name_array);

    for (int pos_num_array = 0; pos_num_array < num_const_num; pos_num_array++)
        if (num_array[pos_num_array].already_exist == false)
            fprintf(fp, "const_%d dq %lg\n", pos_num_array, num_array[pos_num_array].num);

    fprintf(fp, "\n");
}

int Free_Reg_Search(bool* reg_free_array)
{
    for (int i = 0; i < NUM_AVAILABLE_REGS; i++)
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

    switch (node->value.op_code_t)
    {
        case FUNC_INIT_CODE:
            fprintf(fp, "JMP .%p\n", node);
            fprintf(fp, ".func%d:\n", node->left->value.name_ind);
            Node_Processing(node->right, reg_free_array, name_array, fp);
            fprintf(fp, "RET\n");
            fprintf(fp, ".%p:\n", node);
            return 0;

        case FUNC_CALL_CODE:
            fprintf(fp, "CALL .func%d\n", node->left->value.name_ind);
            return 0;

        default:
            break;
    }

    if (node->left != NULL)
        Node_Processing(node->left, reg_free_array, name_array, fp);
    
    if (node->right != NULL && (node->type != BODY_CODE && node->type != TREE_ROOT_CODE))
        Node_Processing(node->right, reg_free_array, name_array, fp);

    return 0;
}