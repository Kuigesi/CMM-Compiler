typedef struct Type_ Type;
typedef struct Fieldlist_ Fieldlist;
typedef struct intercode_ Intercode;
typedef struct oprand_ Oprand;
typedef struct Array_ {
    Type * type;
    int size;
} Array;
struct tree_node {
	struct tree_node * child_node;
	struct tree_node * bro_node;
	char name[80];
	char idname[80];
	int line;
	int type;
	int val;
    Type * base_type;
    Type * var_type;
    Fieldlist * field;
    int gno;
	double dval;
    Type * func_return_type;
    int semantic_error_flag;
    int left_flag;
    Intercode * code;
    Oprand * store_place;
};
struct tree_node * new_node(char * type_name,int type,int line,char * content,int geneno);
void add_node (int num,...);
void pstree (struct tree_node * p,int indent);
int error_flag;
void yyerror(char *);

//******************semantic*****************//

void semantic_check (struct tree_node * p);
unsigned int hash(char * name);
struct Fieldlist_ {
    char * name;
    Type * type;
    Fieldlist * next;
};
typedef struct Structure_ {
    char * structname;
    Fieldlist * field;
} Structure;
typedef struct Function_ {
    char * funcname;
    Type * returntype;
    Fieldlist * varlist;
} Function;
struct Type_ {
    enum {basic,array,structure,function} kind;
    union {
        int basic;
        Array array;
        Structure structure;
        Function function;
    };
};
typedef struct ID_table_ebtry_ ID_table_ebtry;
struct ID_table_ebtry_ {
    Type * type;
    char * name;
    ID_table_ebtry * next;
};
typedef struct ID_table_ {
	ID_table_ebtry * entry[16384];
} ID_table;
ID_table * Global_id_table;
ID_table * Global_struct_table;
int in_struct_flag;
int in_logic_flag;

struct tree_node * get_child_node (struct tree_node * p,int i);
char * malloc_copy(char * str);

//******************intercode*****************//

typedef struct Variable_table_entry_ Variable_table_entry;
struct Variable_table_entry_ {
    char * id;
    Oprand * var;
    Variable_table_entry * next;
};
typedef struct Variable_table_ {
	Variable_table_entry * entry[16384];
} Variable_table;

struct tree_node * root_tree_node;
int global_variable_no;
int global_temp_no;
int global_label_no;
Variable_table * Global_variable_table;
void translate_Program (struct tree_node * p,Variable_table * table);
struct oprand_ {
    enum { OP_VARIABLE, OP_CONSTANT,OP_LABEL,OP_FUNCTION,OP_ARRAY,OP_STRUCTURE,OP_TEMP} kind;
    union {
        int var_no;
        int temp_no;
        int value;
        int label_no;
        char * name;
    } u;
    int get_addr;
    int get_val;
    int in_dec;
};
#ifndef GLOBAL_NUM_DEFINE
#define GLOBAL_NUM_DEFINE
#define CODE_LABEL 0
#define CODE_FUNCTION 1
#define CODE_ASSIGN 2
#define CODE_BINOP 3
#define CODE_JMP 4
#define CODE_CONDJMP 5
#define CODE_RETURN 6
#define CODE_MEMDEC 7
#define CODE_ARG 8
#define CODE_CALL 9
#define CODE_PARAM 10
#define CODE_READ 11
#define CODE_WRITE 12
#define BOP_ADD 0
#define BOP_SUB 1
#define BOP_MUL 2
#define BOP_DIV 3
#define RLP_G 0
#define RLP_L 1
#define RLP_EQ 2
#define RLP_GEQ 3
#define RLP_LEQ 4
#define RLP_NEQ 5
#endif
struct intercode_ {
    int kind;
    union {
        struct {Oprand * l;} label;
        struct {Oprand * f;} function_def;
        struct {Oprand * result, *op1 ;} assign;
        struct {Oprand * result,* op1, * op2 ;int bop;} binop;
        struct {Oprand * des ;} jmp;
        struct {Oprand * des ,* x,* y; int relop;} condjmp;
        struct {Oprand * x;} return_val;
        struct {Oprand * x,* size;} memdec;
        struct {Oprand * x;} arg;
        struct {Oprand * x, * f;} function_call;
        struct {Oprand * x;} param;
        struct {Oprand * x;} read;
        struct {Oprand * x;} write;
    } u;
    Intercode * prev;
    Intercode * next;
};
FILE * Code_output_file;
int print_dept;
void print_oprand(FILE * stream,Oprand * op);
//***************************** mips *******************************//

typedef struct op_offset_map_ op_offset_map;
struct op_offset_map_
{
    Oprand * op;
    int offset;
    op_offset_map * next;
};
op_offset_map * Function_op_offset_table;
void asm_code_generate(FILE *stream,Intercode * code);
FILE * Machine_code_output_file;
Intercode * Global_intercode;
