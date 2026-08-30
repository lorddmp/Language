#include "convert.h"

#include <stdlib.h>
#include <tech_func.h>

#define SEGMENT_SIZE 4096
#define SEGMENT_BASE_ADRESS 0x400000

#define ELF_HEADER_SIZE 64
#define PROGRAM_HEADER_SIZE 56
#define INPUT_DATA_SIZE 81
#define PRINTF_DATA_SIZE 282
#define OVERALL_SHIFT ELF_HEADER_SIZE + PROGRAM_HEADER_SIZE + INPUT_DATA_SIZE + PRINTF_DATA_SIZE

#define NUM_AVAILABLE_REGS 14
#define NUM_ARITHMETIC_FUNCS 4
#define NUM_LOG_FUNCS 6

#define ARITHMETIC_FUNCS(operation)                                                          \
{                                                                                            \
    Node_Processing(node->left, name_array, num_name, num_array, num_const_num, fp);         \
    Node_Processing(node->right, name_array, num_name, num_array, num_const_num, fp);        \
    fprintf(fp, "movsd xmm15, [rsp]\n");                                                     \
    fprintf(fp, "add rsp, 8\n");                                                             \
    fprintf(fp, "movsd xmm14, [rsp]\n");                                                     \
    fprintf(fp, "%s xmm14, xmm15\n", operation);                                             \
    fprintf(fp, "movsd [rsp], xmm14\n");                                                     \
    return 0;                                                                                \
}

#define LOG_FUNCS(operation)                                                                 \
{                                                                                            \
    Node_Processing(node->left, name_array, num_name, num_array, num_const_num, fp);         \
    Node_Processing(node->right, name_array, num_name, num_array, num_const_num, fp);        \
    fprintf(fp, "movsd xmm15, [rsp]\n");                                                     \
    fprintf(fp, "add rsp, 8\n");                                                             \
    fprintf(fp, "movsd xmm14, [rsp]\n");                                                     \
    fprintf(fp, "add rsp, 8\n");                                                             \
    fprintf(fp, "comisd xmm14, xmm15\n");                                                    \
    fprintf(fp, "xorpd xmm14, xmm14\n");                                                     \
    fprintf(fp, "%s _mark%p\n", operation, node->left);                                      \
    fprintf(fp, "movsd xmm14, [const_0]\n");                                                 \
    fprintf(fp, "_mark%p:\n", node->left);                                                   \
    fprintf(fp, "sub rsp, 8\n");                                                             \
    fprintf(fp, "movsd [rsp], xmm14\n");                                                     \
    return 0;                                                                                \
}

struct var_info {
    char scope;
    int reg;
    Node_t* func_adress;
    long adr_data;
};

struct num_info {
    data_t num;
    long adr_data;
};

struct str {
    oper_codes op_code;
    const char* cmd_name;
};

str arithm_codes[NUM_ARITHMETIC_FUNCS] = {
    {ADD_CODE,      "addsd"},
    {SUB_CODE,      "subsd"},
    {MUL_CODE,      "mulsd"},
    {DIV_CODE,      "divsd"},
};

str log_codes[NUM_LOG_FUNCS] = {
    {DOUBLE_EQ_CODE,    "je"},
    {NOT_EQ_CODE,       "jne"},
    {MORE_CODE,         "jb"},
    {MORE_OR_EQ_CODE,   "jbe"},
    {LESS_CODE,         "ja"},
    {LESS_OR_EQ_CODE,   "jae"},
};

void Set_Headers(char* byte_array);

void Set_Var_Num_Array(Node_t* node, var_info* name_array, bool* inside_func, num_info* num_array, int* free_index_num_array, int* free_index_data, bool* reg_busy_array);

void Ban_Regs_From_Calls(Node_t* node, bool* reg_busy_array, var_info* name_array, bool inside_call, Node_t* first_func);
void Unlock_Regs(Node_t* node, bool* reg_busy_array, var_info* name_array, Node_t* first_func);

