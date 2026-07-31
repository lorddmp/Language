#ifndef CALC_FUNCS
#define CALC_FUNCS
#include "structs_defines_types.h"

data_t ADD_CASE(Node_t* node, errors* err);
data_t SUB_CASE(Node_t* node, errors* err);
data_t MUL_CASE(Node_t* node, errors* err);
data_t DIV_CASE(Node_t* node, errors* err);
data_t STEPEN_CASE(Node_t* node, errors* err);
data_t SIN_CASE(Node_t* node, errors* err);
data_t COS_CASE(Node_t* node, errors* err);
data_t TAN_CASE(Node_t* node, errors* err);
data_t COTAN_CASE(Node_t* node, errors* err);
data_t ARCSIN_CASE(Node_t* node, errors* err);
data_t ARCCOS_CASE(Node_t* node, errors* err);
data_t ARCTAN_CASE(Node_t* node, errors* err);
data_t ARCCOTAN_CASE(Node_t* node, errors* err);
data_t LN_CASE(Node_t* node, errors* err);
data_t DOUBLE_EQ_CASE(Node_t* node, errors* err);
data_t NOT_EQ_CASE(Node_t* node, errors* err);
data_t MORE_CASE(Node_t* node, errors* err);
data_t MORE_OR_EQ_CASE(Node_t* node, errors* err);
data_t LESS_CASE(Node_t* node, errors* err);
data_t LESS_OR_EQ_CASE(Node_t* node, errors* err);

#endif