#include "print_tree.h"
#include <assert.h>

void Print_Node(Node_t* node, FILE* fp);

void Print_Node(Node_t* node, FILE* fp)
{
    assert(node);

    if (node->type == OPER_CODE)
        fprintf(fp, "node%p [shape = record, color = blue, label=\"{type: operation | value: %d|{<f0> %p| <f1> %p}}\"];\n", node, node->value.op_code_t, node->left, node->right);
    else if (node->type == VAR_CODE)
        fprintf(fp, "node%p [shape = record, color = green, label=\"{type: variable | value: %d|{<f0> %p| <f1> %p}}\"];\n", node, node->value.var_ind, node->left, node->right);
    else if (node->type == BODY_CODE)
        fprintf(fp, "node%p [shape = record, color = yellow, label=\"{type: body | value: %d|{<f0> %p| <f1> %p}}\"];\n", node, node->value.var_ind, node->left, node->right);
    else if (node->type == TREE_ROOT_CODE)
        fprintf(fp, "node%p [shape = record, color = black, label=\"{type: tree_root | value: %d|{<f0> %p| <f1> %p}}\"];\n", node, node->value.var_ind, node->left, node->right);
    else
        fprintf(fp, "node%p [shape = record, color = red, label=\"{type: number | value: %lg|{<f0> %p| <f1> %p}}\"];\n", node, node->value.num_t, node->left, node->right);

    if (node->left != NULL)
    {
        Print_Node(node->left, fp);
        fprintf(fp, "node%p:<f0>->node%p;\n", node, node->left);
    }
    if (node->right != NULL)
    {
        Print_Node(node->right, fp);
        fprintf(fp, "node%p:<f1>->node%p;\n", node, node->right);
    }
}

void Print_Tree(Node_t* node)
{
    assert(node);
    FILE* fp = fopen(DUMP_TREE_FILE, "w");

    if (fp == NULL)
        fprintf(stderr, "ERROR IN OPENING DUMP FILE\n");

    fprintf(fp, "Digraph G{\n");
    Print_Node(node, fp);
    fprintf(fp, "}");

    fclose(fp);
}