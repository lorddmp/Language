#ifndef STACK
#define STACK

#include <stdlib.h>

#define NAME_ERROR_FILE "errors_report.txt"

#define ZASHITA

#ifdef ZASHITA

#define VERIF(stk) _Stack_Verify(stk, FILENAME, NUM_STRING, FUNCNAME)

#else

#define VERIF(stk) NO_ERRORS 

#endif

#define StackInit(processor, capacity) _Stack_Init(processor, capacity, __FILE__, __LINE__, __func__)
#define StackPush(processor, num) _Stack_Push(processor, num, __FILE__, __LINE__, __func__)
#define StackPop(processor, err) _Stack_Pop(processor, __FILE__, __LINE__, __func__, err)
#define StackDestroyer(processor) _Stack_Destroyer(processor, __FILE__, __LINE__, __func__)
#define StackDump(processor) _Stack_Dump(processor, __FILE__, __LINE__, __func__)

#define IF_ERROR(arg, stk)                  \
    if (arg)                                \
    {                                       \
        printf("%d", arg);                  \
        StackDump(stk);                     \
        StackDestroyer(&stk);               \
        return NO_ERRORS;                   \
    }

typedef double data_t;
#define SPEC "%lg"

typedef struct stack_str
{
    int can_struck1;
    data_t* data;
    int size;
    int capacity;
    int can_struck2;
} stack_t;

enum StackErr_t {
    NO_ERRORS = 0,
    ERROR_OPEN_INPUTFILE = 1,
    ERROR_OPEN_ERRORFILE = 2,
    ERROR_CREATING_BYTECODE_FILE = 3,
    ERROR_OPEN_BYTECODE_FILE = 4,
    ZERO_PTR_STACK = 5,
    ZERO_PTR_DATASTACK = 6,
    ILLEGAL_CAPACITY = 7,
    EMPTY_STACK = 8,
    ILLEGAL_SIZE = 9,
    POISON_ERROR = 10,
    CANARY_DEATH = 11,
    ERROR_CALLOC = 12,
    ERROR_REALLOC = 13,
    ERROR_PUSH_NUM = 14,
    ILLEGAL_COMMAND = 15,
    ILLEGAL_REGISTER = 16,
    ILLEGAL_JUMP_ADDRESS = 17,
    ILLEGAL_METKA = 18,
};

#include "processor.h"

StackErr_t _Stack_Init(stack_t* stk, int capacity, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME);

StackErr_t _Stack_Push(stack_t* stk, data_t value, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME);

data_t _Stack_Pop(stack_t* stk, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME,  StackErr_t *err);

StackErr_t _Stack_Dump(stack_t stk, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME);

StackErr_t _Stack_Verify(stack_t* stk, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME);

StackErr_t _Stack_Destroyer(stack_t* stk, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME);

StackErr_t _Stack_Bigger(stack_t* stk, int capacity, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME);

int _Is_Zero(double a);

#endif