#include "create_bytecode.h"

#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#define MAX_LEN_MARK_NAME 100

struct table_names {
    char* name;
    int len;
    int byte;
};

StackErr_t String_Processing(unsigned char* bytecode_array, char* command_array, table_names* mark_array, int* num_elements, int num_prohod, struct stat buf, FILE* fpp);

int Find_command_code(char* command_name, int* pos_command_array);

StackErr_t Work_With_PushValue(unsigned char* bytecode_array, char* command_array, int* num_elements, int* pos_bytecode_array, int* pos_command_array);

StackErr_t Work_With_Register(unsigned char* bytecode_array, char* command_array, int* num_elements, int code, int* pos_bytecode_array, int* pos_command_array);

StackErr_t Work_With_Jump(unsigned char* bytecode_array, char* command_array, table_names* mark_array, int* num_elements, int* pos_bytecode_array, int* pos_command_array);

StackErr_t Work_Oper_Memory(unsigned char* bytecode_array, char* command_array, int* num_elements, int code, int* pos_bytecode_array, int* pos_command_array);

bool Skip_Spaces(char* command_array, int* pos_command_array);

StackErr_t Create_Bytecode(void)
{
    struct stat buf = {};
    unsigned char bytecode_array[SIZE_MASSIVE] = {0};

    table_names* mark_array = (table_names*)calloc(MAX_METKA_NUM, sizeof(table_names));
    if (mark_array == NULL)
    {
        fprintf(stderr, "Code error: %d. Calloc error", ERROR_CALLOC);
        return ERROR_CALLOC;
    }


    int num_elements = 0, num_prohod = 1;
    
    FILE* fp = fopen(NAME_INPUT_FILE, "r");
    FILE* fpp = fopen(NAME_BYTECODE_FILE, "w");

    if (fp == NULL)
    {
        fprintf(stderr, "Code error: %d. Error open file\n", ERROR_OPEN_INPUTFILE);
        return ERROR_OPEN_INPUTFILE;
    }

    if (fpp == NULL)
    {
        fprintf(stderr, "Code error: %d. Couldn't create bytecode file\n", ERROR_CREATING_BYTECODE_FILE);
        return ERROR_CREATING_BYTECODE_FILE;
    }

    int descriptor = fileno(fp);
    fstat(descriptor, &buf);

    char* command_array = (char*)calloc((size_t)buf.st_size + 1, sizeof(char));
    if (command_array == NULL)
    {
        fprintf(stderr, "Code error: %d. Calloc error", ERROR_CALLOC);
        return ERROR_CALLOC;
    }

    fread(command_array, sizeof(char), (size_t)buf.st_size, fp);

    fclose(fp);

    if (String_Processing(bytecode_array, command_array, mark_array, &num_elements, num_prohod, buf, fpp))
        return ERROR_FINAL;

    num_prohod++;

    if (String_Processing(bytecode_array, command_array, mark_array, &num_elements, num_prohod, buf, fpp))
        return ERROR_FINAL;

    fwrite(bytecode_array, sizeof(unsigned char), (size_t)num_elements, fpp);

    for (int pos_mark_array = 0; pos_mark_array < MAX_METKA_NUM && mark_array[pos_mark_array].name != NULL; pos_mark_array++)
        free(mark_array[pos_mark_array].name);

    free(mark_array);
    free(command_array);
    fclose(fpp);

    return NO_ERRORS;
}

