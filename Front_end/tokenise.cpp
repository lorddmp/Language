#include "tokenise.h"
#include "tech_func.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN_OF_NAME 100

void Skip_Spaces(char* code_array, int* pos_code);

oper_t oper_array[NUM_OPER] = {
    {"+",               ADD_CODE,               1},
    {"-",               SUB_CODE,               1},
    {"*",               MUL_CODE,               1},
    {"/",               DIV_CODE,               1},
    {"^",               POW_CODE,               1},
    {"синус",           SIN_CODE,               10},
    {"косинус",         COS_CODE,               14},
    {"тангенс",         TAN_CODE,               14},
    {"котангенс",       COTAN_CODE,             18},
    {"арксинус",        ARCSIN_CODE,            16},
    {"арккосинус",      ARCCOS_CODE,            20},
    {"арктангенс",      ARCTAN_CODE,            20},
    {"арккотангенс",    ARCCOTAN_CODE,          24},
    {"логарифм",        LN_CODE,                16},

    {"==",              DOUBLE_EQ_CODE,         2},
    {"!=",              NOT_EQ_CODE,            2},
    {">=",              MORE_OR_EQ_CODE,        2},
    {">",               MORE_CODE,              1},
    {"<=",              LESS_OR_EQ_CODE,        2},
    {"<",               LESS_CODE,              1},
    
    {"(",               OPEN_BRAC_CODE,         1},
    {")",               CLOSED_BRAC_CODE,       1},
    {"{",               OPEN_FIG_BRAC_CODE,     1},
    {"}",               CLOSED_FIG_BRAC_CODE,   1},
    {"=",               EQUA_CODE,              1},
    {"переменночка",    VAR_INIT_CODE,          24},
    {"тепереча",        CHANGE_VAR_CODE,        16},
    
    {";",               SEMICOLONE_CODE,        1},

    {"ежели",           IF_CODE,                10},
    {"покудова",        WHILE_CODE,             16},
    {"напечатай",       PRINTF_CODE,            18},
    {"ввод",            INPUT_CODE,             8},

    {"функция",         FUNC_INIT_CODE,         14},
    {"вызови",          FUNC_CALL_CODE,         12},
};

#define ERROR(filename, funcname, line)                                                             \
{                                                                                                   \
    fprintf(stderr, "Error in file: %s, function: %s, line: %d", filename, funcname, line);         \
    return NULL;                                                                                    \
}                                                                                                   \

Node_t** Tokenize(char** name_array, int size_name_array, int* num_name, int* num_token)
{
    FILE* fp = fopen(READ_TREE_FILE, "r");

    int size_token_array = 1000;
    Node_t** token_array = (Node_t**)calloc((size_t)size_token_array, sizeof(Node_t*));
    if (token_array == NULL)
        ERROR(__FILE__, __func__, __LINE__)

    char name[MAX_LEN_OF_NAME] = {};

    data_t num = 0;
    int skip = 0;

    struct stat stat1 = {};
    int descriptor = fileno(fp);
    fstat(descriptor, &stat1);

    char* code_array = (char*)calloc((size_t)stat1.st_size + 1, sizeof(char));
    if (code_array ==  NULL)
        ERROR(__FILE__, __func__, __LINE__)

    fread(code_array, sizeof(char), (size_t)stat1.st_size, fp);

    for (int pos_code = 0, pos_tokens = 0; code_array[pos_code] != '\0'; pos_tokens++)
    {
        Skip_Spaces(code_array, &pos_code);
        if (code_array[pos_code] == '\0')
            break;

        if (pos_tokens == size_token_array)
        {
            size_token_array *= 2;
            token_array = (Node_t**)realloc(token_array, (size_t)size_token_array);
            if (token_array == NULL)
                ERROR(__FILE__, __func__, __LINE__)
        }

        bool found = false;
        *num_token = pos_tokens + 1;

        for (int pos_opers = 0; pos_opers < NUM_OPER; pos_opers++)
        {
            if (strncmp(&code_array[pos_code], oper_array[pos_opers].op_symb, (size_t)oper_array[pos_opers].len) == 0)
            {
                if ((oper_array[pos_opers].op_code == SUB_CODE) &&    !((token_array[pos_tokens-1]->type == NUM_CODE) || 
                                                                            (token_array[pos_tokens-1]->type == NAME_CODE))) //-12? -10?
                    break;

                pos_code += oper_array[pos_opers].len;
                token_array[pos_tokens] = Make_Node(OPER_CODE, {.op_code_t = oper_array[pos_opers].op_code});
                found = true;
                break;
            }
        }

        if (found)
            continue;

        else if (sscanf(&code_array[pos_code], SPEC "%n", &num, &skip) != 0)
        {
            token_array[pos_tokens] = Make_Node(NUM_CODE, {.num_t = num});
            pos_code += skip;
            found = true;
        }

        else if (sscanf(&code_array[pos_code], "%[A-Za-z0-9_]%n", name, &skip) != 0)
        {

            for (int j = 0; j < *num_name && found == false; j++)
            {
                if (strncmp(name, name_array[j], (size_t)skip) == 0)
                {
                    token_array[pos_tokens] = Make_Node(NAME_CODE, {.name_ind = j});
                    pos_code += skip;
                    found = true;

                    break;
                }
            }

            if (!found)
            {
                name_array[*num_name] = strdup(name);
                token_array[pos_tokens] = Make_Node(NAME_CODE, {.name_ind = *num_name});
                pos_code += skip;
                (*num_name)++;
                found = true;

                if (*num_name == size_name_array)
                {
                    size_name_array *= 2;
                    name_array = (char**)realloc(name_array, (size_t)size_name_array);
                    if (name_array == NULL)
                        ERROR(__FILE__, __func__, __LINE__)
                }
            }
        }

        else
            ERROR(__FILE__, __func__, __LINE__)
    }


    token_array[*num_token] = Make_Node(OPER_CODE, {.op_code_t = END_CODE});
    (*num_token)++;

    free(code_array);
    fclose(fp);

    return token_array;
}

void Skip_Spaces(char* code_array, int* pos_code)
{
    while (isspace(code_array[*pos_code]))
        (*pos_code)++;
}