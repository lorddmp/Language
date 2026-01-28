#include "calc_func.h"
#include "calculator.h"
#include "tech_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ARITHMETIC_FUNC(funcname, sign)                                 \
double funcname(tree_t* tree, Node_t* node, errors* err)                \
{                                                                       \
    if (node->left == NULL || node->right == NULL)                      \
    {                                                                   \
        fprintf (stderr, "Empty node where it needs to be filled");     \
        *err = NODE_NULL;                                               \
        return 0;                                                       \
    }                                                                   \
    double a = Calculate(tree, node->left,err);            \
    double b = Calculate(tree, node->right,err);           \
    if (isnan(a) || isnan(b))                                           \
        return NAN;                                                     \
    return a sign b;                                                    \
}


ARITHMETIC_FUNC(ADD_CASE, +)
ARITHMETIC_FUNC(SUB_CASE, -)
ARITHMETIC_FUNC(MUL_CASE, *) 

#undef ARITHMETIC_FUNC

double DIV_CASE(tree_t* tree, Node_t* node, errors* err)
{
    if (node->left == NULL || node->right == NULL)
    {
        fprintf (stderr, "Empty node where it needs to be filled\n");
        *err = NODE_NULL;
        return 1;
    }
    double a = Calculate(tree, node->left, err);
    double b = Calculate(tree, node->right, err);
    if (Is_Zero(b))
    {
        fprintf (stderr, "NA NOL DELIT NELZYA!\n");
        *err = FORBIDDEN_NULL;
        return 1;
    }
    else if (isnan(a) || isnan(b))
        return NAN;
    return a/b;
}

double STEPEN_CASE(tree_t* tree, Node_t* node, errors* err)
{
    if (node->left == NULL || node->right == NULL)
    {
        fprintf (stderr, "Empty node where it needs to be filled\n");
        *err = NODE_NULL;
        return 1;
    }

    double a = Calculate(tree, node->left, err);
    double b = Calculate(tree, node->right, err);
    
    if (isnan(a) || isnan(b))
        return NAN;
    return pow(a,b);
}

#define TRIG_FUNCS(funcname, func)                                      \
double funcname(tree_t* tree, Node_t* node, errors* err)                \
{                                                                       \
    if (node->left == NULL && node->right == NULL)                      \
    {                                                                   \
        fprintf (stderr, "Empty node where it needs to be filled\n");   \
        *err = NODE_NULL;                                               \
        return 1;                                                       \
    }                                                                   \
    else if (node->left != NULL && node->right != NULL)                 \
    {                                                                   \
        fprintf (stderr, "Trig_func gets only one argument\n");         \
        *err = TOO_MANY_ARGS;                                           \
        return 1;                                                       \
    }                                                                   \
    double a = Calculate(tree, node->left,err);                         \
    if (isnan(a))                                                       \
        return NAN;                                                     \
    return a;                                                           \
}

TRIG_FUNCS(SIN_CASE, sin)
TRIG_FUNCS(COS_CASE, cos)
TRIG_FUNCS(TAN_CASE, tan)
double COTAN_CASE(tree_t* tree, Node_t* node, errors* err)
{
    if (node->left == NULL && node->right == NULL)
    {
        fprintf (stderr, "Empty node where it needs to be filled\n");
        *err = NODE_NULL;
        return 1;
    }
    else if (node->left != NULL && node->right != NULL)
    {
        fprintf (stderr, "Trig_func gets only one argument\n");
        *err = TOO_MANY_ARGS;
        return 1;
    }
    double a = Calculate(tree, node->left,err);
    if (isnan(a))
        return NAN;
    if (Is_Zero(tan(a)))
    {
        fprintf (stderr, "Cotangens doesn't exist at this point\n");;
        *err = FORBIDDEN_NULL;
        return 1;
    }
    return 1/tan(a);
}

TRIG_FUNCS(ARCSIN_CASE, asin)
TRIG_FUNCS(ARCCOS_CASE, acos)
TRIG_FUNCS(ARCTAN_CASE, atan)
double ARCCOTAN_CASE(tree_t* tree, Node_t* node, errors* err)
{
    if (node->left == NULL && node->right == NULL)
    {
        fprintf (stderr, "Empty node where it needs to be filled\n");
        *err = NODE_NULL;
        return 1;
    }
    else if (node->left != NULL && node->right != NULL)
    {
        fprintf (stderr, "Trig_func gets only one argument\n");
        *err = TOO_MANY_ARGS;
        return 1;
    }
    double a = Calculate(tree, node->left,err);

    if (isnan(a))
        return NAN;

    return M_PI/2 - tan(a);
}
#undef TRIG_FUNCS

double LN_CASE(tree_t* tree, Node_t* node, errors* err)
{
    if (node->left == NULL && node->right == NULL)
    {
        fprintf (stderr, "Empty node where it needs to be filled\n");
        *err = NODE_NULL;
        return 1;
    }
    else if (node->left != NULL && node->right != NULL)
    {
        fprintf (stderr, "Log_func gets only one argument\n");
        *err = TOO_MANY_ARGS;
        return 1;
    }
    double a = Calculate(tree, node->left,err);

    if (isnan(a))
        return NAN;

    if (!(a > 0))
    {
        fprintf (stderr, "Logarythm get argument >= 0\n");;
        *err = FORBIDDEN_NULL;
        return 1;
    }

    return log(a);
}