void Set_Data(char* byte_array, num_info* num_array, int num_const_num);

int Free_Reg_Search(bool* reg_busy_array);
int Node_Processing(Node_t* node, var_info* name_array, int num_name, num_info* num_array, int num_const_num, FILE* fp);

void Converting(Node_t* root_node, int num_name, int num_const_num)
{
    FILE* fp = fopen("Back_end_binary/proga", "w");

    char byte_array[SEGMENT_SIZE] = {};
    Set_Headers(byte_array);

    num_const_num++;
    var_info* name_array = (var_info*)calloc((size_t)num_name, sizeof(var_info));
    num_info* num_array = (num_info*)calloc((size_t)num_const_num, sizeof(num_info));
    bool* reg_busy_array = (bool*)calloc(NUM_AVAILABLE_REGS, sizeof(bool));

    bool inside_func = false;
    int free_index_num_array = 0;
    int free_index_data = OVERALL_SHIFT;

    num_array[free_index_num_array].num = 1;       //necessary constant
    num_array[free_index_num_array].adr_data = free_index_data;
    free_index_data += 8;
    free_index_num_array++;

    Set_Var_Num_Array(root_node, name_array, &inside_func, num_array, &free_index_num_array, &free_index_data, reg_busy_array);

    for (int i = 0; i < free_index_num_array; i++)
        printf("%lg/%ld ", num_array[i].num, num_array[i].adr_data);

    Set_Data(byte_array, num_array, free_index_num_array);

    // Node_Processing(root_node, name_array, num_name, num_array, num_const_num, fp);

    fwrite(byte_array, sizeof(char), SEGMENT_SIZE, fp);

    free(reg_busy_array);
    free(name_array);
    free(num_array);
    fclose(fp);
}

