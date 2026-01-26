#include "structs_defines_types.h"
#include "tech_func.h"
#include "parsing.h"

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// РБНФ
// End -> Decl* $
// Decl -> Init | Expr | If
// Init -> 'var' Var '=' Expr
// If -> 'if' Paren '{' Decl* '}'
// Expr -> AS ['==' ... Expr]*
// AS  -> MD ['+'|'-' AS]*
// MD  -> Pow ['*'|'/' MD]*
// Pow -> P ['^' Pow]*
// P -> Paren | N | Func | Var
// Paren -> '(' AS ')'
// N -> 0-9*
// Func -> 'sin' Paren
// Var -> a-zA-Z_

#define ERROR(filename, funcname, line)                                                             \
{                                                                                                   \
    fprintf(stderr, "Error in file: %s, function: %s, line: %d", filename, funcname, line);         \
    return NULL;                                                                                    \
}

#define IF_ERROR_READING(a)                                         \
if (a == NULL)                                              \
    return NULL;                                            \

Node_t* Get_End(int* position, Node_t** mas_tokenov);
Node_t* Get_Expr(int* position, Node_t** mas_tokenov);
Node_t* Get_AS(int* position, Node_t** mas_tokenov);
Node_t* Get_MD(int* position, Node_t** mas_tokenov);
Node_t* Get_Pow(int* position, Node_t** mas_tokenov);
Node_t* Get_Object(int* position, Node_t** mas_tokenov);
Node_t* Get_Paren(int* position, Node_t** mas_tokenov);
Node_t* Get_N(int* position, Node_t** mas_tokenov);
Node_t* Get_Func(int* position, Node_t** mas_tokenov);
Node_t* Get_Var(int* position, Node_t** mas_tokenov);

tree_t Parsing(Node_t** mas_tokenov)
{
    int position = 0;
    tree_t tree = {};

    tree.root_node = Get_End(&position, mas_tokenov);

    return tree;
}

Node_t* Get_End(int* position, Node_t** mas_tokenov)
{
    printf("GetEnd\n");
    Node_t* val = Get_Expr(position, mas_tokenov);
    IF_ERROR_READING(val)
    
    if (mas_tokenov[*position]->value.op_code_t != END_CODE)
    {
        printf("type = %d\n", mas_tokenov[*position]->type); 
        printf("value_num? = %lg\n", mas_tokenov[*position]->value.num_t); 
        printf("value_var? = %d\n", mas_tokenov[*position]->value.var_ind); 
        printf("value_op? = %d\n", mas_tokenov[*position]->value.op_code_t); 
        ERROR(__FILE__, __func__, __LINE__)
    }
    
    return val;
}

