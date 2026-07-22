#include "tokenise.h"
#include "tech_func.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN_OF_NAME 100

void Skip_Spaces(char* massive_code, int* position_code);

oper_t massive_op[NUM_OPER] = {
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
    {"}",               CLOSE_FIG_BRAC_CODE,    1},
    {"=",               EQUA_CODE,              1},
    {"переменночка",    VAR_INIT_CODE,          24},
    {"тепереча",        CHANGE_VAR_CODE,        16},
    {";",               SEMICOLONE_CODE,        1},

    {"ежели",           IF_CODE,                10},
    {"покудова",        WHILE_CODE,             16},
    {"напечатай",       PRINTF_CODE,            18},

    {"функция",         FUNC_INIT_CODE,         14},
    {"вызови",          FUNC_CALL_CODE,         12},
};

#define ERROR(filename, funcname, line)                                                             \
{                                                                                                   \
    fprintf(stderr, "Error in file: %s, function: %s, line: %d", filename, funcname, line);         \
    return NULL;                                                                                    \
}                                                                                                   \

Node_t** Tokenize(char** massive_name)
{
    FILE* fp = fopen(READ_TREE_FILE, "r");

    int num_of_tokens = 1000;
    Node_t** massive_tokenov = (Node_t**)calloc(num_of_tokens, sizeof(Node_t*));
    if (massive_tokenov == NULL)
        ERROR(__FILE__, __func__, __LINE__)

    char name[MAX_LEN_OF_NAME] = {};

    double num = 0;
    int num_name = 0, skip = 0, num_tokens = 0;

    struct stat stat1 = {};
    int descriptor = fileno(fp);
    fstat(descriptor, &stat1);

    char* massive_code = (char*)calloc((size_t)stat1.st_size + 1, sizeof(char));
    if (massive_code ==  NULL)
        ERROR(__FILE__, __func__, __LINE__)

    fread(massive_code, sizeof(char), (size_t)stat1.st_size, fp);

    for (int position_code = 0, position_tokens = 0; massive_code[position_code] != '\0'; position_tokens++)
    {
        Skip_Spaces(massive_code, &position_code);
        if (massive_code[position_code] == '\0')
            break;

        if (position_tokens == num_of_tokens)
        {
            num_of_tokens *= 2;
            massive_tokenov = (Node_t**)realloc(massive_tokenov, num_of_tokens);
            if (massive_tokenov == NULL)
                ERROR(__FILE__, __func__, __LINE__)
        }

        bool found = false;
        num_tokens = position_tokens + 1;

        for (int position_mas_op = 0; position_mas_op < NUM_OPER; position_mas_op++)
        {
            if (strncmp(&massive_code[position_code], massive_op[position_mas_op].op_symb, (size_t)massive_op[position_mas_op].len) == 0)
            {
                if ((massive_op[position_mas_op].op_code == SUB_CODE) &&    !((massive_tokenov[position_tokens-1]->type == NUM_CODE) || 
                                                                            (massive_tokenov[position_tokens-1]->type == NAME_CODE))) //-12? -10?
                    break;

                position_code += massive_op[position_mas_op].len;
                massive_tokenov[position_tokens] = Make_Node(OPER_CODE, {.op_code_t = massive_op[position_mas_op].op_code});
                found = true;
                break;
            }
        }

        if (found)
            continue;

        else if (sscanf(&massive_code[position_code], "%lg%n", &num, &skip) != 0)
        {
            massive_tokenov[position_tokens] = Make_Node(NUM_CODE, {.num_t = num});
            position_code += skip;
            found = true;
        }

        else if (sscanf(&massive_code[position_code], "%[A-Za-z0-9_]%n", name, &skip) != 0)
        {

            for (int j = 0; j < num_name && found == false; j++)
            {
                if (strncmp(name, massive_name[j], (size_t)skip) == 0)
                {
                    massive_tokenov[position_tokens] = Make_Node(NAME_CODE, {.name_ind = j});
                    position_code += skip;
                    found = true;

                    break;
                }
            }

            if (!found)
            {
                massive_name[num_name] = strdup(name);
                massive_tokenov[position_tokens] = Make_Node(NAME_CODE, {.name_ind = num_name});
                num_name++;
                found = true;
            }
        }

        else
            ERROR(__FILE__, __func__, __LINE__)
    }


    massive_tokenov[num_tokens] = Make_Node(OPER_CODE, {.op_code_t = END_CODE});

    free(massive_code);

    return massive_tokenov;
}

void Skip_Spaces(char* massive_code, int* position_code)
{
    while (isspace(massive_code[*position_code]))
        (*position_code)++;
}