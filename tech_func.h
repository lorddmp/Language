#ifndef TECH_FUNC
#define TECH_FUNC

#include "structs_defines_types.h"
#include <stdio.h>

Node_t* Make_Node(type_codes type, value_dif value, Node_t* left = NULL, Node_t* right = NULL);

void Tree_Destructor(Node_t* node);

void Clean_Buffer(void);

void Skip_Spaces(int* position, char* massive);

bool Is_Zero(double a);

#endif