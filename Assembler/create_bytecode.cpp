#include "create_bytecode.h"

#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

struct table_names {
    char* name;
    int len;
    int byte;
};

StackErr_t String_Processing(unsigned char* massive_bytecode, char* massive_command, table_names* massive_metok, int* num_elements, int num_prohod,struct stat buf, FILE* fpp);

int Find_command_code(char* command, int* j);

StackErr_t Work_With_PushValue(unsigned char* massive_bytecode, char* massive_command, int* num_elements, int* i, int* j);

StackErr_t Work_With_Register(unsigned char* massive_bytecode, char* massive_command, int* num_elements, int code, int* i, int* j);

StackErr_t Work_With_Jump(unsigned char* massive_bytecode, char* massive_command, table_names* massive_metok, int* num_elements, int* i, int* j);

StackErr_t Work_Oper_Memory(unsigned char* massive_bytecode, char* massive_command, int* num_elements, int code, int* i, int* j);

bool Skip_Spaces(char* massive_command, int* j);

StackErr_t Create_Bytecode(void)
{
    struct stat buf = {};
    unsigned char massive_bytecode[SIZE_MASSIVE] = {0};
    table_names* massive_metok = (table_names*)calloc(METKA_NUM, sizeof(table_names));
    int num_elements = 0, num_prohod = 1;
    
    FILE* fp = fopen(NAME_INPUT_FILE, "r");
    FILE* fpp = fopen(NAME_BYTECODE_FILE, "w");

    if (fp == NULL)
    {
        printf("Code error: %d. Error open file\n", ERROR_OPEN_INPUTFILE);
        return ERROR_OPEN_INPUTFILE;
    }

    if (fpp == NULL)
    {
        printf("Code error: %d. Couldn't create bytecode file\n", ERROR_CREATING_BYTECODE_FILE);
        return ERROR_CREATING_BYTECODE_FILE;
    }

    int descriptor = fileno(fp);
    fstat(descriptor, &buf);

    char* massive_command = (char*)calloc((size_t)buf.st_size + 1, sizeof(char));
    fread(massive_command, sizeof(char), (size_t)buf.st_size, fp);

    fclose(fp);

    if (String_Processing(massive_bytecode, massive_command, massive_metok, &num_elements, num_prohod, buf, fpp))
        return ERROR_FINAL;

    num_prohod = 2;

    if (String_Processing(massive_bytecode, massive_command, massive_metok, &num_elements, num_prohod, buf, fpp))
        return ERROR_FINAL;

    fwrite(massive_bytecode, sizeof(unsigned char), (size_t)num_elements, fpp);

    free(massive_metok);
    free(massive_command);
    fclose(fpp);

    return NO_ERRORS;
}

StackErr_t String_Processing(unsigned char* massive_bytecode, char* massive_command, table_names* massive_metok, int* num_elements, int num_prohod, struct stat buf,  FILE* fpp)
{
    char command[MAX_LEN_COMMAND] = {0};
    int cmd = 0, skip = 0, adr_metka = 0;
    StackErr_t err = NO_ERRORS;

    for (int byte_ptr = 0, mas_cmd_ptr = 0; mas_cmd_ptr < buf.st_size; byte_ptr++)
    {
        Skip_Spaces(massive_command, &mas_cmd_ptr);
        char* metka_name = (char*)calloc(100, sizeof(char));

        cmd = sscanf(massive_command + mas_cmd_ptr, "%s", command);

        if (cmd == 0 || cmd == -1) 
            break;

        Skip_Spaces(massive_command, &mas_cmd_ptr);
        *num_elements = byte_ptr + 1;

        if (sscanf(command, ":%s%n", metka_name, &skip) != 0)
        {
            for (int mas_mark_ptr = 0; mas_mark_ptr < METKA_NUM; mas_mark_ptr++)
            {
                if (massive_metok[mas_mark_ptr].len == 0)
                {
                    adr_metka = mas_mark_ptr;
                    break;
                }

                if (mas_mark_ptr == METKA_NUM)
                {
                    printf("Code error: %d. You can't create more variables than %d\n", ILLEGAL_METKA, METKA_NUM);
                    return ILLEGAL_METKA;
                }
            }

            massive_metok[adr_metka].byte = byte_ptr;
            massive_metok[adr_metka].name = metka_name;
            massive_metok[adr_metka].len = skip - 1;
            mas_cmd_ptr += skip;
            byte_ptr--;
            continue;
        }

        if (byte_ptr + (int)sizeof(data_t) >= SIZE_MASSIVE)
        {
            if (num_prohod == 2)
                fwrite(massive_bytecode, sizeof(unsigned char), (size_t)byte_ptr, fpp);

            byte_ptr = 0;
            *num_elements = 1;
        }

        int cmd_code = Find_command_code(command, &mas_cmd_ptr);

        if (cmd_code == -1)
        {
            printf("Code error: %d. Invalid command\n", ILLEGAL_COMMAND);
            return ILLEGAL_COMMAND;
        }

        massive_bytecode[byte_ptr] = (unsigned char)cmd_code; 

        if (cmd_code == PUSH_CODE)
        {
            if ((err = Work_With_PushValue(massive_bytecode, massive_command, num_elements, &byte_ptr, &mas_cmd_ptr)) == ERROR_PUSH_NUM)
            {
                if ((err = Work_With_Register(massive_bytecode, massive_command, num_elements, cmd_code, &byte_ptr, &mas_cmd_ptr)) == ILLEGAL_REGISTER)
                {
                    IF_ERROR_COMPILER(Work_Oper_Memory(massive_bytecode, massive_command, num_elements, cmd_code, &byte_ptr, &mas_cmd_ptr))
                    continue;
                }

                if (err != NO_ERRORS)
                    return err;

                continue;
            }

            if (err != NO_ERRORS)
                return err;

            continue;
        }

        else if (cmd_code == POP_CODE)
        {
            if ((err = Work_With_Register(massive_bytecode, massive_command, num_elements, cmd_code, &byte_ptr, &mas_cmd_ptr)) == ILLEGAL_REGISTER)
            {
                if ((err = Work_Oper_Memory(massive_bytecode, massive_command, num_elements, cmd_code, &byte_ptr, &mas_cmd_ptr)) == ILLEGAL_REGISTER)
                    continue;
                
                if (err != NO_ERRORS)
                    return err;

                continue;
            }

            if (err != NO_ERRORS)
                return err;

            continue;
        }

        else if (cmd_code >= JB_CODE && cmd_code <= CALL_CODE)
            IF_ERROR_COMPILER(Work_With_Jump(massive_bytecode, massive_command, massive_metok, num_elements, &byte_ptr, &mas_cmd_ptr))
    }

    return NO_ERRORS;
}

