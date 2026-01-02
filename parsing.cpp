#include "structs_defines_types.h"
#include "tech_func.h"
#include "parsing.h"

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ERROR(filename, funcname, line)                                                             \
{                                                                                                   \
    fprintf(stderr, "Error in file: %s, function: %s, line: %d", filename, funcname, line);         \
    return NULL;                                                                                    \
}

#define IF_ERROR_READING(a)                                         \
if (a == NULL)                                              \
    return NULL;                                            \
    
Node_t* Get_N(int* position, Node_t** mas_tokenov);
Node_t* Get_Var(int* position, Node_t** mas_tokenov);
Node_t* Get_Func(int* position, Node_t** mas_tokenov);
Node_t* Get_P(int* position, Node_t** mas_tokenov);
Node_t* Get_T(int* position, Node_t** mas_tokenov);
Node_t* Get_Pow(int* position, Node_t** mas_tokenov);
Node_t* Get_E(int* position, Node_t** mas_tokenov);

differentiator_t Parsing(Node_t** mas_tokenov)
{
    int position = 0;
    differentiator_t tree = {};

    tree.root_node = Get_E(&position, mas_tokenov);

    return tree;
}

Node_t* Get_E(int* position, Node_t** mas_tokenov)
{
    printf("GetE\n");
    Node_t* val = Get_T(position, mas_tokenov);
    IF_ERROR_READING(val)
    while (mas_tokenov[*position]->value.op_code_t == ADD_CODE || mas_tokenov[*position]->value.op_code_t == SUB_CODE)
    {
        value_dif op = {.op_code_t = mas_tokenov[*position]->value.op_code_t};
        (*position)++;
        Node_t* val2 = Get_E(position, mas_tokenov);
        IF_ERROR_READING(val2)
        if (op.op_code_t == ADD_CODE)
            val = Make_Node(OPER_CODE, {.op_code_t = ADD_CODE}, val, val2);
        else
            val = Make_Node(OPER_CODE, {.op_code_t = SUB_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_T(int* position, Node_t** mas_tokenov)
{
    printf("GetT\n");
    Node_t* val = Get_P(position, mas_tokenov);
    IF_ERROR_READING(val)
    while (mas_tokenov[*position]->value.op_code_t == MUL_CODE || mas_tokenov[*position]->value.op_code_t == DIV_CODE)
    {
        char op = mas_tokenov[*position];
        (*position)++;
        Node_t* val2 = Get_P(position, mas_tokenov);
        IF_ERROR_READING(val2)
        if (op == '*')
            val = Make_Node(OPER_CODE, {.op_code_t = MUL_CODE}, val, val2);
        else
            val = Make_Node(OPER_CODE, {.op_code_t = DIV_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_Func(int* position, Node_t** mas_tokenov)
{
    printf("Get_func\n");
    Node_t* val = NULL;

    for (int i = 0; i < NUM_OPER - 4; i++)
    {
        if (strncmp(&mas_tokenov[*position], mas_tokenov_op[i].op_symb, (size_t)mas_tokenov_op[i].len) == 0 && mas_tokenov[*position + mas_tokenov_op[i].len] == '(')
        {
            (*position) +=  mas_tokenov_op[i].len + 1;
            val = Make_Node(OPER_CODE, {.op_code_t = mas_tokenov_op[i].op_code}, Get_E(position, mas_tokenov));
            IF_ERROR_READING(val)

            if (mas_tokenov[*position] == ')')
                (*position)++;
            else
                ERROR(__FILE__, __func__, __LINE__)
        }
    }

    return val;
}

Node_t* Get_Var(int* position, Node_t** mas_tokenov)
{
    Node_t* val = NULL;
    for (int i = 0; i < tree.num_var; i++)
    {
        if (strncmp(&mas_tokenov[*position], tree.hash_table[i].name, (size_t)tree.hash_table[i].hash_len) == 0)
        {
            val = Make_Node(VAR_CODE, {.var_ind = tree.hash_table[i].index});
            (*position) += tree.hash_table[i].hash_len;
            IF_ERROR_READING(val)
        }
    }

    return val;
}

Node_t* Get_P(int* position, Node_t** mas_tokenov)
{
    printf("GetP\n");
    if (mas_tokenov[*position] == '(')
    {
        (*position)++;
        Node_t* val = Get_E(position, mas_tokenov);
        IF_ERROR_READING(val)
        if (mas_tokenov[*position] == ')')
            (*position)++;
        else
            ERROR(__FILE__, __func__, __LINE__)

        return val;
    }
    Node_t* val = NULL;
    if ((val = Get_N(position, mas_tokenov)) == NULL)
        if ((val = Get_Func(position, mas_tokenov)) == NULL)
            if ((val = Get_Var(position, mas_tokenov)) == NULL)
            {
                ERROR(__FILE__, __func__, __LINE__)
                return NULL;
            }

    return val;
}

Node_t* Get_N(int* position, Node_t** mas_tokenov)
{
    printf("GetN\n");
    Node_t* val = Make_Node(NUM_CODE, {.num_t = 0}); 
    int count = 0;

    while ('0' <= mas_tokenov[*position] && mas_tokenov[*position] <= '9')
    {
        val->value.num_t = val->value.num_t * 10 + (mas_tokenov[*position] - '0');
        (*position)++;
        count++;
    }

    if (count == 0)
        return NULL;

    return val;
}