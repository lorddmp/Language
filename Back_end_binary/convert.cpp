#include "convert.h"
#include "asm_binary_defines.h"

#include <stdlib.h>
#include <tech_func.h>

#define SEGMENT_SIZE 4096
#define SEGMENT_BASE_ADRESS 0x400000

#define ELF_HEADER_SIZE 64
#define PROGRAM_HEADER_SIZE 56
#define INPUT_DATA_SIZE 81
#define PRINTF_DATA_SIZE 282
#define DATA_SHIFT ELF_HEADER_SIZE + PROGRAM_HEADER_SIZE + INPUT_DATA_SIZE + PRINTF_DATA_SIZE

#define INPUT_CODE_SIZE 214
#define PRINTF_CODE_SIZE 340
#define FUNC_SHIFT 2048
#define CODE_SHIFT FUNC_SHIFT + PRINTF_CODE_SIZE + INPUT_CODE_SIZE

#define NUM_AVAILABLE_REGS 14
#define NUM_ARITHMETIC_FUNCS 4
#define NUM_LOG_FUNCS 6

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

struct str_arithm {
    oper_codes op_code;
    int cmd_code;
};

struct str_log {
    oper_codes op_code;
    char cmd_code;
};

str_arithm arithm_codes[NUM_ARITHMETIC_FUNCS] = {
    {ADD_CODE,      0xF2440F58},
    {SUB_CODE,      0xF2440F5C},
    {MUL_CODE,      0xF2440F59},
    {DIV_CODE,      0xF2440F5E},
};

str_log log_codes[NUM_LOG_FUNCS] = {
    {DOUBLE_EQ_CODE,    0x74},
    {NOT_EQ_CODE,       0x75},
    {MORE_CODE,         0x72},
    {MORE_OR_EQ_CODE,   0x76},
    {LESS_CODE,         0x77},
    {LESS_OR_EQ_CODE,   0x73},
};

void Set_Headers(char* byte_array);

void Set_Var_Num_Array(Node_t* node, var_info* name_array, bool* inside_func, num_info* num_array, int* free_index_num_array, int* free_index_data, bool* reg_busy_array);

void Ban_Regs_From_Calls(Node_t* node, bool* reg_busy_array, var_info* name_array, bool inside_call, Node_t* first_func);
void Unlock_Regs(Node_t* node, bool* reg_busy_array, var_info* name_array, Node_t* first_func);

void Set_Data(char* byte_array, num_info* num_array, int num_const_num);

void Set_InOutput_Funcs(char* byte_array);

int Node_Processing(Node_t* node, char* byte_array,  var_info* name_array, int num_name, num_info* num_array, int num_const_num, int* free_index_code, FILE* fp);
int Free_Reg_Search(bool* reg_busy_array);

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
    int free_index_data = DATA_SHIFT;
    int free_index_code = CODE_SHIFT;

    num_array[free_index_num_array].num = 1;       //necessary constant
    num_array[free_index_num_array].adr_data = free_index_data;
    free_index_data += 8;
    free_index_num_array++;

    Set_Var_Num_Array(root_node, name_array, &inside_func, num_array, &free_index_num_array, &free_index_data, reg_busy_array);

    Set_Data(byte_array, num_array, free_index_num_array);
    Set_InOutput_Funcs(byte_array);

    Node_Processing(root_node, byte_array, name_array, num_name, num_array, num_const_num, &free_index_code, fp);

    fwrite(byte_array, sizeof(char), SEGMENT_SIZE, fp);

    free(reg_busy_array);
    free(name_array);
    free(num_array);
    fclose(fp);
}

