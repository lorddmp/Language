#include "convert.h"

#define MATH_FUNCS 14
#define LOG_FUNCS 6

struct str {
    oper_codes op_code;
    const char* cmd_name;
};

str math_codes[MATH_FUNCS] = {
    {ADD_CODE,      "ADD"},
    {SUB_CODE,      "SUB"},
    {MUL_CODE,      "MUL"},
    {DIV_CODE,      "DIV"},
    {POW_CODE,      "POW"},
    {SIN_CODE,      "SIN"},
    {COS_CODE,      "COS"},
    {TAN_CODE,      "TAN"},
    {COTAN_CODE,    "COTAN"},
    {ARCSIN_CODE,   "ARCSIN"},
    {ARCCOS_CODE,   "ARCCOS"},
    {ARCTAN_CODE,   "ARCTAN"},
    {ARCCOTAN_CODE, "ARCCOTAN"},
    {LN_CODE,       "LN"},
};

str log_codes[LOG_FUNCS] = {
    {DOUBLE_EQ_CODE,    "JE"},
    {NOT_EQ_CODE,       "JNE"},
    {MORE_CODE,         "JB"},
    {MORE_OR_EQ_CODE,   "JBE"},
    {LESS_CODE,         "JA"},
    {LESS_OR_EQ_CODE,   "JAE"},
};

int Obrabotka_node(Node_t* node, FILE* fp);

void Converting(Node_t* root_node)
{
    FILE* fp = fopen("Back_end_my_asm/Commands.txt", "w"); 

    Obrabotka_node(root_node, fp);

    fprintf(fp, "HLT");

    fclose(fp);
}

int Obrabotka_node(Node_t* node, FILE* fp)
{
    // printf("NODE: type = %d, left = %p, right = %p\n", node->type, node->left, node->right);
    if (node->type == BODY_CODE || node->type == TREE_ROOT_CODE)
        Obrabotka_node(node->right, fp);

    switch (node->value.op_code_t)
    {
        case FUNC_INIT_CODE:
            fprintf(fp, "JMP :%p\n", node);
            fprintf(fp, ":%d\n", node->left->value.name_ind);
            Obrabotka_node(node->right, fp);
            fprintf(fp, "RET\n");
            fprintf(fp, ":%p\n", node);
            return 0;
        
        case VAR_INIT_CODE:
        case EQUA_CODE:
            Obrabotka_node(node->right, fp);
            fprintf(fp, "POP REG%dX\n", node->left->value.name_ind);
            return 0;

        case FUNC_CALL_CODE:
            fprintf(fp, "CALL :%d\n", node->left->value.name_ind);
            return 0;

        case IF_CODE:
            Obrabotka_node(node->left, fp);
            fprintf(fp, "PUSH 0\n");
            fprintf(fp, "JE :%p\n", node);
            Obrabotka_node(node->right, fp);
            fprintf(fp, ":%p\n", node);
            return 0;

        case WHILE_CODE:
            fprintf(fp, ":%p\n", node->left);
            Obrabotka_node(node->left, fp);
            fprintf(fp, "PUSH 0\n");
            fprintf(fp, "JE :%p\n", node);
            Obrabotka_node(node->right, fp);
            fprintf(fp, "JMP :%p\n", node->left);
            fprintf(fp, ":%p\n", node);
            return 0;
        
        case PRINTF_CODE:
            Obrabotka_node(node->left, fp);
            fprintf(fp, "POP\n");
            return 0;

        default:
            break;
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

    if (node->type == NAME_CODE)
    {
        fprintf(fp, "PUSH REG%dX\n", node->value.name_ind);
        return 0;
    }

    for (int i = 0; i < MATH_FUNCS; i++)
    {
        if (node->value.op_code_t == math_codes[i].op_code)
            fprintf(fp, "%s\n", math_codes[i].cmd_name);
    }

    for (int i = 0; i < LOG_FUNCS; i++)
    {
        if (node->value.op_code_t == log_codes[i].op_code)
        {
            fprintf(fp, "%s :%p\n", log_codes[i].cmd_name, node->left);
            fprintf(fp, "PUSH 0\n");
            fprintf(fp, "JMP :%p\n", node->right);
            fprintf(fp, ":%p\n", node->left);
            fprintf(fp, "PUSH 1\n");
            fprintf(fp, ":%p\n", node->right);
        }
    }
    
    return 0;
}