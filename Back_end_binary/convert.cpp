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

#define ERROR_IF_VALUES_EQUAL(val1, val2, file, func, line)                                  \
if (val1 == val2)                                                                            \
{                                                                                            \
    fprintf(stderr, "ERROR in file: %s, function: %s, line: %d\n", file, func, line);        \
    return -1;                                                                               \
}

#define ERROR_IF_VALUES_NOT_EQUAL(val1, val2, file, func, line)                              \
if (val1 != val2)                                                                            \
{                                                                                            \
    fprintf(stderr, "ERROR in file: %s, function: %s, line: %d\n", file, func, line);        \
    return -1;                                                                               \
}

struct var_info {
    char scope;
    int reg;
    Node_t* func_adress;
    int adr_data;
};

struct num_info {
    data_t num;
    int adr_data;
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
    {ADD_CODE,      0x580F45F2},
    {SUB_CODE,      0x5C0F45F2},
    {MUL_CODE,      0x590F45F2},
    {DIV_CODE,      0x5E0F45F2},
};

str_log log_codes[NUM_LOG_FUNCS] = {
    {DOUBLE_EQ_CODE,    0x75},
    {NOT_EQ_CODE,       0x74},
    {MORE_CODE,         0x76},
    {MORE_OR_EQ_CODE,   0x72},
    {LESS_CODE,         0x73},
    {LESS_OR_EQ_CODE,   0x77},
};

int Set_Headers(char* byte_array);

int Set_Var_Num_Array(Node_t* node, var_info* name_array, bool* inside_func, num_info* num_array, int* free_index_num_array, int* free_index_data, bool* reg_busy_array);

void Ban_Regs_From_Calls(Node_t* node, bool* reg_busy_array, var_info* name_array, bool inside_call, Node_t* first_func);
void Unlock_Regs(Node_t* node, bool* reg_busy_array, var_info* name_array, Node_t* first_func);

int Set_Data(char* byte_array, num_info* num_array, int num_const_num);

int Set_InOutput_Funcs(char* byte_array);

int Node_Processing(Node_t* node, char* byte_array,  var_info* name_array, int num_name, num_info* num_array, int num_const_num, int* free_index_code, FILE* fp);
int Free_Reg_Search(bool* reg_busy_array);

int Converting(Node_t* root_node, int num_name, int num_const_num)
{
    FILE* fp = fopen("Back_end_binary/proga", "w");

    char byte_array[SEGMENT_SIZE] = {};
    ERROR_IF_VALUES_EQUAL(Set_Headers(byte_array), -1, __FILE__, __func__, __LINE__)

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

    ERROR_IF_VALUES_EQUAL(Set_Var_Num_Array(root_node, name_array, &inside_func, num_array, &free_index_num_array, &free_index_data, reg_busy_array), -1, __FILE__, __func__, __LINE__)
    ERROR_IF_VALUES_EQUAL(Set_Data(byte_array, num_array, free_index_num_array), -1, __FILE__, __func__, __LINE__)
    ERROR_IF_VALUES_EQUAL(Set_InOutput_Funcs(byte_array), -1, __FILE__, __func__, __LINE__)

    ERROR_IF_VALUES_EQUAL(Node_Processing(root_node, byte_array, name_array, num_name, num_array, num_const_num, &free_index_code, fp), -1, __FILE__, __func__, __LINE__)

    EXIT_PROGRAM

    fwrite(byte_array, sizeof(char), SEGMENT_SIZE, fp);

    free(reg_busy_array);
    free(name_array);
    free(num_array);
    fclose(fp);

    return 0;
}

