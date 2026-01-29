#include "stack.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define POISON 7062007
#define CANARY1 0xDEDDED
#define CANARY2 0xDADDAD
#define CANARY_STRUCK1 0xEDAEDA
#define CANARY_STRUCK2 0xBEDABED

StackErr_t _Stack_Init(stack_t* stk, int capacity, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME)
{
    if (capacity <= 0)
    {
        _Stack_Dump(*stk, FILENAME, NUM_STRING, FUNCNAME);
        printf("Code error: %d. Stack capacity <= 0\n", ILLEGAL_CAPACITY);
        return ILLEGAL_CAPACITY;
    }

    stk->can_struck1 = CANARY_STRUCK1;
    stk->can_struck2 = CANARY_STRUCK2;
    stk->data = (data_t*)calloc((size_t)capacity + 2, sizeof(data_t));
    stk->capacity = capacity;

    if (stk->data == NULL)
    {
        _Stack_Dump(*stk, FILENAME, NUM_STRING, FUNCNAME);
        printf("Code error: %d. Calloc worked incorrectly\n", ERROR_CALLOC);
        return ERROR_CALLOC;
    }

    stk->data[0] = CANARY1;
    stk->data[capacity + 1] = CANARY2;
    for(int i = 1; i < capacity + 1; i++)
        stk->data[i] = POISON;
    
    stk->size = 0;

    return VERIF(stk);
}

StackErr_t _Stack_Push(stack_t* stk, data_t value, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME)
{
    StackErr_t err = NO_ERRORS;

    if ((err = VERIF(stk)) != 0)
        return err;

    if (stk->capacity == stk->size \
            && _Stack_Bigger(stk, stk->capacity <<= 1,  FILENAME, NUM_STRING, FUNCNAME))
        return ERROR_REALLOC;

    stk->data[stk->size + 1] = value;
    stk->size++;

    return VERIF(stk);
}

data_t _Stack_Pop(stack_t* stk, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME,  StackErr_t *err)
{
    if ((*err = VERIF(stk)) != 0)
        return 1;
    
    if (stk->size == 0)
    {
        _Stack_Dump(*stk, FILENAME, NUM_STRING, FUNCNAME);
        printf("Code error: %d. Empty stack\n", EMPTY_STACK);
        return 1;
    }

    stk->size--;
    data_t varvar = stk->data[stk->size + 1];
    stk->data[stk->size + 1] = POISON;
    
    if (VERIF(stk) != 0)
        return 0;

    return(varvar);
}

StackErr_t _Stack_Dump(stack_t stk, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME)
{
    FILE* fp = fopen(NAME_ERROR_FILE, "w");

    if (fp == NULL)
    {
        _Stack_Dump(stk, FILENAME, NUM_STRING, FUNCNAME);
        printf("Code error: %d. Empty stack\n", ERROR_OPEN_ERRORFILE);
        return ERROR_OPEN_ERRORFILE;
    }

    fprintf(fp, "_Stack_Dump called from %s, %s:%d\n", FILENAME, FUNCNAME, NUM_STRING);
    fprintf(fp, "stack [%p]\n\n", &stk);
    fprintf(fp, "size = %d\n", stk.size);
    fprintf(fp, "capacity = %d\n", stk.capacity);
    fprintf(fp, "data [%p]\n", stk.data);
    fprintf(fp, "{\n");
    
    for(int i = 1; i <= stk.capacity; i++)
        fprintf(fp, "*[%d] = " SPEC "\n", i, stk.data[i]);

    fprintf(fp, "}\n");

    fclose(fp);
    return NO_ERRORS;
}

StackErr_t _Stack_Verify(stack_t* stk, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME)
{
    if (stk == NULL)
    {
        _Stack_Dump(*stk, FILENAME, NUM_STRING, FUNCNAME);
        printf("Code error: %d. Stack pointer = 0x00000\n", ZERO_PTR_STACK);
        return ZERO_PTR_STACK;
    }

    if (stk->capacity <= 0)
    {
        _Stack_Dump(*stk, FILENAME, NUM_STRING, FUNCNAME);
        printf("Code error: %d. Stack capacity <= 0\n", ILLEGAL_CAPACITY);
        return ILLEGAL_CAPACITY;
    }

    if (stk->data == NULL)
    {
        _Stack_Dump(*stk, FILENAME, NUM_STRING, FUNCNAME);
        printf("Code error: %d. Data stack pointer = 0x00000\n", ZERO_PTR_DATASTACK);
        return ZERO_PTR_DATASTACK;
    }

    if (stk->size < 0)
    {
        _Stack_Dump(*stk, FILENAME, NUM_STRING, FUNCNAME);
        printf("Code error: %d. Illegal size of data\n", ILLEGAL_SIZE);
        return ILLEGAL_SIZE;
    }
    
    for (int i = stk->size + 1; i < stk->capacity + 1; i++)
        if (!(_Is_Zero(stk->data[i] - POISON)))
        {
            _Stack_Dump(*stk, FILENAME, NUM_STRING, FUNCNAME);
            printf("Code error: %d. In free stack memory illegal values\n", POISON_ERROR);
            return POISON_ERROR;
        }

    if (!(_Is_Zero(stk->data[0] - CANARY1)) || 
    !(_Is_Zero(stk->data[stk->capacity + 1] - CANARY2) || 
    (stk->can_struck1 != CANARY_STRUCK1) || 
    (stk->can_struck2 != CANARY_STRUCK2)))
    {
        _Stack_Dump(*stk, FILENAME, NUM_STRING, FUNCNAME);
        printf("Code error: %d. Canary was dead. RIP\n", CANARY_DEATH);
        return CANARY_DEATH;
    }

    return NO_ERRORS;
}

StackErr_t _Stack_Destroyer(stack_t* stk, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME)
{
    VERIF(stk);

    free(stk->data);
    stk->data = NULL;
    stk->capacity = -1;
    stk->size = -1;

    return NO_ERRORS;
}

StackErr_t _Stack_Bigger (stack_t* stk, int capacity, const char* FILENAME, const int NUM_STRING, const char* FUNCNAME)
{
    data_t *var = (data_t*)realloc(stk->data, ((size_t)capacity + 2)*sizeof(data_t));

    if (var == NULL)
    {
        _Stack_Dump(*stk, FILENAME, NUM_STRING, FUNCNAME);
        printf("Code error: %d. Error realloc\n", ERROR_REALLOC);
        return ERROR_REALLOC;
    }

    stk->data = var;
    stk->data[stk->capacity + 1] = CANARY2;
    for (int i = stk->capacity / 2 + 1; i <= stk->capacity; i++)
        stk->data[i] = POISON;


    return NO_ERRORS;
}

int _Is_Zero(double a)
{
    if (a < 1e-9)
        return 1;
    else
        return 0;
}