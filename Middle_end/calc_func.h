#ifndef CALC_FUNCS
#define CALC_FUNCS
#include "structs_defines_types.h"

double ADD_CASE(Node_t* node, errors* err);
double SUB_CASE(Node_t* node, errors* err);
double MUL_CASE(Node_t* node, errors* err);
double DIV_CASE(Node_t* node, errors* err);
double STEPEN_CASE(Node_t* node, errors* err);
double SIN_CASE(Node_t* node, errors* err);
double COS_CASE(Node_t* node, errors* err);
double TAN_CASE(Node_t* node, errors* err);
double COTAN_CASE(Node_t* node, errors* err);
double ARCSIN_CASE(Node_t* node, errors* err);
double ARCCOS_CASE(Node_t* node, errors* err);
double ARCTAN_CASE(Node_t* node, errors* err);
double ARCCOTAN_CASE(Node_t* node, errors* err);
double LN_CASE(Node_t* node, errors* err);
double DOUBLE_EQ_CASE(Node_t* node, errors* err);
double NOT_EQ_CASE(Node_t* node, errors* err);
double MORE_CASE(Node_t* node, errors* err);
double MORE_OR_EQ_CASE(Node_t* node, errors* err);
double LESS_CASE(Node_t* node, errors* err);
double LESS_OR_EQ_CASE(Node_t* node, errors* err);

#endif