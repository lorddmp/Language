#include "convert.h"

#include <stdlib.h>

void Set_Scope_Array(Node_t* node, char* scope_var_array, bool* inside_func);

void Set_Data(char* scope_var_array, int num_name, FILE* fp);

void Converting(Node_t* root_node, int num_name)
{
    FILE* fp = fopen("Back_end_nasm/Commands.asm", "w");
    bool inside_func = false;
    char* scope_var_array = (char*)calloc((size_t)num_name, sizeof(char));

    Set_Scope_Array(root_node, scope_var_array, &inside_func);

    Set_Data(scope_var_array, num_name, fp);

    // for (int i = 0; i < num_name; i++)
    //     putchar(scope_var_array[i]);

    // putchar('\n');

    free(scope_var_array);
    fclose(fp);
}

void Set_Scope_Array(Node_t* node, char* scope_var_array, bool* inside_func)
{
    if (node->value.op_code_t == FUNC_INIT_CODE)
        *inside_func = true;

    else if (node->type == NAME_CODE && node->parent->value.op_code_t != FUNC_INIT_CODE && node->parent->value.op_code_t != FUNC_CALL_CODE)
    {
        if (*inside_func == true)
            scope_var_array[node->value.name_ind] = 'l'; //local
        else
            scope_var_array[node->value.name_ind] = 'g'; //global

    }

    if (node->left != NULL)
        Set_Scope_Array(node->left, scope_var_array, inside_func);

    if (node->right != NULL)
        Set_Scope_Array(node->right, scope_var_array, inside_func);

    if (node->value.op_code_t == FUNC_INIT_CODE)
        *inside_func = false;
}

void Set_Data(char* scope_var_array, int num_name, FILE* fp)
{
    fprintf(fp, "section .data\n\n");

    for (int pos_scope_var_array = 0; pos_scope_var_array < num_name; pos_scope_var_array++)
        if (scope_var_array[pos_scope_var_array] == 'g')
            fprintf(fp, "var%d dq 0\n", pos_scope_var_array);
}