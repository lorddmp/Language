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
    fprintf(stderr, "Error in file: %s, function: %s, line: %d\n", filename, funcname, line);       \
    return NULL;                                                                                    \
}

#define IF_ERROR_READING(a)                                 \
if (a == NULL)                                              \
    return NULL;                                            \

#define NULL_IF_NODE_WRONG(a)                               \
if (token_array[*pos_token_array]->value.op_code_t != a)    \
    return NULL;                                            \

#define ERROR_IF_NODE_WRONG(a, filename, funcname, line)    \
if (token_array[*pos_token_array]->value.op_code_t != a)    \
    ERROR(filename, funcname, line)                         \

Node_t* Get_End(int* pos_token_array, Node_t** token_array);
Node_t* Get_Tree(int* pos_token_array, Node_t** token_array);
Node_t* Get_Decl(int* pos_token_array, Node_t** token_array);

Node_t* Get_Func_Init(int* pos_token_array, Node_t** token_array);
Node_t* Get_Var_Init(int* pos_token_array, Node_t** token_array);
Node_t* Get_Equat(int* pos_token_array, Node_t** token_array);
Node_t* Get_Expr(int* pos_token_array, Node_t** token_array);
Node_t* Get_If_While(int* pos_token_array, Node_t** token_array);
Node_t* Get_Printf(int* pos_token_array, Node_t** token_array);

Node_t* Get_AS(int* pos_token_array, Node_t** token_array);
Node_t* Get_MD(int* pos_token_array, Node_t** token_array);
Node_t* Get_Pow(int* pos_token_array, Node_t** token_array);
Node_t* Get_Object(int* pos_token_array, Node_t** token_array);
Node_t* Get_Paren(int* pos_token_array, Node_t** token_array);
Node_t* Get_Fig_Paren(int* pos_token_array, Node_t** token_array);
Node_t* Get_Num(int* pos_token_array, Node_t** token_array);
Node_t* Get_TrigLn(int* pos_token_array, Node_t** token_array);
Node_t* Get_Func_Call(int* pos_token_array, Node_t** token_array);
Node_t* Get_Name(int* pos_token_array, Node_t** token_array);

void Set_Parent(Node_t* node, Node_t* parent_node);

Node_t* Parsing(Node_t** token_array)
{
    int pos_token_array = 0;

    Node_t* root_node = Get_End(&pos_token_array, token_array);

    IF_ERROR_READING(root_node);

    Set_Parent(root_node, NULL);

    return root_node;
}

Node_t* Get_End(int* pos_token_array, Node_t** token_array)
{
    // printf("GetEnd\n");
    Node_t* val = Get_Decl(pos_token_array, token_array);

    if (val == NULL)
        ERROR(__FILE__, __func__, __LINE__)

    Node_t* tree_root_val = Make_Node(TREE_ROOT_CODE, {}, NULL, val);
    tree_root_val->left = Get_Tree(pos_token_array, token_array);

    ERROR_IF_NODE_WRONG(END_CODE, __FILE__, __func__, __LINE__)
    
    // if (token_array[*pos_token_array]->value.op_code_t != END_CODE)
    // {
    //     printf("type = %d\n", token_array[*pos_token_array]->type); 
    //     printf("value_num? = %lg\n", token_array[*pos_token_array]->value.num_t); 
    //     printf("value_var? = %d\n", token_array[*pos_token_array]->value.name_ind); 
    //     printf("value_op? = %d\n", token_array[*pos_token_array]->value.op_code_t); 
    //     ERROR(__FILE__, __func__, __LINE__)
    // }
    
    return tree_root_val;
}

Node_t* Get_Tree(int* pos_token_array, Node_t** token_array)
{
    // printf("GetTree\n");
    Node_t* val = Get_Decl(pos_token_array, token_array);
    IF_ERROR_READING(val)

    Node_t* val_body_root = Make_Node(BODY_CODE, {}, NULL, val);
    Node_t* val2 = val_body_root;

    while (token_array[*pos_token_array]->value.op_code_t != END_CODE && (val = Get_Decl(pos_token_array, token_array)) != NULL)
    {
        val2->left = Make_Node(BODY_CODE, {}, NULL, val);
        val2 = val2->left;
    }

    return val_body_root;
}

Node_t* Get_Decl(int* pos_token_array, Node_t** token_array)
{
    // printf("GetDecl\n");

    Node_t* val = NULL;
    if ((val = Get_Func_Init(pos_token_array, token_array)) == NULL)
        if ((val = Get_Var_Init(pos_token_array, token_array)) == NULL)
            if ((val = Get_Equat(pos_token_array, token_array)) == NULL)
                if ((val = Get_Expr(pos_token_array, token_array)) == NULL)
                    if ((val = Get_If_While(pos_token_array, token_array)) == NULL)
                        if ((val = Get_Printf(pos_token_array, token_array)) == NULL)
                            return NULL;

    ERROR_IF_NODE_WRONG(SEMICOLONE_CODE, __FILE__, __func__, __LINE__)
    
    (*pos_token_array)++;
    return val;
}

