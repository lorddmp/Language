#include "calc_func.h"
#include "calculator.h"
#include "tech_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//ЗАЧЕМ ПЕРЕДАВАТЬ tree???????????????

#define ARITHMETIC_AND_LOG_FUNC(funcname, sign)                         \
double funcname(Node_t* node, errors* err)                \
{                                                                       \
    if (node->left == NULL || node->right == NULL)                      \
    {                                                                   \
        fprintf (stderr, "Empty node where it needs to be filled");     \
        *err = NODE_NULL;                                               \
        return 0;                                                       \
    }                                                                   \
    double a = Calculate(node->left,err);            \
    double b = Calculate(node->right,err);           \
    if (isnan(a) || isnan(b))                                           \
        return NAN;                                                     \
    return a sign b;                                                    \
}


ARITHMETIC_AND_LOG_FUNC(ADD_CASE, +)
ARITHMETIC_AND_LOG_FUNC(SUB_CASE, -)
ARITHMETIC_AND_LOG_FUNC(MUL_CASE, *) 


double DIV_CASE(Node_t* node, errors* err)
{
    if (node->left == NULL || node->right == NULL)
    {
        fprintf (stderr, "Empty node where it needs to be filled\n");
        *err = NODE_NULL;
        return 1;
    }
    double a = Calculate(node->left, err);
    double b = Calculate(node->right, err);
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

double DOUBLE_EQ_CASE(Node_t* node, errors* err)
{
    if (node->left == NULL || node->right == NULL)
    {
        fprintf (stderr, "Empty node where it needs to be filled\n");
        *err = NODE_NULL;
        return 1;
    }
    double a = Calculate(node->left, err);
    double b = Calculate(node->right, err);
    
    if (isnan(a) || isnan(b))
        return NAN;
    return Is_Zero(a-b);
}

double NOT_EQ_CASE(Node_t* node, errors* err)
{
    if (node->left == NULL || node->right == NULL)
    {
        fprintf (stderr, "Empty node where it needs to be filled\n");
        *err = NODE_NULL;
        return 1;
    }
    double a = Calculate(node->left, err);
    double b = Calculate(node->right, err);
    
    if (isnan(a) || isnan(b))
        return NAN;
    return !(Is_Zero(a-b));
}
ARITHMETIC_AND_LOG_FUNC(MORE_CASE, >) 
ARITHMETIC_AND_LOG_FUNC(MORE_OR_EQ_CASE, >=) 
ARITHMETIC_AND_LOG_FUNC(LESS_CASE, <) 
ARITHMETIC_AND_LOG_FUNC(LESS_OR_EQ_CASE, <=) 

#undef ARITHMETIC_AND_LOG_FUNC

double STEPEN_CASE(Node_t* node, errors* err)
{
    if (node->left == NULL || node->right == NULL)
    {
        fprintf (stderr, "Empty node where it needs to be filled\n");
        *err = NODE_NULL;
        return 1;
    }

    double a = Calculate(node->left, err);
    double b = Calculate(node->right, err);
    
    if (isnan(a) || isnan(b))
        return NAN;
    return pow(a,b);
}

#define TRIG_FUNCS(funcname, func)                                      \
double funcname(Node_t* node, errors* err)                \
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
    double a = func(Calculate(node->left,err));                   \
    if (isnan(a))                                                       \
        return NAN;                                                     \
    return a;                                                           \
}

TRIG_FUNCS(SIN_CASE, sin)
TRIG_FUNCS(COS_CASE, cos)
TRIG_FUNCS(TAN_CASE, tan)
double COTAN_CASE(Node_t* node, errors* err)
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
    double a = Calculate(node->left,err);
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
double ARCCOTAN_CASE(Node_t* node, errors* err)
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
    double a = Calculate(node->left,err);

    if (isnan(a))
        return NAN;

    return M_PI/2 - tan(a);
}
#undef TRIG_FUNCS

double LN_CASE(Node_t* node, errors* err)
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
    double a = Calculate(node->left,err);

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