int Find_command_code(char* command, int* j)
{
    for (int t = 0; t < NUM_COMMAND; t++)
    {
        if (!(strncmp(command, massive_structur[t].CMD, size_t(massive_structur[t].LEN_CMD + 1))))
        {
            *j += massive_structur[t].LEN_CMD;
            return massive_structur[t].CMD_CODE;
        }
    }

    return -1;
}

StackErr_t Work_With_PushValue(unsigned char* massive_bytecode, char* massive_command, int* num_elements, int* i, int* j)
{
    data_t num = 0;
    int n = 0;

    Skip_Spaces(massive_command, j);

    if(sscanf(massive_command + *j, SPEC "%n", &num, &n) == 0)
        return ERROR_PUSH_NUM; 

    *((data_t*)(massive_bytecode + *i + 1)) = num;
    *i += (int)sizeof(data_t);
    *num_elements = *i + 1;
    *j += n;


    Skip_Spaces(massive_command, j);

    return NO_ERRORS;
}

StackErr_t Work_With_Register(unsigned char* massive_bytecode, char* massive_command, int* num_elements, int code, int* i, int* j)
{
    int n = 0;
    unsigned char reg = 0;

    Skip_Spaces(massive_command, j);

    if (sscanf(massive_command + *j, "REG%cX%n", &reg, &n) == 0)
        return ILLEGAL_REGISTER; 

    if (reg >= REG_NUM)
    {
        printf("Code error: %d. Invalid register FROM POPR\n", ILLEGAL_COMMAND);
        return ILLEGAL_COMMAND;
    }

    if (code == PUSH_CODE)
        massive_bytecode[*i] = PUSHR_CODE;

    else
        massive_bytecode[*i] = POPR_CODE;

    (*i)++;
    massive_bytecode[*i] = reg;

    *num_elements = *i + 1;
    *j += n;
    
    return NO_ERRORS;
}

StackErr_t Work_With_Jump(unsigned char* massive_bytecode, char* massive_command, table_names* massive_metok, int* num_elements, int* i, int* j)
{
    int n = 0;
    char jump_adr_name[100] = {};

    Skip_Spaces(massive_command, j);

    if(sscanf(massive_command + *j, ":%s%n", jump_adr_name, &n) == 0)
    {
        printf("Code error: %d. Error in reading address jump\n", ILLEGAL_JUMP_ADDRESS);
        return ILLEGAL_JUMP_ADDRESS; 
    } 

    for (int t = 0; t < METKA_NUM; t++)
    {
        if (t == METKA_NUM - 1 || massive_metok[t].len == 0)
        {
            *((int*)(massive_bytecode + *i + 1)) = 0;
            break;
        }

        if (strcmp(jump_adr_name, massive_metok[t].name) == 0)
        {
            *((int*)(massive_bytecode + *i + 1)) = massive_metok[t].byte;
            break;
        }
    }

    *i += (int)sizeof(int);
    *num_elements = *i + 1;
    *j += n;
    
    return NO_ERRORS;
}

StackErr_t Work_Oper_Memory(unsigned char* massive_bytecode, char* massive_command, int* num_elements, int code, int* i, int* j)
{
    int n = 0;
    unsigned char reg = 0;

    Skip_Spaces(massive_command, j);

    if(sscanf(massive_command + *j, "[%cX]%n", &reg, &n) == 0)
        return ILLEGAL_REGISTER; 

    unsigned char number_of_register = reg - 'A';

    if (number_of_register > REG_NUM)
    {
        printf("Code error: %d. Invalid register\n", ILLEGAL_COMMAND);
        return ILLEGAL_COMMAND;
    }

    if (code == PUSH_CODE)
        massive_bytecode[*i] = PUSHM_CODE;

    else
        massive_bytecode[*i] = POPM_CODE;

    (*i)++;
    massive_bytecode[*i] = number_of_register;

    *num_elements = *i + 1;
    *j += n;
    
    return NO_ERRORS;
}

bool Skip_Spaces(char* massive_command, int* j)
{
    while(isspace(massive_command[*j]))
        (*j)++;

    return 0;
}