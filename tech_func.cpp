#include "tech_func.h"
#include "structs_defines_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>

void Clean_Buffer(void)
{
    while(getchar() != '\n');
}

void Skip_Spaces(int* position, char* massive)
{
    assert(massive);
    while (massive[*position] == ' ')
        (*position)++;
}

bool Is_Zero(double a)
{ 
    if (abs(a) < 1e-9)
        return true;
    else
        return false; 
}

Node_t* Make_Node(type_codes type, value_dif value, Node_t* left, Node_t* right)
{
    Node_t* new_node = (Node_t*)calloc(1, sizeof(Node_t));
    IF_ERROR(new_node);

    new_node->type = type;
    new_node->value = value;
    new_node->left = left;
    new_node->right = right;

    return new_node;
}

void Tree_Destructor(Node_t* node)
{
    assert(node);
    if (node->left != NULL)
        Tree_Destructor(node->left);

    if (node->right != NULL)
        Tree_Destructor(node->right);

    free(node);
}