int Node_Processing(Node_t* node, char* byte_array, var_info* name_array, int num_name, num_info* num_array, int num_const_num, int* free_index_code, FILE* fp)
{
    int start_pos_byte_array = *free_index_code;

    if (node->type == BODY_CODE || node->type == TREE_ROOT_CODE)
        {ERROR_IF_VALUES_EQUAL(Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp), -1, __FILE__, __func__, __LINE__)}

    else if (node->type == OPER_CODE)
    {
        switch (node->value.op_code_t)
        {
            case FUNC_INIT_CODE:
            {
                byte_array[*free_index_code] = JMP_CODE;
                (*free_index_code)++;

                int index_for_shift = *free_index_code;
                (*free_index_code) += 4;

                ERROR_IF_VALUES_EQUAL(Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp), -1, __FILE__, __func__, __LINE__)

                byte_array[*free_index_code] = RET_CODE;
                (*free_index_code)++;

                *(unsigned int*)(byte_array + index_for_shift) = *free_index_code - index_for_shift;
                
                return *free_index_code - start_pos_byte_array;
            }

            case VAR_INIT_CODE:
            case CHANGE_VAR_CODE:
            {
                ERROR_IF_VALUES_EQUAL(Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp), -1, __FILE__, __func__, __LINE__)
                POPR_XMM14
                if (name_array[node->left->value.name_ind].scope == 'g')
                    {MOVSD_VAR_XMM14(name_array[node->left->value.name_ind].adr_data)}
                else
                    {MOVSD_XMM_XMM14(name_array[node->left->value.name_ind].reg)}

                return *free_index_code - start_pos_byte_array;
            }

            case FUNC_CALL_CODE:
            {
                CALL_FUNC(SEGMENT_BASE_ADRESS + name_array[node->left->value.name_ind].adr_data)
                return *free_index_code - start_pos_byte_array;
            }

            case IF_CODE:
            {
                ERROR_IF_VALUES_EQUAL(Node_Processing(node->left, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp), -1, __FILE__, __func__, __LINE__)
                POPR_XMM14
                XORPD_XMM15_XMM15
                COMISD_XMM14_XMM15

                *(short*)(byte_array + *free_index_code) = JE_CODE;
                (*free_index_code) += 2;

                int index_for_shift_if = *free_index_code;
                (*free_index_code) += 4;

                ERROR_IF_VALUES_EQUAL(Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp), -1, __FILE__, __func__, __LINE__)

                *(unsigned int*)(byte_array + index_for_shift_if) = *free_index_code - index_for_shift_if - 4;
                
                return *free_index_code - start_pos_byte_array;
            }

            case WHILE_CODE:
            {
                int index_for_shift_jmp_while = *free_index_code;

                ERROR_IF_VALUES_EQUAL(Node_Processing(node->left, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp), -1, __FILE__, __func__, __LINE__)
                POPR_XMM14
                XORPD_XMM15_XMM15
                COMISD_XMM14_XMM15

                *(short*)(byte_array + *free_index_code) = 0x840F; // je
                (*free_index_code) += 2;
                int index_for_shift_je_while = *free_index_code;
                (*free_index_code) += 4;

                ERROR_IF_VALUES_EQUAL(Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp), -1, __FILE__, __func__, __LINE__)
                byte_array[*free_index_code] = JMP_CODE;
                (*free_index_code)++;

                *(unsigned int*)(byte_array + *free_index_code) = index_for_shift_jmp_while - *free_index_code - 4;
                (*free_index_code) += 4;
                *(unsigned int*)(byte_array + index_for_shift_je_while) = *free_index_code - index_for_shift_je_while - 4;
                return *free_index_code - start_pos_byte_array;
            }

            case PRINTF_CODE:
            {
                ERROR_IF_VALUES_EQUAL(Node_Processing(node->left, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp), -1, __FILE__, __func__, __LINE__)
                POPR_XMM14
                CALL_PRINTF
                return *free_index_code - start_pos_byte_array;
            }

            case INPUT_CODE:
            {
                CALL_INPUT
                if (name_array[node->left->value.name_ind].scope == 'g')
                    {MOVSD_VAR_XMM14(name_array[node->left->value.name_ind].adr_data)} 
                else
                    {MOVSD_XMM_XMM14(name_array[node->left->value.name_ind].reg)}

                return *free_index_code - start_pos_byte_array;
            }

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
        return -1;
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
        return -1;
    }

    if (node->left != NULL)
        {ERROR_IF_VALUES_EQUAL(Node_Processing(node->left, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp), -1, __FILE__, __func__, __LINE__)}
        
    if (node->right != NULL && (node->type != BODY_CODE && node->type != TREE_ROOT_CODE))
        {ERROR_IF_VALUES_EQUAL(Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp), -1, __FILE__, __func__, __LINE__)}

    return 0;
}

int Set_Headers(char* byte_array)
{
    FILE* fp_ELF = fopen("Back_end_binary/ELF_header", "r");
    ERROR_IF_VALUES_EQUAL(fp_ELF, NULL, __FILE__, __func__, __LINE__)
    ERROR_IF_VALUES_NOT_EQUAL(fread(byte_array, sizeof(char), ELF_HEADER_SIZE, fp_ELF), ELF_HEADER_SIZE, __FILE__, __func__, __LINE__);
    fclose(fp_ELF);

    FILE* fp_program = fopen("Back_end_binary/Program_header", "r");
    ERROR_IF_VALUES_EQUAL(fp_ELF, NULL, __FILE__, __func__, __LINE__)
    ERROR_IF_VALUES_NOT_EQUAL(fread(&byte_array[ELF_HEADER_SIZE], sizeof(char), PROGRAM_HEADER_SIZE, fp_program), PROGRAM_HEADER_SIZE, __FILE__, __func__, __LINE__);
    fclose(fp_program);

    return 0;
}