int Node_Processing(Node_t* node, char* byte_array, var_info* name_array, int num_name, num_info* num_array, int num_const_num, int* free_index_code, FILE* fp)
{
    int start_pos_byte_array = *free_index_code;

    if (node->type == BODY_CODE || node->type == TREE_ROOT_CODE)
        Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);

    else if (node->type == OPER_CODE)
    {
        switch (node->value.op_code_t)
        {
            case FUNC_INIT_CODE:
                byte_array[*free_index_code] = JMP_CODE;
                (*free_index_code)++;

                int index_for_shift = *free_index_code;
                (*free_index_code) += 4;

                Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);

                byte_array[*free_index_code] = RET_CODE;
                (*free_index_code)++;

                *(int*)(byte_array + index_for_shift) = *free_index_code - index_for_shift;
                
                return *free_index_code - start_pos_byte_array;

            case VAR_INIT_CODE:
            case CHANGE_VAR_CODE:
                Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);
                POPR_XMM14
                if (name_array[node->left->value.name_ind].scope == 'g')
                    {MOVSD_VAR_XMM14(name_array[node->left->value.name_ind].adr_data)}
                else
                    {MOVSD_XMM_XMM14(name_array[node->left->value.name_ind].reg)}

                return *free_index_code - start_pos_byte_array;

            case FUNC_CALL_CODE:
                CALL_FUNC(SEGMENT_BASE_ADRESS + name_array[node->left->value.name_ind].adr_data)
                return *free_index_code - start_pos_byte_array;

            case IF_CODE:
                Node_Processing(node->left, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);
                POPR_XMM14
                XORPD_XMM15_XMM15
                COMISD_XMM14_XMM15

                byte_array[*free_index_code] = JE_CODE;
                (*free_index_code)++;

                int index_for_shift = *free_index_code;
                (*free_index_code) += 4;

                Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);

                *(int*)(byte_array + index_for_shift) = *free_index_code - index_for_shift;
                
                return *free_index_code - start_pos_byte_array;

            case WHILE_CODE:
                int index_for_shift_jmp = *free_index_code;

                Node_Processing(node->left, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);
                POPR_XMM14
                XORPD_XMM15_XMM15
                COMISD_XMM14_XMM15

                byte_array[*free_index_code] = JE_CODE;
                (*free_index_code)++;
                int index_for_shift_je = *free_index_code;
                (*free_index_code) += 4;

                Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);
                byte_array[*free_index_code] = JMP_CODE;
                (*free_index_code)++;

                *(int*)(byte_array + *free_index_code) = index_for_shift_jmp - *free_index_code;
                *(int*)(byte_array + index_for_shift_je) = *free_index_code - index_for_shift_je;
                return *free_index_code - start_pos_byte_array;

            case PRINTF_CODE:
                Node_Processing(node->left, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);
                POPR_XMM14
                CALL_PRINTF
                return *free_index_code - start_pos_byte_array;

            case INPUT_CODE:
                CALL_INPUT
                if (name_array[node->left->value.name_ind].scope == 'g')
                    {MOVSD_VAR_XMM14(name_array[node->left->value.name_ind].adr_data)} 
                else
                    {MOVSD_XMM_XMM14(name_array[node->left->value.name_ind].reg)}

                return *free_index_code - start_pos_byte_array;

            default:
                break;
        }

        for (int i = 0; i < NUM_ARITHMETIC_FUNCS; i++)
            if (node->value.op_code_t == arithm_codes[i].op_code)
                ARITHMETIC_FUNCS(arithm_codes[i].cmd_code)

        for (int i = 0; i < NUM_LOG_FUNCS; i++)
            if (node->value.op_code_t == log_codes[i].op_code)
                LOG_FUNCS(log_codes[i].cmd_code)
    }

    else if (node->type == NUM_CODE)
    {
        for (int pos_num_array = 0; pos_num_array < num_const_num; pos_num_array++)
        {
            if (Is_Zero(num_array[pos_num_array].num - node->value.num_t))
            {
                MOVSD_XMM14_VAR_OR_CONST_NUM(num_array[pos_num_array].adr_data)
                PUSHR_XMM(14)
                return *free_index_code - start_pos_byte_array;
            }
        }

        fprintf(stderr, "ERROR in file: %s, function: %s, line: %d\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    else if (node->type == NAME_CODE)
    {
        if (name_array[node->value.name_ind].scope == 'g')
        {
            MOVSD_XMM14_VAR_OR_CONST_NUM(name_array[node->value.name_ind].adr_data)
            PUSHR_XMM(14)
            return *free_index_code - start_pos_byte_array;
        }
            
        else if (name_array[node->value.name_ind].scope == 'l')
        {   
            PUSHR_XMM(name_array[node->value.name_ind].reg)
            return *free_index_code - start_pos_byte_array;
        }

        fprintf(stderr, "ERROR in file: %s, function: %s, line: %d\n", __FILE__, __func__, __LINE__);
        return 1;
    }

    if (node->left != NULL)
        Node_Processing(node->left, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);
        
    if (node->right != NULL && (node->type != BODY_CODE && node->type != TREE_ROOT_CODE))
        Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);

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
        name_array[node->value.name_ind].adr_data = *free_index_data;
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

void Set_InOutput_Funcs(char* byte_array)
{
    FILE* fp_input = fopen("Back_end_binary/input_code.o", "r");
    fread(&byte_array[FUNC_SHIFT], sizeof(char), INPUT_CODE_SIZE, fp_input);
    fclose(fp_input);

    FILE* fp_printf = fopen("Back_end_binary/printf_code.o", "r");
    fread(&byte_array[FUNC_SHIFT + INPUT_CODE_SIZE], sizeof(char), PRINTF_CODE_SIZE, fp_printf);
    fclose(fp_printf);
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