Node_t* Get_Expr(int* position, Node_t** mas_tokenov)
{
    printf("GetExpr\n");
    Node_t* val = Get_AS(position, mas_tokenov);
    IF_ERROR_READING(val)

     while (mas_tokenov[*position]->value.op_code_t == DOUBLE_EQ_CODE)
    {
        (*position)++;
        Node_t* val2 = Get_AS(position, mas_tokenov);
        IF_ERROR_READING(val2)
        val = Make_Node(OPER_CODE, {.op_code_t = DOUBLE_EQ_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_AS(int* position, Node_t** mas_tokenov)
{
    printf("GetAS before\n");
    Node_t* val = Get_MD(position, mas_tokenov);
    IF_ERROR_READING(val)
    // printf("pos = %d\n", mas_tokenov[*position]->value.op_code_t);
    while (mas_tokenov[*position]->value.op_code_t == ADD_CODE || mas_tokenov[*position]->value.op_code_t == SUB_CODE)
    {
        value_dif op = {.op_code_t = mas_tokenov[*position]->value.op_code_t};
        (*position)++;
        Node_t* val2 = Get_AS(position, mas_tokenov);
        IF_ERROR_READING(val2)
        if (op.op_code_t == ADD_CODE)
            val = Make_Node(OPER_CODE, {.op_code_t = ADD_CODE}, val, val2);
        else
            val = Make_Node(OPER_CODE, {.op_code_t = SUB_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_MD(int* position, Node_t** mas_tokenov)
{
    printf("GetMD before\n");
    Node_t* val = Get_Pow(position, mas_tokenov);
    IF_ERROR_READING(val)
    while (mas_tokenov[*position]->value.op_code_t == MUL_CODE || mas_tokenov[*position]->value.op_code_t == DIV_CODE)
    {
        value_dif op = {.op_code_t = mas_tokenov[*position]->value.op_code_t};
        (*position)++;
        Node_t* val2 = Get_MD(position, mas_tokenov);
        IF_ERROR_READING(val2)
        if (op.op_code_t == MUL_CODE)
            val = Make_Node(OPER_CODE, {.op_code_t = MUL_CODE}, val, val2);
        else
            val = Make_Node(OPER_CODE, {.op_code_t = DIV_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_Pow(int* position, Node_t** mas_tokenov)
{
    printf("Get_Pow before\n");
    Node_t* val = Get_Object(position, mas_tokenov);
    IF_ERROR_READING(val)
    while (mas_tokenov[*position]->value.op_code_t == STEPEN_CODE)
    {
        (*position)++;
        Node_t* val2 = Get_Pow(position, mas_tokenov);
        IF_ERROR_READING(val2)
        val = Make_Node(OPER_CODE, {.op_code_t = STEPEN_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_Object(int* position, Node_t** mas_tokenov)
{
    printf("GetObject\n");
    
    Node_t* val = NULL;

    if ((val = Get_Paren(position, mas_tokenov)) == NULL)
        if ((val = Get_N(position, mas_tokenov)) == NULL)
            if ((val = Get_Func(position, mas_tokenov)) == NULL)
                if ((val = Get_Var(position, mas_tokenov)) == NULL)
                {
                    ERROR(__FILE__, __func__, __LINE__)
                    return NULL;
                }

    return val;
}

Node_t* Get_Paren(int* position, Node_t** mas_tokenov)
{
    if (mas_tokenov[*position]->value.op_code_t == OPEN_BRAC_CODE)
    {
        (*position)++;
        Node_t* val = Get_AS(position, mas_tokenov);
        IF_ERROR_READING(val)
        if (mas_tokenov[*position]->value.op_code_t == CLOSED_BRAC_CODE)
            (*position)++;
        else
            ERROR(__FILE__, __func__, __LINE__)

        return val;
    }
    else
        return NULL;
}

Node_t* Get_N(int* position, Node_t** mas_tokenov)
{
    printf("GetN\n");

    if (mas_tokenov[*position]->type == NUM_CODE)
    {
        Node_t* val = Make_Node(NUM_CODE, {.num_t = mas_tokenov[*position]->value.num_t}); 
        printf("val = %lg\n", val->value.num_t);
        (*position)++;
        return val;
    }
    else
        return NULL;
}

Node_t* Get_Func(int* position, Node_t** mas_tokenov)
{
    printf("Get_func\n");
    Node_t* val = NULL;

    if (mas_tokenov[*position]->value.op_code_t >= SIN_CODE && mas_tokenov[*position]->value.op_code_t <= LN_CODE)
    {
        value_dif op = {.op_code_t = mas_tokenov[*position]->value.op_code_t};
        (*position)++;
        val = Make_Node(OPER_CODE, {.op_code_t = op.op_code_t}, Get_Paren(position, mas_tokenov));
        IF_ERROR_READING(val)
    }

    return val;
}

Node_t* Get_Var(int* position, Node_t** mas_tokenov)
{
    printf("Get_Var\n");
    Node_t* val = NULL;

    if (mas_tokenov[*position]->type == VAR_CODE)
    {
        val = Make_Node(VAR_CODE, {.var_ind = mas_tokenov[*position]->value.var_ind});
        (*position)++;
        IF_ERROR_READING(val)
    }

    return val;
}
