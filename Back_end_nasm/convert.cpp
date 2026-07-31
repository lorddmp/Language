#include "convert.h"

#include <stdlib.h>
#include <tech_func.h>

#define NUM_AVAILABLE_REGS 14

struct var_info {
    char scope;
    int reg;
    Node_t* func_adress;
};

struct num_info {
    bool already_exist;
    data_t num;
};

void Set_Var_Num_Array(Node_t* node, var_info* name_array, bool* inside_func, num_info* num_array, int* free_index_num_array, bool* reg_busy_array);

void Ban_Regs_From_Calls(Node_t* node, bool* reg_busy_array, var_info* name_array, bool inside_call);
void Unlock_Regs(Node_t* node, bool* reg_busy_array, var_info* name_array);

void Set_Data(var_info* name_array, int num_name, num_info* num_array, int num_const_num, FILE* fp);

int Free_Reg_Search(bool* reg_busy_array);
int Node_Processing(Node_t* node, var_info* name_array, int num_name, num_info* num_array, int num_const_num, FILE* fp);

void Converting(Node_t* root_node, int num_name, int num_const_num)
{
    FILE* fp = fopen("Back_end_nasm/Commands.asm", "w");

    var_info* name_array = (var_info*)calloc((size_t)num_name, sizeof(var_info));
    num_info* num_array = (num_info*)calloc((size_t)num_const_num, sizeof(num_info));
    bool* reg_busy_array = (bool*)calloc(NUM_AVAILABLE_REGS, sizeof(bool));

    bool inside_func = false;
    int free_index_num_array = 0;
    
    Set_Var_Num_Array(root_node, name_array, &inside_func, num_array, &free_index_num_array, reg_busy_array);
    Set_Data(name_array, num_name, num_array, num_const_num, fp);

    fprintf(fp, "section .text\n\n");
    fprintf(fp, "_Start:\n\n");


    Node_Processing(root_node, name_array, num_name, num_array, num_const_num, fp);

    // for (int i = 0; i < num_name; i++)
    //     putchar(name_array[i]);

    // putchar('\n');

    free(reg_busy_array);
    free(name_array);
    free(num_array);
    fclose(fp);
}

void Set_Var_Num_Array(Node_t* node, var_info* name_array, bool* inside_func, num_info* num_array, int* free_index_num_array, bool* reg_busy_array)
{
    if (node->value.op_code_t == FUNC_INIT_CODE)
    {
        *inside_func = true;
        name_array[node->left->value.name_ind].func_adress = node->right;
        Ban_Regs_From_Calls(node->right, reg_busy_array, name_array, false);
    }

    else if (node->type == NAME_CODE && node->parent->value.op_code_t != FUNC_INIT_CODE && node->parent->value.op_code_t != FUNC_CALL_CODE)
    {
        if (*inside_func == false)
            name_array[node->value.name_ind].scope = 'g'; //global var
        else
        {
            name_array[node->value.name_ind].scope = 'l'; //local var
            name_array[node->value.name_ind].reg = Free_Reg_Search(reg_busy_array);
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
        Set_Var_Num_Array(node->left, name_array, inside_func, num_array, free_index_num_array, reg_busy_array);

    if (node->right != NULL)
        Set_Var_Num_Array(node->right, name_array, inside_func, num_array, free_index_num_array, reg_busy_array);

    if (node->value.op_code_t == FUNC_INIT_CODE)
    {
        Unlock_Regs(node->right, reg_busy_array, name_array);
        *inside_func = false;
    }
}

void Ban_Regs_From_Calls(Node_t* node, bool* reg_busy_array, var_info* name_array, bool inside_call)
{
    if (node->value.op_code_t == FUNC_CALL_CODE)
        Ban_Regs_From_Calls(name_array[node->left->value.name_ind].func_adress, reg_busy_array, name_array, true);
    
    else if (node->value.op_code_t == VAR_INIT_CODE && inside_call)
        reg_busy_array[name_array[node->left->value.name_ind].reg] = true;

    if (node->left != NULL)
        Ban_Regs_From_Calls(node->left, reg_busy_array, name_array, inside_call);

    if (node->right != NULL)
        Ban_Regs_From_Calls(node->right, reg_busy_array, name_array, inside_call);
}

void Unlock_Regs(Node_t* node, bool* reg_busy_array, var_info* name_array)
{
    if (node->value.op_code_t == FUNC_CALL_CODE)
        Unlock_Regs(name_array[node->left->value.name_ind].func_adress, reg_busy_array, name_array);
    
    else if (node->value.op_code_t == VAR_INIT_CODE)
        reg_busy_array[name_array[node->left->value.name_ind].reg] = false;

    if (node->left != NULL)
        Unlock_Regs(node->left, reg_busy_array, name_array);

    if (node->right != NULL)
        Unlock_Regs(node->right, reg_busy_array, name_array);
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

int Free_Reg_Search(bool* reg_busy_array)
{
    for (int i = 0; i < NUM_AVAILABLE_REGS; i++)
    {
        if (reg_busy_array[i] == false)
            return i;
    }

    fprintf(stderr, "ERROR in file: %s, function: %s, line: %d\n", __FILE__, __func__, __LINE__);
    return -1;
}

int Node_Processing(Node_t* node, var_info* name_array, int num_name, num_info* num_array, int num_const_num, FILE* fp)
{
    if (node->type == BODY_CODE || node->type == TREE_ROOT_CODE)
        Node_Processing(node->right, name_array, num_name, num_array, num_const_num, fp);

    switch (node->value.op_code_t)
    {
        case FUNC_INIT_CODE:
            fprintf(fp, "JMP .%p\n", node);
            fprintf(fp, ".func%d:\n", node->left->value.name_ind);
            Node_Processing(node->right, name_array, num_name, num_array, num_const_num, fp);
            fprintf(fp, "RET\n");
            fprintf(fp, ".%p:\n", node);
            return 0;

        case FUNC_CALL_CODE:
            fprintf(fp, "CALL .func%d\n", node->left->value.name_ind);
            return 0;

        default:
            break;
    }

    if (node->type == NUM_CODE)
    {
        for (int pos_num_array = 0; pos_num_array < num_const_num; pos_num_array++)
        {
            if (Is_Zero(num_array[pos_num_array].num - node->value.num_t))
            {
                fprintf(fp, "[const_%d]", pos_num_array);
                return 0;
            }
        }

        fprintf(stderr, "ERROR in file: %s, function: %s, line: %d\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    else if (node->type == NAME_CODE)
    {
        if (name_array[node->value.name_ind].scope == 'g')
        {
            fprintf(fp, "[var%d]", node->value.name_ind);
            return 0;
        }
        
        else if (name_array[node->value.name_ind].scope == 'l')
        {
            fprintf(fp, "xmm%d", name_array[node->value.name_ind].reg);
            return 0;
        }

        fprintf(stderr, "ERROR in file: %s, function: %s, line: %d\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    if (node->left != NULL)
        Node_Processing(node->left, name_array, num_name, num_array, num_const_num, fp);
    
    if (node->right != NULL && (node->type != BODY_CODE && node->type != TREE_ROOT_CODE))
        Node_Processing(node->right, name_array, num_name, num_array, num_const_num, fp);

    return 0;
}