StackErr_t String_Processing(unsigned char* bytecode_array, char* command_array, table_names* mark_array, int* num_elements, int num_prohod, struct stat buf,  FILE* fpp)
{
    char command_name[MAX_LEN_COMMAND_NAME] = {0};
    int command_was_read = 0, skip = 0, mark_adr = 0;
    StackErr_t err = NO_ERRORS;

    for (int pos_bytecode_array = 0, pos_command_array = 0; pos_command_array < buf.st_size; pos_bytecode_array++)
    {
        Skip_Spaces(command_array, &pos_command_array);

        command_was_read = sscanf(command_array + pos_command_array, "%s", command_name);

        if (command_was_read == 0 || command_was_read == -1) 
            break;

        Skip_Spaces(command_array, &pos_command_array);
        *num_elements = pos_bytecode_array + 1;

        char* mark_name = (char*)calloc(MAX_LEN_MARK_NAME, sizeof(char));
        if (sscanf(command_name, ":%s%n", mark_name, &skip) != 0)
        {
            if (num_prohod == 2)
            {
                free(mark_name);
                pos_command_array += skip;
                continue;
            }

            for (int mas_mark_ptr = 0; mas_mark_ptr < MAX_METKA_NUM; mas_mark_ptr++)
            {
                if (mark_array[mas_mark_ptr].len == 0)
                {
                    mark_adr = mas_mark_ptr;
                    break;
                }

                if (mas_mark_ptr == MAX_METKA_NUM)
                {
                    printf("Code error: %d. You can't create more variables than %d\n", ILLEGAL_METKA, MAX_METKA_NUM);
                    return ILLEGAL_METKA;
                }
            }

            mark_array[mark_adr].byte = pos_bytecode_array;
            mark_array[mark_adr].name = mark_name;
            mark_array[mark_adr].len = skip - 1;
            pos_command_array += skip;
            pos_bytecode_array--;
            continue;
        }
        else
            free(mark_name);

        if (pos_bytecode_array + (int)sizeof(data_t) >= SIZE_MASSIVE)
        {
            if (num_prohod == 2)
                fwrite(bytecode_array, sizeof(unsigned char), (size_t)pos_bytecode_array, fpp);

            pos_bytecode_array = 0;
            *num_elements = 1;
        }

        int cmd_code = Find_command_code(command_name, &pos_command_array);

        if (cmd_code == -1)
        {
            printf("Code error: %d. Invalid command_name\n", ILLEGAL_COMMAND);
            return ILLEGAL_COMMAND;
        }

        bytecode_array[pos_bytecode_array] = (unsigned char)cmd_code; 

        if (cmd_code == PUSH_CODE)
        {
            if ((err = Work_With_PushValue(bytecode_array, command_array, num_elements, &pos_bytecode_array, &pos_command_array)) == ERROR_PUSH_NUM)
            {
                if ((err = Work_With_Register(bytecode_array, command_array, num_elements, cmd_code, &pos_bytecode_array, &pos_command_array)) == ILLEGAL_REGISTER)
                {
                    IF_ERROR_COMPILER(Work_Oper_Memory(bytecode_array, command_array, num_elements, cmd_code, &pos_bytecode_array, &pos_command_array))
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
            if ((err = Work_With_Register(bytecode_array, command_array, num_elements, cmd_code, &pos_bytecode_array, &pos_command_array)) == ILLEGAL_REGISTER)
            {
                if ((err = Work_Oper_Memory(bytecode_array, command_array, num_elements, cmd_code, &pos_bytecode_array, &pos_command_array)) == ILLEGAL_REGISTER)
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
            IF_ERROR_COMPILER(Work_With_Jump(bytecode_array, command_array, mark_array, num_elements, &pos_bytecode_array, &pos_command_array))
    }

    return NO_ERRORS;
}

int Find_command_code(char* command_name, int* pos_command_array)
{
    for (int pos_cmd_array = 0; pos_cmd_array < NUM_COMMAND; pos_cmd_array++)
    {
        if (!(strncmp(command_name, struct_сmd_array[pos_cmd_array].cmd, size_t(struct_сmd_array[pos_cmd_array].len_cmd + 1))))
        {
            *pos_command_array += struct_сmd_array[pos_cmd_array].len_cmd;
            return struct_сmd_array[pos_cmd_array].code_cmd;
        }
    }

    return -1;
}

StackErr_t Work_With_PushValue(unsigned char* bytecode_array, char* command_array, int* num_elements, int* pos_bytecode_array, int* pos_command_array)
{
    data_t num = 0;
    int skip = 0;

    Skip_Spaces(command_array, pos_command_array);

    if (sscanf(command_array + *pos_command_array, SPEC "%n", &num, &skip) == 0)
        return ERROR_PUSH_NUM; 

    *((data_t*)(bytecode_array + *pos_bytecode_array + 1)) = num;
    *pos_bytecode_array += (int)sizeof(data_t);
    *num_elements = *pos_bytecode_array + 1;
    *pos_command_array += skip;

    Skip_Spaces(command_array, pos_command_array);

    return NO_ERRORS;
}

StackErr_t Work_With_Register(unsigned char* bytecode_array, char* command_array, int* num_elements, int code, int* pos_bytecode_array, int* pos_command_array)
{
    int skip = 0, reg = 0;

    Skip_Spaces(command_array, pos_command_array);

    if (sscanf(command_array + *pos_command_array, "REG%dX%n", &reg, &skip) == 0)
        return ILLEGAL_REGISTER; 

    if (reg >= REG_NUM)
    {
        printf("Code error: %d. Invalid register FROM POPR\n", ILLEGAL_COMMAND);
        return ILLEGAL_COMMAND;
    }

    if (code == PUSH_CODE)
        bytecode_array[*pos_bytecode_array] = PUSHR_CODE;

    else
        bytecode_array[*pos_bytecode_array] = POPR_CODE;

    (*pos_bytecode_array)++;
    bytecode_array[*pos_bytecode_array] = (unsigned char)reg;

    *num_elements = *pos_bytecode_array + 1;
    *pos_command_array += skip;
    
    return NO_ERRORS;
}

StackErr_t Work_With_Jump(unsigned char* bytecode_array, char* command_array, table_names* mark_array, int* num_elements, int* pos_bytecode_array, int* pos_command_array)
{
    int skip = 0;
    char jump_adr_name[MAX_LEN_MARK_NAME] = {};

    Skip_Spaces(command_array, pos_command_array);

    if (sscanf(command_array + *pos_command_array, ":%s%n", jump_adr_name, &skip) == 0)
    {
        printf("Code error: %d. Error in reading address jump\n", ILLEGAL_JUMP_ADDRESS);
        return ILLEGAL_JUMP_ADDRESS; 
    } 

    for (int pos_mark_array = 0; pos_mark_array < MAX_METKA_NUM; pos_mark_array++)
    {
        if (pos_mark_array == MAX_METKA_NUM - 1 || mark_array[pos_mark_array].len == 0)
        {
            *((int*)(bytecode_array + *pos_bytecode_array + 1)) = 0;
            break;
        }

        if (strcmp(jump_adr_name, mark_array[pos_mark_array].name) == 0)
        {
            *((int*)(bytecode_array + *pos_bytecode_array + 1)) = mark_array[pos_mark_array].byte;
            break;
        }
    }

    *pos_bytecode_array += (int)sizeof(int);
    *num_elements = *pos_bytecode_array + 1;
    *pos_command_array += skip;
    
    return NO_ERRORS;
}

StackErr_t Work_Oper_Memory(unsigned char* bytecode_array, char* command_array, int* num_elements, int code, int* pos_bytecode_array, int* pos_command_array)
{
    int skip = 0, reg = 0;

    Skip_Spaces(command_array, pos_command_array);

    if (sscanf(command_array + *pos_command_array, "[%dX]%n", &reg, &skip) == 0)
        return ILLEGAL_REGISTER; 

    if (reg > REG_NUM)
    {
        printf("Code error: %d. Invalid register\n", ILLEGAL_COMMAND);
        return ILLEGAL_COMMAND;
    }

    if (code == PUSH_CODE)
        bytecode_array[*pos_bytecode_array] = PUSHM_CODE;

    else
        bytecode_array[*pos_bytecode_array] = POPM_CODE;

    (*pos_bytecode_array)++;
    bytecode_array[*pos_bytecode_array] = (unsigned char)reg;

    *num_elements = *pos_bytecode_array + 1;
    *pos_command_array += skip;
    
    return NO_ERRORS;
}

bool Skip_Spaces(char* command_array, int* pos_command_array)
{
    while(isspace(command_array[*pos_command_array]))
        (*pos_command_array)++;

    return 0;
}