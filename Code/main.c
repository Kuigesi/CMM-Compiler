#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "tree.h"
int yyparse();
int main(int argc, char** argv) {
	extern FILE *yyin;
    extern int yylineno;
	extern int yydebug;
	extern int error_flag;
	error_flag = 0;
	in_logic_flag = 0;
	in_struct_flag = 0;
	yylineno = 1;
	//yydebug = 1;
	if (argc > 1) {
		if (!(yyin = fopen(argv[1], "r"))) {
			perror(argv[1]);
			return 1;
		}
	}
	Global_id_table = (ID_table *)malloc(sizeof(ID_table));
	global_variable_no = 0;
	global_temp_no = 0;
	global_label_no = 0;
	Global_variable_table = (Variable_table *)malloc(sizeof(Variable_table));
	yyparse();
	Code_output_file = fopen("./intercode.ir", "w+");
	Machine_code_output_file = fopen(argv[2], "w+");
	translate_Program(root_tree_node,Global_variable_table);
	asm_code_generate(Machine_code_output_file,Global_intercode);
	return 0;
}
