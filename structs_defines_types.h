#ifndef STRUCTS
#define STRUCTS

#define READ_TREE_FILE "Math_problem.txt"
#define NUM_OPER 24

#define IF_ERROR(arg)                                                                                   \
do {                                                                                                    \
    if (arg == NULL)                                                                                    \
    {                                                                                                   \
        fprintf(stderr, "ERROR in file: %s, function: %s, line: %d\n", __FILE__, __func__, __LINE__);  \
        return NULL;                                                                                    \
    }                                                                                                   \
} while (0)

struct hash {
    char name[10];
    int hash_len;
    int index;
    double var_value;
};

enum type_codes{
    OPER_CODE = 1,
    VAR_CODE = 2,
    NUM_CODE = 3,
    BODY_CODE = 4,
    TREE_ROOT_CODE = 2007,
};

enum oper_codes{
    END_CODE = 666,
    ADD_CODE = 1,
    SUB_CODE = 2,
    MUL_CODE = 3,
    DIV_CODE = 4,
    STEPEN_CODE = 5,

    SIN_CODE = 6,
    COS_CODE = 7,
    TAN_CODE = 8,
    COTAN_CODE = 9,
    ARCSIN_CODE = 10,
    ARCCOS_CODE = 11,
    ARCTAN_CODE = 12,
    ARCCOTAN_CODE = 13,
    LN_CODE = 14,
    OPEN_BRAC_CODE = 15,
    CLOSED_BRAC_CODE = 16,
    OPEN_FIG_BRAC_CODE = 17,
    CLOSE_FIG_BRAC_CODE = 18,
    SEMICOLONE_CODE = 19,

    DOUBLE_EQ_CODE = 20,
    IF_CODE = 21,
    EQUA_CODE = 22,
    VAR_INIT_CODE = 23,
    CHANGE_VAR_CODE = 24,
};

struct oper_t{
    const char* op_symb;
    enum oper_codes op_code;
    int len;
};

union value_dif
{
    enum oper_codes op_code_t;
    int var_ind;
    double num_t;
};

struct Node_t {
    Node_t* left;
    Node_t* right;
    Node_t* parent;
    type_codes type;
    value_dif value;
};

struct tree_t {
    int num_var;
    Node_t* root_node;
    hash* hash_table;
};

enum errors{
    NO_ERRORS = 0,
    NODE_NULL = 1,
    TOO_MANY_ARGS = 2,
    FORBIDDEN_NULL = 3,
};

#endif