#include "structs_defines_types.h"
#include "tech_func.h"
#include "calculator.h"
#include "calc_func.h"
#include <stdio.h>
#include <math.h>

#define NUM_MATH_AND_LOG_OPER 20

struct functions{
    double(*funcname)(tree_t* tree, Node_t* node, errors* err);
    enum oper_codes func_code;
};

functions mas_functions[NUM_MATH_AND_LOG_OPER] = {
    {ADD_CASE, ADD_CODE},
    {SUB_CASE, SUB_CODE},
    {MUL_CASE, MUL_CODE},
    {DIV_CASE, DIV_CODE},
    {STEPEN_CASE, POW_CODE},
    {SIN_CASE, SIN_CODE}, 
    {COS_CASE, COS_CODE},
    {TAN_CASE, TAN_CODE},
    {COTAN_CASE, COTAN_CODE},
    {ARCSIN_CASE, ARCSIN_CODE},
    {ARCCOS_CASE, ARCCOS_CODE},
    {ARCTAN_CASE, ARCTAN_CODE},
    {ARCCOTAN_CASE, ARCCOTAN_CODE},
    {LN_CASE, LN_CODE},
    {DOUBLE_EQ_CASE, DOUBLE_EQ_CODE},
    {NOT_EQ_CASE, NOT_EQ_CODE},
    {MORE_CASE, MORE_CODE},
    {MORE_OR_EQ_CASE, MORE_OR_EQ_CODE},
    {LESS_CASE, LESS_CODE},
    {LESS_OR_EQ_CASE, LESS_OR_EQ_CODE},
};

double Calculate(tree_t* tree, Node_t* node, errors* err)
{
    if (node == NULL)
        return 0;
    if (node->type == NUM_CODE)
        return node->value.num_t;
    else if (node->type == NAME_CODE)
        return NAN;
    else
    {
        double a = Calculate(tree, node->left, err);
        double b = Calculate(tree, node->right, err);
        if (isnan(a) || isnan(b))
            return NAN;
        
        for (int i = 0; i < NUM_MATH_AND_LOG_OPER; i++)
            if (mas_functions[i].func_code == node->value.op_code_t)
            {
                double c = mas_functions[i].funcname(tree, node, err);
                if (node->left != NULL)
                {
                    Tree_Destructor(node->left);
                    node->left = NULL;
                }
                if (node->right != NULL)
                {
                    Tree_Destructor(node->right);
                    node->right = NULL;
                }
                node->type = NUM_CODE;
                node->value.num_t = c;
                return c;
            }
        return 0;
    }
}