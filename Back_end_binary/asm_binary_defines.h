#ifndef ASM_BINARY_DEFINES
#define ASM_BINARY_DEFINES

#define JE_CODE 0x74
#define JMP_CODE 0xE9
#define RET_CODE 0xC3

#define ADD_RSP_8           *(int*)(byte_array + *free_index_code) = 0x4883C408;        \
                            (*free_index_code) += 4;

#define SUB_RSP_8           *(int*)(byte_array + *free_index_code) = 0x4883EC08;        \
                            (*free_index_code) += 4;

#define PUSHR_XMM(reg)      SUB_RSP_8                                                       \
                            if (reg < 8)                                                    \
                            {                                                               \
                                *(byte_array + *free_index_code) = 0xF2;                    \
                                (*free_index_code)++;                                       \
                                *(short*)(byte_array + *free_index_code) = 0x0F11;          \
                                (*free_index_code) += 2;                                    \
                                *(byte_array + *free_index_code) = 0x04 + 8*reg;            \
                                (*free_index_code)++;                                       \
                            }                                                               \
                            else                                                            \
                            {                                                               \
                                *(int*)(byte_array + *free_index_code) = 0xF2440F11;        \
                                (*free_index_code) += 4;                                    \
                                *(byte_array + *free_index_code) = 0x04 + 8*(reg-8);        \
                                (*free_index_code)++;                                       \
                            }                                                               \
                                                                                            \
                            *(byte_array + *free_index_code) = 0x24;                        \
                            (*free_index_code)++;

#define POPR_XMM14          *(int*)(byte_array + *free_index_code) = 0xF2440F10;        \
                            (*free_index_code) += 4;                                    \
                            *(short*)(byte_array + *free_index_code) = 0x3424;          \
                            (*free_index_code) += 2;                                    \
                            ADD_RSP_8

#define POPR_XMM15          *(int*)(byte_array + *free_index_code) = 0xF2440F10;        \
                            (*free_index_code) += 4;                                    \
                            *(short*)(byte_array + *free_index_code) = 0x3C24;          \
                            (*free_index_code) += 2;                                    \
                            ADD_RSP_8

#define MOVSD_XMM14_1       *(int*)(byte_array + *free_index_code) = 0xF2440F10;                              \
                            (*free_index_code) += 4;                                                          \
                            *(short*)(byte_array + *free_index_code) = 0x3425;                                \
                            (*free_index_code) += 2;                                                          \
                            *(int*)(byte_array + *free_index_code) = SEGMENT_BASE_ADRESS + DATA_SHIFT;        \
                            (*free_index_code) += 4;

#define MOVSD_XMM14_VAR_OR_CONST_NUM(adr)                                                               \
                            *(int*)(byte_array + *free_index_code) = 0xF2440F10;                       \
                            (*free_index_code) += 4;                                                   \
                            *(short*)(byte_array + *free_index_code) = 0x3425;                         \
                            (*free_index_code) += 2;                                                   \
                            *(int*)(byte_array + *free_index_code) = SEGMENT_BASE_ADRESS + adr;        \
                            (*free_index_code) += 4;

#define MOVSD_VAR_XMM14(adr) *(int*)(byte_array + *free_index_code) = 0xF2440F11;                      \
                            (*free_index_code) += 4;                                                   \
                            *(short*)(byte_array + *free_index_code) = 0x3425;                         \
                            (*free_index_code) += 2;                                                   \
                            *(int*)(byte_array + *free_index_code) = SEGMENT_BASE_ADRESS + adr;        \
                            (*free_index_code) += 4;