int Node_Processing(Node_t* node, var_info* name_array, int num_name, num_info* num_array, int num_const_num, FILE* fp)
{
    if (node->type == BODY_CODE || node->type == TREE_ROOT_CODE)
        Node_Processing(node->right, name_array, num_name, num_array, num_const_num, fp);

    if (node->type == OPER_CODE)
    {
        switch (node->value.op_code_t)
        {
            case FUNC_INIT_CODE:
                fprintf(fp, "jmp _mark%p\n", node);
                fprintf(fp, "func%d:\n", node->left->value.name_ind);
                Node_Processing(node->right, name_array, num_name, num_array, num_const_num, fp);
                fprintf(fp, "ret\n");
                fprintf(fp, "_mark%p:\n", node);
                return 0;

            case VAR_INIT_CODE:
            case CHANGE_VAR_CODE:
                Node_Processing(node->right, name_array, num_name, num_array, num_const_num, fp);
                fprintf(fp, "movsd xmm14, [rsp]\n");
                fprintf(fp, "add rsp, 8\n");
                if (name_array[node->left->value.name_ind].scope == 'g')
                    fprintf(fp, "movsd [var%d], xmm14\n", node->left->value.name_ind);
                else
                    fprintf(fp, "movsd xmm%d, xmm14\n", name_array[node->left->value.name_ind].reg);

                return 0;

            case FUNC_CALL_CODE:
                fprintf(fp, "call func%d\n", node->left->value.name_ind);
                return 0;

            case IF_CODE:
                Node_Processing(node->left, name_array, num_name, num_array, num_const_num, fp);
                fprintf(fp, "movsd xmm14, [rsp]\n");
                fprintf(fp, "add rsp, 8\n");
                fprintf(fp, "xorpd xmm15, xmm15\n");
                fprintf(fp, "comisd xmm14, xmm15\n");
                fprintf(fp, "je _mark%p\n", node);
                Node_Processing(node->right, name_array, num_name, num_array, num_const_num, fp);
                fprintf(fp, "_mark%p:\n", node);
                return 0;

            case WHILE_CODE:
                fprintf(fp, "_mark%p:\n", node->left);
                Node_Processing(node->left, name_array, num_name, num_array, num_const_num, fp);
                fprintf(fp, "movsd xmm14, [rsp]\n");
                fprintf(fp, "add rsp, 8\n");
                fprintf(fp, "xorpd xmm15, xmm15\n");
                fprintf(fp, "comisd xmm14, xmm15\n");
                fprintf(fp, "je _mark%p\n", node);
                Node_Processing(node->right, name_array, num_name, num_array, num_const_num, fp);
                fprintf(fp, "jmp _mark%p\n", node->left);
                fprintf(fp, "_mark%p:\n", node);
                return 0;

            case PRINTF_CODE:
                Node_Processing(node->left, name_array, num_name, num_array, num_const_num, fp);
                fprintf(fp, "movsd xmm14, [rsp]\n");
                fprintf(fp, "add rsp, 8\n");
                fprintf(fp, "call my_printf_float\n");
                return 0;

            case INPUT_CODE:
                fprintf(fp, "call my_input_float\n");
                if (name_array[node->left->value.name_ind].scope == 'g')
                    fprintf(fp, "movsd [var%d], xmm14\n", node->left->value.name_ind);
                else
                    fprintf(fp, "movsd xmm%d, xmm14\n", name_array[node->left->value.name_ind].reg);

                return 0;

            default:
                break;
        }

        for (int i = 0; i < NUM_ARITHMETIC_FUNCS; i++)
            if (node->value.op_code_t == arithm_codes[i].op_code)
                ARITHMETIC_FUNCS(arithm_codes[i].cmd_name)

        for (int i = 0; i < NUM_LOG_FUNCS; i++)
            if (node->value.op_code_t == log_codes[i].op_code)
                LOG_FUNCS(log_codes[i].cmd_name)
    }

    if (node->type == NUM_CODE)
    {
        for (int pos_num_array = 0; pos_num_array < num_const_num; pos_num_array++)
        {
            if (Is_Zero(num_array[pos_num_array].num - node->value.num_t))
            {
                fprintf(fp, "movsd xmm14, [const_%d]\n", pos_num_array);
                fprintf(fp, "sub rsp, 8\n");
                fprintf(fp, "movsd [rsp], xmm14\n");
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
            fprintf(fp, "movsd xmm14, [var%d]\n", node->value.name_ind);
            fprintf(fp, "sub rsp, 8\n");
            fprintf(fp, "movsd [rsp], xmm14\n");
            return 0;
        }
            
        else if (name_array[node->value.name_ind].scope == 'l')
        {   
            fprintf(fp, "sub rsp, 8\n");
            fprintf(fp, "movsd [rsp], xmm%d\n", name_array[node->value.name_ind].reg);
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

void Set_Headers(char* byte_array)
{
    FILE* fp = fopen("Back_end_binary/ELF_header", "r");
    fread(byte_array, sizeof(char), ELF_HEADER_SIZE, fp);
    fclose(fp);

    FILE* fpp = fopen("Back_end_binary/Program_header", "r");
    fread(&byte_array[ELF_HEADER_SIZE], sizeof(char), PROGRAM_HEADER_SIZE, fpp);
    fclose(fpp);
}

void Set_Var_Num_Array(Node_t* node, var_info* name_array, bool* inside_func, num_info* num_array, int* free_index_num_array, int* free_index_data, bool* reg_busy_array)
{
    if (node->value.op_code_t == FUNC_INIT_CODE)
    {
        *inside_func = true;
        name_array[node->left->value.name_ind].func_adress = node->right;
        Ban_Regs_From_Calls(node->right, reg_busy_array, name_array, false, node->right);
    }

    else if (node->type == NAME_CODE && node->parent->value.op_code_t != FUNC_INIT_CODE && node->parent->value.op_code_t != FUNC_CALL_CODE)
    {
        if (*inside_func == false)
        {
            name_array[node->value.name_ind].scope = 'g'; //global var
            name_array[node->value.name_ind].adr_data = *free_index_data;
            (*free_index_data) += 8;
        }
        else
        {
            name_array[node->value.name_ind].scope = 'l'; //local var
            name_array[node->value.name_ind].reg = Free_Reg_Search(reg_busy_array);
        }
    }

    else if (node->type == NUM_CODE)
    {

        for (int i = 0; i < *free_index_num_array; i++)
        {
            if (Is_Zero(num_array[i].num - node->value.num_t))
                break;

            else if (i == *free_index_num_array - 1)
            {
                num_array[*free_index_num_array].num = node->value.num_t;
                num_array[*free_index_num_array].adr_data = *free_index_data;
                (*free_index_num_array)++;
                (*free_index_data) += 8;
                break;
            }
        }
    }

    if (node->right != NULL)
        Set_Var_Num_Array(node->right, name_array, inside_func, num_array, free_index_num_array, free_index_data, reg_busy_array);

    if (node->left != NULL)
        Set_Var_Num_Array(node->left, name_array, inside_func, num_array, free_index_num_array, free_index_data, reg_busy_array);

    if (node->value.op_code_t == FUNC_INIT_CODE)
    {
        Unlock_Regs(node->right, reg_busy_array, name_array, node->right);
        *inside_func = false;
    }
}

void Ban_Regs_From_Calls(Node_t* node, bool* reg_busy_array, var_info* name_array, bool inside_call, Node_t* first_func)
{
    if (node->value.op_code_t == FUNC_CALL_CODE && (!inside_call || (inside_call && name_array[node->left->value.name_ind].func_adress != first_func))) //to avoid looping
        Ban_Regs_From_Calls(name_array[node->left->value.name_ind].func_adress, reg_busy_array, name_array, true, first_func);
        
    else if (node->value.op_code_t == VAR_INIT_CODE && inside_call)
        reg_busy_array[name_array[node->left->value.name_ind].reg] = true;

    if (node->left != NULL)
        Ban_Regs_From_Calls(node->left, reg_busy_array, name_array, inside_call, first_func);

    if (node->right != NULL)
        Ban_Regs_From_Calls(node->right, reg_busy_array, name_array, inside_call, first_func);
}

void Unlock_Regs(Node_t* node, bool* reg_busy_array, var_info* name_array, Node_t* first_func)
{
    if (node->value.op_code_t == FUNC_CALL_CODE && name_array[node->left->value.name_ind].func_adress != first_func) //to avoid looping
        Unlock_Regs(name_array[node->left->value.name_ind].func_adress, reg_busy_array, name_array, first_func);
        
    else if (node->value.op_code_t == VAR_INIT_CODE)
        reg_busy_array[name_array[node->left->value.name_ind].reg] = false;

    if (node->left != NULL)
        Unlock_Regs(node->left, reg_busy_array, name_array, first_func);

    if (node->right != NULL)
        Unlock_Regs(node->right, reg_busy_array, name_array, first_func);
}

void Set_Data(char* byte_array, num_info* num_array, int num_const_num)
{
    //input_data
    FILE* fpp1 = fopen("Back_end_binary/input_data", "r");
    fread(&byte_array[ELF_HEADER_SIZE + PROGRAM_HEADER_SIZE], sizeof(char), INPUT_DATA_SIZE, fpp1);
    fclose(fpp1);

    //printf_data
    FILE* fpp2 = fopen("Back_end_binary/printf_data", "r");
    fread(&byte_array[ELF_HEADER_SIZE + PROGRAM_HEADER_SIZE + INPUT_DATA_SIZE], sizeof(char), PRINTF_DATA_SIZE, fpp2);
    fclose(fpp2);

    for (int pos_num_array = 0; pos_num_array < num_const_num; pos_num_array++)
        *(data_t*)(byte_array + num_array[pos_num_array].adr_data) = num_array[pos_num_array].num;
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