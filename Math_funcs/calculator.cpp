#include "structs_defines_types.h"
#include "tech_func.h"
#include "calculator.h"
#include "calc_func.h"
#include <stdio.h>
#include <math.h>

struct functions{
    double(*funcname)(differentiator_t* tree, Node_t* node, errors* err);
    enum oper_codes func_code;
};

functions mas_functions[NUM_OPER] = {
    {ADD_CASE, ADD_CODE},
    {SUB_CASE, SUB_CODE},
    {MUL_CASE, MUL_CODE},
    {DIV_CASE, DIV_CODE},
    {STEPEN_CASE, STEPEN_CODE},
    {SIN_CASE, SIN_CODE},
    {COS_CASE, COS_CODE},
    {TAN_CASE, TAN_CODE},
    {COTAN_CASE, COTAN_CODE},
    {ARCSIN_CASE, ARCSIN_CODE},
    {ARCCOS_CASE, ARCCOS_CODE},
    {ARCTAN_CASE, ARCTAN_CODE},
    {ARCCOTAN_CASE, ARCCOTAN_CODE},
    {LN_CASE, LN_CODE},

};

double Calculate(differentiator_t* tree, Node_t* node, errors* err)
{
    if (node == NULL)
        return 0;
    if (node->type == NUM_CODE)
        return node->value.num_t;
    else if (node->type == VAR_CODE)
        return NAN;
    else
    {
        double a = Calculate(tree, node->left, err);
        double b = Calculate(tree, node->right, err);
        if (isnan(a) || isnan(b))
            return NAN;
        
        for (int i = 0; i < NUM_OPER; i++)
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