Node_t* Get_Func_Init(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_Func_Init\n");
    NULL_IF_NODE_WRONG(FUNC_INIT_CODE)

    (*pos_token_array)++;

    Node_t* val = Get_Name(pos_token_array, token_array);
    IF_ERROR_READING(val)

    ERROR_IF_NODE_WRONG(EQUA_CODE, __FILE__, __func__, __LINE__)
    (*pos_token_array)++;

    Node_t* val2 = Get_Fig_Paren(pos_token_array, token_array);
    if (val2 == NULL)
        ERROR(__FILE__, __func__, __LINE__)
        
    val = Make_Node(OPER_CODE, {.op_code_t = FUNC_INIT_CODE}, val, val2);
    return val;
}

Node_t* Get_Var_Init(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_Var_Init\n");
    NULL_IF_NODE_WRONG(VAR_INIT_CODE)

    (*pos_token_array)++;
    Node_t* val = Get_Name(pos_token_array, token_array);
    IF_ERROR_READING(val)

    ERROR_IF_NODE_WRONG(EQUA_CODE, __FILE__, __func__, __LINE__)
    (*pos_token_array)++;

    Node_t* val2 = Get_Expr(pos_token_array, token_array);
    if (val2 == NULL)
        ERROR(__FILE__, __func__, __LINE__)

    val = Make_Node(OPER_CODE, {.op_code_t = VAR_INIT_CODE}, val, val2);
    return val;
}

Node_t* Get_Equat(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_Equat\n");
    NULL_IF_NODE_WRONG(CHANGE_VAR_CODE)

    (*pos_token_array)++;
    Node_t* val = Get_Name(pos_token_array, token_array);
    IF_ERROR_READING(val)

    ERROR_IF_NODE_WRONG(EQUA_CODE, __FILE__, __func__, __LINE__)

    (*pos_token_array)++;
    Node_t* val2 = Get_Expr(pos_token_array, token_array);
    if (val2 == NULL)
        ERROR(__FILE__, __func__, __LINE__)

    val = Make_Node(OPER_CODE, {.op_code_t = EQUA_CODE}, val, val2);
    return val;

}

Node_t* Get_Expr(int* pos_token_array, Node_t** token_array)
{
    // printf("GetExpr\n");
    Node_t* val = Get_AS(pos_token_array, token_array);
    IF_ERROR_READING(val)

    while (token_array[*pos_token_array]->value.op_code_t >= DOUBLE_EQ_CODE and 
            token_array[*pos_token_array]->value.op_code_t <= LESS_OR_EQ_CODE)
    {
        oper_codes oper = token_array[*pos_token_array]->value.op_code_t;
        (*pos_token_array)++;
        Node_t* val2 = Get_AS(pos_token_array, token_array);
        IF_ERROR_READING(val2)
        val = Make_Node(OPER_CODE, {.op_code_t = oper}, val, val2);
    }
    return val;
}

Node_t* Get_If_While(int* pos_token_array, Node_t** token_array)
{
    // printf("GetIf_While\n");
    if (token_array[*pos_token_array]->value.op_code_t != IF_CODE && token_array[*pos_token_array]->value.op_code_t != WHILE_CODE)
        return NULL;

    oper_codes op = token_array[*pos_token_array]->value.op_code_t;

    (*pos_token_array)++;
    Node_t* val = Get_Paren(pos_token_array, token_array);
    IF_ERROR_READING(val)
    Node_t* val2 = Get_Fig_Paren(pos_token_array, token_array);
    IF_ERROR_READING(val2)

    val = Make_Node(OPER_CODE, {.op_code_t = op}, val, val2);
    return val;
}

Node_t* Get_Printf(int* pos_token_array, Node_t** token_array)
{
    // printf("GetPrintf\n");
    NULL_IF_NODE_WRONG(PRINTF_CODE)

    (*pos_token_array)++;
    Node_t* val = Get_Paren(pos_token_array, token_array);
    IF_ERROR_READING(val)

    val = Make_Node(OPER_CODE, {.op_code_t = PRINTF_CODE}, val, NULL);
    return val;
}

