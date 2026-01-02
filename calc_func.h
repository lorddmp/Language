#ifndef CALC_FUNCS
#define CALC_FUNCS
#include "structs_defines_types.h"

double ADD_CASE(differentiator_t* tree, Node_t* node, errors* err);
double SUB_CASE(differentiator_t* tree, Node_t* node, errors* err);
double MUL_CASE(differentiator_t* tree, Node_t* node, errors* err);
double DIV_CASE(differentiator_t* tree, Node_t* node, errors* err);
double STEPEN_CASE(differentiator_t* tree, Node_t* node, errors* err);
double SIN_CASE(differentiator_t* tree, Node_t* node, errors* err);
double COS_CASE(differentiator_t* tree, Node_t* node, errors* err);
double TAN_CASE(differentiator_t* tree, Node_t* node, errors* err);
double COTAN_CASE(differentiator_t* tree, Node_t* node, errors* err);
double ARCSIN_CASE(differentiator_t* tree, Node_t* node, errors* err);
double ARCCOS_CASE(differentiator_t* tree, Node_t* node, errors* err);
double ARCTAN_CASE(differentiator_t* tree, Node_t* node, errors* err);
double ARCCOTAN_CASE(differentiator_t* tree, Node_t* node, errors* err);
double LN_CASE(differentiator_t* tree, Node_t* node, errors* err);

#endif