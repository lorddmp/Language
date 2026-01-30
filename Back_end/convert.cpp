#include "convert.h"

#define MATH_FUNCS 4 // функции которые есть и в языке, и в процессоре

struct str {
    oper_codes op_code;
    const char* cmd_code;
};

// str mas_codes[MATH_FUNCS] = {
//     {ADD_CODE, "ADD\n", },
//     {SUB_CODE, "SUB\n"},
//     {MUL_CODE, "MUL\n"},
//     {DIV_CODE, "DIV\n"},
// };

int Obrabotka_node(Node_t* node, FILE* fp);

void Converting(tree_t tree)
{
    FILE* fp = fopen("Back_end/Commands.txt", "w");

    Obrabotka_node(tree.root_node, fp);

    fprintf(fp, "HLT");

    fclose(fp);
}

int Obrabotka_node(Node_t* node, FILE* fp)
{
    if (node->type == BODY_CODE || node->type == TREE_ROOT_CODE)
        Obrabotka_node(node->right, fp);

    if (node->value.op_code_t == VAR_INIT_CODE || node->value.op_code_t == EQUA_CODE)
    {
        Obrabotka_node(node->right, fp);
        fprintf(fp, "POPR %dX\n", node->left->value.var_ind);
        return 0;
    }

    if (node->value.op_code_t == IF_CODE)
    {
        Obrabotka_node(node->left, fp);
        fprintf(fp, "PUSH 0\n");
        fprintf(fp, "JE :%d\n", node);
        Obrabotka_node(node->right, fp);
        fprintf(fp, ":%d\n", node);
    }

    if (node->value.op_code_t == WHILE_CODE)
    {
        fprintf(fp, ":%d\n", (int)node->left);
        Obrabotka_node(node->left, fp);
        fprintf(fp, "PUSH 0\n");
        fprintf(fp, "JE :%d\n", (int)node);
        Obrabotka_node(node->right, fp);
        fprintf(fp, "JMP :%d", (int)node->left);
        fprintf(fp, ":%d\n", (int)node);
    }

    if (node->left != NULL)
        Obrabotka_node(node->left, fp);
    if (node->right != NULL && (node->type != BODY_CODE && node->type != TREE_ROOT_CODE))
        Obrabotka_node(node->right, fp);

    if (node->type == NUM_CODE)
    {
        fprintf(fp, "PUSH %lg\n", node->value.num_t);
        return 0;
    }

    if (node->type == VAR_CODE)
    {
        fprintf(fp, "PUSH %dX\n", node->value.var_ind);
        return 0;
    }


    if (node->value.op_code_t == ADD_CODE)          // Сорри что так убого, МАЛО ВРЕМЕНИ, потом исправлю
        fprintf(fp, "ADD\n");
    else if (node->value.op_code_t == SUB_CODE)
        fprintf(fp, "SUB\n");
    else if (node->value.op_code_t == MUL_CODE)
        fprintf(fp, "MUL\n");
    else if (node->value.op_code_t == DIV_CODE)
        fprintf(fp, "DIV\n");
    
    return 0;
}