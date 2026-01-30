#include "tokenise.h"
#include "tech_func.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

void Skip_Spaces(char* massive_code, int* position);

oper_t massive_op[NUM_OPER] = {
    {"+",           ADD_CODE,               1},
    {"-",           SUB_CODE,               1},
    {"*",           MUL_CODE,               1},
    {"/",           DIV_CODE,               1},
    {"^",           STEPEN_CODE,            1},
    {"(",           OPEN_BRAC_CODE,         1},
    {")",           CLOSED_BRAC_CODE,       1},
    {"{",           OPEN_FIG_BRAC_CODE,     1},
    {"}",           CLOSE_FIG_BRAC_CODE,    1},
    {"==",          DOUBLE_EQ_CODE,         2},
    {"=",           EQUA_CODE,              1},
    {"var",         VAR_INIT_CODE,          3},
    {"now",         CHANGE_VAR_CODE,        3},
    {";",           SEMICOLONE_CODE,        1},

    {"sin",         SIN_CODE,               3},
    {"cos",         COS_CODE,               3},
    {"tan",         TAN_CODE,               3},
    {"cotan",       COTAN_CODE,             5},
    {"arcsin",      ARCSIN_CODE,            6},
    {"arccos",      ARCCOS_CODE,            6},
    {"arctan",      ARCTAN_CODE,            6},
    {"arccotan",    ARCCOTAN_CODE,          6},
    {"ln",          LN_CODE,                2},

    {"if",          IF_CODE,                2},
    {"while",       WHILE_CODE,             5},
};

#define ERROR(filename, funcname, line)                                                             \
{                                                                                                   \
    fprintf(stderr, "Error in file: %s, function: %s, line: %d", filename, funcname, line);         \
    return NULL;                                                                                    \
}                                                                                                   \


Node_t** Tokenize(char** massive_var)
{
    FILE* fp = fopen(READ_TREE_FILE, "r");
    Node_t** massive_tokenov = (Node_t**)calloc(1000, sizeof(Node_t*));
    char var[100] = {};

    double num = 0;
    int num_var = 0, skip = 0, count = 0;
    struct stat stat1 = {};
    int descriptor = fileno(fp);

    fstat(descriptor, &stat1);

    char* massive_code = (char*)calloc((size_t)stat1.st_size + 1, sizeof(char));

    fread(massive_code, sizeof(char), (size_t)stat1.st_size, fp);

    for (int position = 0, i = 0; massive_code[position] != '\0'; i++)
    {
        Skip_Spaces(massive_code, &position);
        if (massive_code[position] == '\0')
            break;

        bool found = false;
        count = i + 1;

        // printf("position = %d\n", position);
        // printf("symb = %c\n", massive_code[position]);
        for (int j = 0; j < NUM_OPER && found == false; j++)
        {
            if (strncmp(&massive_code[position], massive_op[j].op_symb, (size_t)massive_op[j].len) == 0)
            {
                if (j >= NUM_OPER - 10 &&   (isalpha(massive_code[position + massive_op[j].len]) || 
                                            isdigit(massive_code[position + massive_op[j].len]) ||
                                            massive_code[position + massive_op[j].len] == '_'))
                    break;

                position += massive_op[j].len;
                massive_tokenov[i] = Make_Node(OPER_CODE, {.op_code_t = massive_op[j].op_code});
                printf("Created node oper: val = %d\n", massive_op[j].op_code);
                found = true;
                break;
            }
        }
        
        if (found == false && sscanf(&massive_code[position], "%lg%n", &num, &skip) != 0)
        {
            // printf("symb = %c\n", massive_code[position]);
            massive_tokenov[i] = Make_Node(NUM_CODE, {.num_t = num});
            printf("Created node num: val = %lg\n", num);
            position += skip;
            found = true;
            continue;
        }

        if (found == false && sscanf(&massive_code[position], "%[A-Za-z0-9_]%n", var, &skip) != 0)
        {
            position += skip;

            for (int j = 0; j < num_var && found == false; j++)
            {
                if (strncmp(var, massive_var[j], (size_t)skip) == 0)
                {
                    massive_tokenov[i] = Make_Node(VAR_CODE, {.var_ind = j});
                    printf("Created node var: val = %d\n", j);
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                massive_var[num_var] = strdup(var);
                massive_tokenov[i] = Make_Node(VAR_CODE, {.var_ind = num_var});
                printf("Created new node var: val = %d\n", num_var);
                num_var++;
                found = true;
            }
        }
        
        if (found == false)
            ERROR(__FILE__, __func__, __LINE__)
    }


    massive_tokenov[count] = Make_Node(OPER_CODE, {.op_code_t = END_CODE});

    free(massive_code);

    return massive_tokenov;
}

void Skip_Spaces(char* massive_code, int* position)
{
    while (isspace(massive_code[*position]))
        (*position)++;
}