int Set_Var_Num_Array(Node_t* node, var_info* name_array, bool* inside_func, num_info* num_array, int* free_index_num_array, int* free_index_data, bool* reg_busy_array)
{
    if (node->value.op_code_t == FUNC_INIT_CODE)
    {
        *inside_func = true;
        name_array[node->left->value.name_ind].func_adress = node->right;
        name_array[node->value.name_ind].adr_data = *free_index_data;
        Ban_Regs_From_Calls(node->right, reg_busy_array, name_array, false, node->right);
    }

    else if (node->value.op_code_t == VAR_INIT_CODE)
    {
        if (*inside_func == false)
        {
            name_array[node->left->value.name_ind].scope = 'g'; //global var
            name_array[node->left->value.name_ind].adr_data = *free_index_data;
            (*free_index_data) += 8;
        }
        else
        {
            name_array[node->left->value.name_ind].scope = 'l'; //local var
            name_array[node->left->value.name_ind].reg = Free_Reg_Search(reg_busy_array);
            ERROR_IF_VALUES_EQUAL(name_array[node->left->value.name_ind].reg, -1, __FILE__, __func__, __LINE__)
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
        {ERROR_IF_VALUES_EQUAL(Set_Var_Num_Array(node->right, name_array, inside_func, num_array, free_index_num_array, free_index_data, reg_busy_array), -1, __FILE__, __func__, __LINE__)}

    if (node->left != NULL)
        {ERROR_IF_VALUES_EQUAL(Set_Var_Num_Array(node->left, name_array, inside_func, num_array, free_index_num_array, free_index_data, reg_busy_array), -1, __FILE__, __func__, __LINE__)}

    if (node->value.op_code_t == FUNC_INIT_CODE)
    {
        Unlock_Regs(node->right, reg_busy_array, name_array, node->right);
        *inside_func = false;
    }

    return 0;
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

int Set_Data(char* byte_array, num_info* num_array, int num_const_num)
{
    FILE* fp_input_data = fopen("Back_end_binary/input_data", "r");
    ERROR_IF_VALUES_EQUAL(fp_input_data, NULL, __FILE__, __func__, __LINE__)
    ERROR_IF_VALUES_NOT_EQUAL(fread(&byte_array[ELF_HEADER_SIZE + PROGRAM_HEADER_SIZE], sizeof(char), INPUT_DATA_SIZE, fp_input_data), INPUT_DATA_SIZE, __FILE__, __func__, __LINE__);
    fclose(fp_input_data);

    FILE* fp_printf_data = fopen("Back_end_binary/printf_data", "r");
    ERROR_IF_VALUES_EQUAL(fp_printf_data, NULL, __FILE__, __func__, __LINE__)
    ERROR_IF_VALUES_NOT_EQUAL(fread(&byte_array[ELF_HEADER_SIZE + PROGRAM_HEADER_SIZE + INPUT_DATA_SIZE], sizeof(char), PRINTF_DATA_SIZE, fp_printf_data), PRINTF_DATA_SIZE, __FILE__, __func__, __LINE__);
    fclose(fp_printf_data);

    for (int pos_num_array = 0; pos_num_array < num_const_num; pos_num_array++)
        *(data_t*)(byte_array + num_array[pos_num_array].adr_data) = num_array[pos_num_array].num;

    return 0;
}

int Set_InOutput_Funcs(char* byte_array)
{
    FILE* fp_input_code = fopen("Back_end_binary/input_code.o", "r");
    ERROR_IF_VALUES_EQUAL(fp_input_code, NULL, __FILE__, __func__, __LINE__)
    ERROR_IF_VALUES_NOT_EQUAL(fread(&byte_array[FUNC_SHIFT], sizeof(char), INPUT_CODE_SIZE, fp_input_code), INPUT_CODE_SIZE, __FILE__, __func__, __LINE__);
    fclose(fp_input_code);

    FILE* fp_printf_code = fopen("Back_end_binary/printf_code.o", "r");
    ERROR_IF_VALUES_EQUAL(fp_printf_code, NULL, __FILE__, __func__, __LINE__)
    ERROR_IF_VALUES_NOT_EQUAL(fread(&byte_array[FUNC_SHIFT + INPUT_CODE_SIZE], sizeof(char), PRINTF_CODE_SIZE, fp_printf_code), PRINTF_CODE_SIZE, __FILE__, __func__, __LINE__);
    fclose(fp_printf_code);

    return 0;
}

int Free_Reg_Search(bool* reg_busy_array)
{
    for (int i = 0; i < NUM_AVAILABLE_REGS; i++)
    {
        if (reg_busy_array[i] == false)
            return i;
    }

    return -1;
}