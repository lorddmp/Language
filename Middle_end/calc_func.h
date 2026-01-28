#ifndef CALC_FUNCS
#define CALC_FUNCS
#include "structs_defines_types.h"

double ADD_CASE(tree_t* tree, Node_t* node, errors* err);
double SUB_CASE(tree_t* tree, Node_t* node, errors* err);
double MUL_CASE(tree_t* tree, Node_t* node, errors* err);
double DIV_CASE(tree_t* tree, Node_t* node, errors* err);
double STEPEN_CASE(tree_t* tree, Node_t* node, errors* err);
double SIN_CASE(tree_t* tree, Node_t* node, errors* err);
double COS_CASE(tree_t* tree, Node_t* node, errors* err);
double TAN_CASE(tree_t* tree, Node_t* node, errors* err);
double COTAN_CASE(tree_t* tree, Node_t* node, errors* err);
double ARCSIN_CASE(tree_t* tree, Node_t* node, errors* err);
double ARCCOS_CASE(tree_t* tree, Node_t* node, errors* err);
double ARCTAN_CASE(tree_t* tree, Node_t* node, errors* err);
double ARCCOTAN_CASE(tree_t* tree, Node_t* node, errors* err);
double LN_CASE(tree_t* tree, Node_t* node, errors* err);

#endif