Node_t* Get_AS(int* pos_token_array, Node_t** token_array)
{
    // printf("GetAS\n");
    Node_t* val = Get_MD(pos_token_array, token_array);
    IF_ERROR_READING(val)
    // printf("pos = %d\n", token_array[*pos_token_array]->value.op_code_t);
    while (token_array[*pos_token_array]->value.op_code_t == ADD_CODE || token_array[*pos_token_array]->value.op_code_t == SUB_CODE)
    {
        value_dif op = {.op_code_t = token_array[*pos_token_array]->value.op_code_t};
        (*pos_token_array)++;
        Node_t* val2 = Get_AS(pos_token_array, token_array);
        IF_ERROR_READING(val2)
        if (op.op_code_t == ADD_CODE)
            val = Make_Node(OPER_CODE, {.op_code_t = ADD_CODE}, val, val2);
        else
            val = Make_Node(OPER_CODE, {.op_code_t = SUB_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_MD(int* pos_token_array, Node_t** token_array)
{
    // printf("GetMD\n");
    Node_t* val = Get_Pow(pos_token_array, token_array);
    IF_ERROR_READING(val)
    while (token_array[*pos_token_array]->value.op_code_t == MUL_CODE || token_array[*pos_token_array]->value.op_code_t == DIV_CODE)
    {
        value_dif op = {.op_code_t = token_array[*pos_token_array]->value.op_code_t};
        (*pos_token_array)++;
        Node_t* val2 = Get_MD(pos_token_array, token_array);
        IF_ERROR_READING(val2)
        if (op.op_code_t == MUL_CODE)
            val = Make_Node(OPER_CODE, {.op_code_t = MUL_CODE}, val, val2);
        else
            val = Make_Node(OPER_CODE, {.op_code_t = DIV_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_Pow(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_Pow\n");
    Node_t* val = Get_Object(pos_token_array, token_array);
    IF_ERROR_READING(val)
    while (token_array[*pos_token_array]->value.op_code_t == POW_CODE)
    {
        (*pos_token_array)++;
        Node_t* val2 = Get_Pow(pos_token_array, token_array);
        IF_ERROR_READING(val2)
        val = Make_Node(OPER_CODE, {.op_code_t = POW_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_Object(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_Object\n");
    Node_t* val = NULL;

    if ((val = Get_Paren(pos_token_array, token_array)) == NULL)
        if ((val = Get_Num(pos_token_array, token_array)) == NULL)
            if ((val = Get_TrigLn(pos_token_array, token_array)) == NULL)
                if ((val = Get_Func_Call(pos_token_array, token_array)) == NULL)
                    if ((val = Get_Name(pos_token_array, token_array)) == NULL)
                        return NULL;

    return val;
}

Node_t* Get_Paren(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_Paren\n");
    NULL_IF_NODE_WRONG(OPEN_BRAC_CODE)
        
    (*pos_token_array)++;
    Node_t* val = Get_Expr(pos_token_array, token_array);
    IF_ERROR_READING(val)

    ERROR_IF_NODE_WRONG(CLOSED_BRAC_CODE, __FILE__, __func__, __LINE__)
        
    (*pos_token_array)++;
    return val;
}

Node_t* Get_Fig_Paren(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_Fig_Paren\n");
    NULL_IF_NODE_WRONG(OPEN_FIG_BRAC_CODE)
        
    (*pos_token_array)++;
    Node_t* val = Get_Tree(pos_token_array, token_array);
    IF_ERROR_READING(val)

    ERROR_IF_NODE_WRONG(CLOSED_FIG_BRAC_CODE, __FILE__, __func__, __LINE__)
        
    (*pos_token_array)++;
    return val;
}

Node_t* Get_Num(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_Num\n");

    if (token_array[*pos_token_array]->type == NUM_CODE)
    {
        Node_t* val = Make_Node(NUM_CODE, {.num_t = token_array[*pos_token_array]->value.num_t}); 
        // printf("val = %lg\n", val->value.num_t);
        (*pos_token_array)++;
        return val;
    }
    else
        return NULL;
}

Node_t* Get_TrigLn(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_trigln\n");
    Node_t* val = NULL;

    if (token_array[*pos_token_array]->value.op_code_t >= SIN_CODE && token_array[*pos_token_array]->value.op_code_t <= LN_CODE)
    {
        value_dif op = {.op_code_t = token_array[*pos_token_array]->value.op_code_t};
        (*pos_token_array)++;
        val = Make_Node(OPER_CODE, {.op_code_t = op.op_code_t}, Get_Paren(pos_token_array, token_array)); //TODO - fix
        IF_ERROR_READING(val)
    }

    return val;
}

Node_t* Get_Func_Call(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_func_call\n");
    NULL_IF_NODE_WRONG(FUNC_CALL_CODE)

    (*pos_token_array)++;
    Node_t* val = Get_Name(pos_token_array, token_array);
    IF_ERROR_READING(val)

    Node_t* val2 = Make_Node(OPER_CODE, {.op_code_t = FUNC_CALL_CODE}, val);
    IF_ERROR_READING(val2)

    return val2;
}

Node_t* Get_Name(int* pos_token_array, Node_t** token_array)
{
    // printf("Get_Name\n");
    Node_t* val = NULL;

    if (token_array[*pos_token_array]->type == NAME_CODE)
    {
        val = Make_Node(NAME_CODE, {.name_ind = token_array[*pos_token_array]->value.name_ind});
        (*pos_token_array)++;
        IF_ERROR_READING(val)
    }

    return val;
}

void Set_Parent(Node_t* node, Node_t* parent_node)
{
    if (node->left != NULL)
        Set_Parent(node->left, node);
    if (node->right != NULL)
        Set_Parent(node->right, node);

    node->parent = parent_node;
}