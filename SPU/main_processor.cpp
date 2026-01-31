#include "stack.h"
#include "processor.h"
#include <stdio.h>
#include <string.h>

int main()
{
    str_processor processor = {
        processor.stk = {},
        processor.call_adr = {},
        processor.oper_memory = (data_t*)calloc(OPER_MEMORY_SIZE, sizeof(data_t)),
        processor.registr_mas[REG_NUM] = {},
        processor.ip = 0,
    };

    if (processor.oper_memory == NULL)
    {
        printf("CALLOC ERROR\n");
        return ERROR_CALLOC;
    }

    IF_ERROR(StackInit(&processor.stk, 4), processor.stk);
    IF_ERROR(StackInit(&processor.call_adr, METKA_NUM), processor.call_adr);

    Run_Bytecode(&processor);

    IF_ERROR(StackDump(processor.stk), processor.stk);
    IF_ERROR(StackDestroyer(&processor.stk), processor.stk);

    printf("\033[32mPROCESSOR WORKING ENDED SUCCESSFULLY\n");
    return 0;
}