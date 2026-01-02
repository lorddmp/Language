#include "tokenise.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "tech_func.h"

oper_t massive_op[NUM_OPER] = {
    {"+",           ADD_CODE,           1},
    {"-",           SUB_CODE,           1},
    {"*",           MUL_CODE,           1},
    {"/",           DIV_CODE,           1},
    {"^",           STEPEN_CODE,        1},
    {"(",           OPEN_BRAC_CODE,     1},
    {")",           CLOSED_BRAC_CODE,   1},
    {"sin",         SIN_CODE,           3},
    {"cos",         COS_CODE,           3},
    {"tan",         TAN_CODE,           3},
    {"cotan",       COTAN_CODE,         5},
    {"arcsin",      ARCSIN_CODE,        6},
    {"arccos",      ARCCOS_CODE,        6},
    {"arctan",      ARCTAN_CODE,        6},
    {"arccotan",    ARCCOTAN_CODE,      6},
    {"ln",          LN_CODE,            2},
};

#define ERROR(filename, funcname, line)                                                             \
    fprintf(stderr, "Error in file: %s, function: %s, line: %d", filename, funcname, line);         \
    return NULL;                                                                                    \


Node_t* Tokenize(void)
{
    FILE* fp = fopen(READ_TREE_FILE, "r");
    Node_t* massive_tokenov[1000] = {};
    char* massive_var[100] = {};

    double num = 0;
    int num_var = 0, skip = 0;
    struct stat stat1 = {};
    int descriptor = fileno(fp);
    int position = 0;

    fstat(descriptor, &stat1);

    char* massive_code = (char*)calloc((size_t)stat1.st_size + 1, sizeof(char));

    fread(massive_code, sizeof(char), (size_t)stat1.st_size, fp);

    for (int position = 0, i = 0; massive_code[position] == '\0'; i++)
    {
        bool found = false;

        if (sscanf(&massive_code[position], "%lg%n", &num, &skip) != 0)
        {
            massive_tokenov[i] = Make_Node(NUM_CODE, {.num_t = num});
            position += skip;
            found = true;
            continue;
        }

        for (int j = 0; i < NUM_OPER && found == false; j++)
        {
            if (strncmp(&massive_code[position], massive_op[i].op_symb, (size_t)massive_op[i].len) == 0 && massive_code[position + massive_op[i].len] == '(')
            {
                position +=  massive_op[i].len;
                massive_tokenov[i] = Make_Node(OPER_CODE, {.op_code_t = massive_op[i].op_code});
                found = true;
                break;
            }
        }
        
        for (int j = 0; j < num_var && found == false; j++)
        {
            if (strncmp(&massive_code[position], massive_var[j], skip) == 0)
            {
                position += skip;
                massive_tokenov[i] = Make_Node(VAR_CODE, {.var_ind = j});
                found = true;
                break;
            }
        }
        
        if (found == false)
        {
            ERROR(__FILE__, __func__, __LINE__)
        }
    }
}