#define MOVSD_XMM_XMM14(reg)                                                                \
                            *(byte_array + *free_index_code) = 0xF7;                        \
                            (*free_index_code)++;                                           \
                                                                                            \
                            if (reg < 8)                                                    \
                                *(byte_array + *free_index_code) = 0x41;                    \
                            else                                                            \
                                *(byte_array + *free_index_code) = 0x45;                    \
                                                                                            \
                            (*free_index_code)++;                                           \
                            *(short*)(byte_array + *free_index_code) = 0x0F10;              \
                            (*free_index_code) += 2;                                        \
                                                                                            \
                            if (reg < 8)                                                    \
                                *(byte_array + *free_index_code) = 0xC6 + 8*reg;            \
                            else                                                            \
                                *(byte_array + *free_index_code) = 0xC6 + 8*(reg-8);        \
                                                                                            \
                            (*free_index_code)++;

#define XORPD_XMM14_XMM14   *(int*)(byte_array + *free_index_code) = 0x450F57F6;        \
                            (*free_index_code) += 4;                                    

#define XORPD_XMM15_XMM15   *(int*)(byte_array + *free_index_code) = 0x450F57FF;        \
                            (*free_index_code) += 4;

#define COMISD_XMM14_XMM15  *(int*)(byte_array + *free_index_code) = 0x66450F2F;        \
                            (*free_index_code) += 4;                                    \
                            *(byte_array + *free_index_code) = 0xF7;                    \
                            (*free_index_code)++;

#define CALL_INPUT          *(byte_array + *free_index_code) = 0xB8;                  \
                            (*free_index_code)++;                                     \
                            *(int*)(byte_array + *free_index_code) = 0x400800;        \
                            (*free_index_code) += 4;                                  \
                            *(short*)(byte_array + *free_index_code) = 0xFFD0;        \
                            (*free_index_code) += 2;                                  \
                            (*free_index_code) += INPUT_CODE_SIZE;

#define CALL_PRINTF         *(byte_array + *free_index_code) = 0xB8;                  \
                            (*free_index_code)++;                                     \
                            *(int*)(byte_array + *free_index_code) = 0x4008D6;        \
                            (*free_index_code) += 4;                                  \
                            *(short*)(byte_array + *free_index_code) = 0xFFD0;        \
                            (*free_index_code) += 2;                                  \
                            (*free_index_code) += PRINTF_CODE_SIZE;

#define CALL_FUNC(func_adr) *(byte_array + *free_index_code) = 0xB8;                  \
                            (*free_index_code)++;                                     \
                            *(int*)(byte_array + *free_index_code) = func_adr;        \
                            (*free_index_code) += 4;                                  \
                            *(short*)(byte_array + *free_index_code) = 0xFFD0;        \
                            (*free_index_code) += 2;

#define ARITHMETIC_FUNCS(cmd_code)                                                                                        \
{                                                                                                                         \
    Node_Processing(node->left, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);         \
    Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);        \
    POPR_XMM15                                                                                                            \
    POPR_XMM14                                                                                                            \
    *(int*)(byte_array + *free_index_code) = cmd_code;                                                                    \
    (*free_index_code) += 4;                                                                                              \
    *(byte_array + *free_index_code) = 0xF7;                                                                               \
    (*free_index_code)++;                                                                                                 \
    PUSHR_XMM(14)                                                                                                           \
    return *free_index_code - start_pos_byte_array;                                                                                                             \
}

#define LOG_FUNCS(cmd_code)                                                                                               \
{                                                                                                                         \
    Node_Processing(node->left, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);         \
    Node_Processing(node->right, byte_array, name_array, num_name, num_array, num_const_num, free_index_code, fp);        \
    POPR_XMM15                                                                                                            \
    POPR_XMM14                                                                                                            \
    COMISD_XMM14_XMM15                                                                                                    \
    XORPD_XMM14_XMM14                                                                                                     \
    *(byte_array + *free_index_code) = cmd_code;                                                                          \
    (*free_index_code)++;                                                                                                 \
    *(byte_array + *free_index_code) = 0x0A;                                                                              \
    (*free_index_code)++;                                                                                                 \
    MOVSD_XMM14_1                                                                                                         \
    PUSHR_XMM(14)                                                                                                           \
    return *free_index_code - start_pos_byte_array;                                                                                                             \
}

#endif