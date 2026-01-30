#ifndef BYTECODE
#define BYTECODE

#define NAME_INPUT_FILE "Back_end/Commands.txt"
#define NAME_BYTECODE_FILE "Assembler/bytecode_file"

#include "stack.h"
#include "processor.h"
#include <stdio.h>

#define MAX_LEN_COMMAND 8
#define NUM_COMMAND 24
#define SIZE_MASSIVE 1024
#define METKA_NUM 1000

#define IF_ERROR_COMPILER(arg)        \
    if (arg)                          \
        return arg;                   \

enum
{
    HLT_CODE = 0,
    PUSH_CODE = 1,
    POP_CODE = 2,
    ADD_CODE = 3,
    SUB_CODE = 4,
    MUL_CODE = 5,
    DIV_CODE = 6,
    SQRT_CODE = 7,
    IN_CODE = 8,
    PUSHR_CODE = 33, 
    POPR_CODE = 42,
    JB_CODE = 50,
    JBE_CODE = 51,
    JA_CODE = 52,
    JAE_CODE = 53,
    JE_CODE = 54,
    JNE_CODE = 55,
    JMP_CODE = 56,
    CALL_CODE = 57,
    RET_CODE = 58,
    PUSHM_CODE = 59,
    POPM_CODE = 60,
    DRAW_CODE = 61,
    SLEEP_CODE = 62,
};

struct cmd_and_codes{
    const char* CMD;
    unsigned char CMD_CODE;
    int LEN_CMD;
};

const cmd_and_codes massive_structur[NUM_COMMAND] = {
    {"PUSH", PUSH_CODE, 4},
    {"POP", POP_CODE, 3},
    {"ADD", ADD_CODE, 3},
    {"SUB", SUB_CODE, 3},
    {"MUL", MUL_CODE, 3},
    {"DIV", DIV_CODE, 3},
    {"SQRT", SQRT_CODE, 4},
    {"IN", IN_CODE, 2},
    {"POPR", POPR_CODE, 4},
    {"HLT", HLT_CODE, 3},
    {"JAE", JAE_CODE, 3},
    {"JA", JA_CODE, 2},
    {"JB", JB_CODE, 2},
    {"JBE", JBE_CODE, 3},
    {"JE", JE_CODE, 2},
    {"JNE", JNE_CODE, 3},
    {"JMP", JMP_CODE, 3},
    {"CALL", CALL_CODE, 4},
    {"RET", RET_CODE, 3},
    {"POPM", POPM_CODE, 4},
    {"DRAW", DRAW_CODE, 4},
    {"SLEEP", SLEEP_CODE, 5},
};

StackErr_t Create_Bytecode(void);

StackErr_t String_Processing(unsigned char* massive_bytecode, char* massive_command, int* massive_metok, int* num_elements, int num_prohod,struct stat buf, FILE* fpp);

int Find_command_code(char* command, int* j);

StackErr_t Work_With_PushValue(unsigned char* massive_bytecode, char* massive_command, int* num_elements, int* i, int* j);

StackErr_t Work_With_Register(unsigned char* massive_bytecode, char* massive_command, int* num_elements, int code, int* i, int* j);

StackErr_t Work_With_Jump(unsigned char* massive_bytecode, char* massive_command, int* massive_metok, int* num_elements, int* i, int* j);

StackErr_t Work_Oper_Memory(unsigned char* massive_bytecode, char* massive_command, int* num_elements, int code, int* i, int* j);

bool Skip_Spaces(char* massive_command, int* j);

#endif