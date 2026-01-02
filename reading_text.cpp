#include "structs_defines_types.h"
#include "tech_func.h"

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
    
differentiator_t Create_Tree_Structure(void);

Node_t* Get_N(int* position, char* massive);
Node_t* Get_Var(differentiator_t tree, int* position, char* massive);
Node_t* Get_Func(differentiator_t tree, int* position, char* massive);
Node_t* Get_P(differentiator_t tree,int* position, char* massive);
Node_t* Get_T(differentiator_t tree, int* position, char* massive);
Node_t* Get_E(differentiator_t tree, int* position, char* massive);
Node_t* Get_G(differentiator_t tree, int* position, char* massive);

differentiator_t Read_Expression(void)
{
    differentiator_t tree = Create_Tree_Structure();

    FILE* fp = fopen(READ_TREE_FILE, "r");
    struct stat stat1 = {};
    int descriptor = fileno(fp);
    int position = 0;

    fstat(descriptor, &stat1);

    char* massive = (char*)calloc((size_t)stat1.st_size + 1, sizeof(char));

    fread(massive, sizeof(char), (size_t)stat1.st_size, fp);
    tree.root_node = Get_G(tree, &position, massive);

    return tree;
}

Node_t* Get_G(differentiator_t tree, int* position, char* massive)
{
    printf("GetG\n");
    Node_t* val = Get_E(tree, position, massive);

    if (massive[*position] != '$')
        ERROR(__FILE__, __func__, __LINE__)

    (*position)++;
    return val;        
}

Node_t* Get_E(differentiator_t tree, int* position, char* massive)
{
    printf("GetE\n");
    Node_t* val = Get_T(tree, position, massive);
    IF_ERROR_READING(val)
    while (massive[*position] == '+' || massive[*position] == '-')
    {
        char op = massive[*position];
        (*position)++;
        Node_t* val2 = Get_E(tree, position, massive);
        IF_ERROR_READING(val2)
        if (op == '+')
            val = Make_Node(OPER_CODE, {.op_code_t = ADD_CODE}, val, val2);
        else
            val = Make_Node(OPER_CODE, {.op_code_t = SUB_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_T(differentiator_t tree, int* position, char* massive)
{
    printf("GetT\n");
    Node_t* val = Get_P(tree, position, massive);
    IF_ERROR_READING(val)
    while (massive[*position] == '*' || massive[*position] == '/' || massive[*position] == '^')
    {
        char op = massive[*position];
        (*position)++;
        Node_t* val2 = Get_P(tree, position, massive);
        IF_ERROR_READING(val2)
        if (op == '*')
            val = Make_Node(OPER_CODE, {.op_code_t = MUL_CODE}, val, val2);
        else if (op == '/')
            val = Make_Node(OPER_CODE, {.op_code_t = DIV_CODE}, val, val2);
        else
            val = Make_Node(OPER_CODE, {.op_code_t = STEPEN_CODE}, val, val2);
    }
    return val;
}

Node_t* Get_Func(differentiator_t tree, int* position, char* massive)
{
    printf("Get_func\n");
    Node_t* val = NULL;

    for (int i = 0; i < NUM_OPER - 4; i++)
    {
        if (strncmp(&massive[*position], massive_op[i].op_symb, (size_t)massive_op[i].len) == 0 && massive[*position + massive_op[i].len] == '(')
        {
            (*position) +=  massive_op[i].len + 1;
            val = Make_Node(OPER_CODE, {.op_code_t = massive_op[i].op_code}, Get_E(tree, position, massive));
            IF_ERROR_READING(val)

            if (massive[*position] == ')')
                (*position)++;
            else
                ERROR(__FILE__, __func__, __LINE__)
        }
    }

    return val;
}

Node_t* Get_Var(differentiator_t tree, int* position, char* massive)
{
    Node_t* val = NULL;
    for (int i = 0; i < tree.num_var; i++)
    {
        if (strncmp(&massive[*position], tree.hash_table[i].name, (size_t)tree.hash_table[i].hash_len) == 0)
        {
            val = Make_Node(VAR_CODE, {.var_ind = tree.hash_table[i].index});
            (*position) += tree.hash_table[i].hash_len;
            IF_ERROR_READING(val)
        }
    }

    return val;
}

Node_t* Get_P(differentiator_t tree, int* position, char* massive)
{
    printf("GetP\n");
    if (massive[*position] == '(')
    {
        (*position)++;
        Node_t* val = Get_E(tree, position, massive);
        IF_ERROR_READING(val)
        if (massive[*position] == ')')
            (*position)++;
        else
            ERROR(__FILE__, __func__, __LINE__)

        return val;
    }
    Node_t* val = NULL;
    if ((val = Get_N(position, massive)) == NULL)
        if ((val = Get_Func(tree, position, massive)) == NULL)
            if ((val = Get_Var(tree, position, massive)) == NULL)
            {
                ERROR(__FILE__, __func__, __LINE__)
                return NULL;
            }

    return val;
}

Node_t* Get_N(int* position, char* massive)
{
    printf("GetN\n");
    Node_t* val = Make_Node(NUM_CODE, {.num_t = 0}); 
    int count = 0;

    while ('0' <= massive[*position] && massive[*position] <= '9')
    {
        val->value.num_t = val->value.num_t * 10 + (massive[*position] - '0');
        (*position)++;
        count++;
    }

    if (count == 0)
        return NULL;

    return val;
}

differentiator_t Create_Tree_Structure(void)
{
    differentiator_t tree = {};
    printf("Please write number of variables: \n");
    scanf("%d", &tree.num_var);

    tree.hash_table = (hash*)calloc((size_t)tree.num_var, sizeof(hash));

    printf ("Please enter all variables(var -space- var...): ");
    for (int i = 0; i < tree.num_var; i++)
    {
        scanf("%s", tree.hash_table[i].name);
        tree.hash_table[i].index = i;
        tree.hash_table[i].hash_len = (int)strlen(tree.hash_table[i].name);
    }

    return tree;
}