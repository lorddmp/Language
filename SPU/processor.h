#ifndef OPERATIONS
#define OPERATIONS

#include "stack.h"
#include "create_bytecode.h"

#define MAX_SIZE_COMMAND 8
#define REG_NUM 4
#define OPER_MEMORY_SIZE 184*250
#define DLINA_STROKI_OPER 250


struct str_processor{
    stack_str stk;
    stack_str call_adr;
    data_t* oper_memory;
    data_t registr_mas[REG_NUM];
    unsigned char* massive_bytecode;
    unsigned char* buffer_commands;
    int ip;
};

StackErr_t Run_Bytecode(str_processor* processor);

#endif