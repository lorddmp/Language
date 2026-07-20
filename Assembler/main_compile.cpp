#include "create_bytecode.h"

#include <stdio.h>
#include <string.h>


int main()
{
    if( Create_Bytecode() != NO_ERRORS)
        return 1;

    printf("\033[32mCOMPILATION ENDED SUCCESSFULLY\033[0m\n");
    return 0;
}