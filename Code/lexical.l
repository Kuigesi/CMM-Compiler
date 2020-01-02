%{
  #include "stdio.h"
  #include <stdlib.h>
  #include <string.h>
  #include <stdarg.h>
  #include "syntax.tab.h"
  #include "tree.h"
  int chars = 0;
  int words = 0;
  int lines = 0;
  int yycolumn = 1;
  #define YY_USER_ACTION \
  yylloc.first_line = yylloc.last_line = yylineno; \
  yylloc.first_column = yycolumn; \
  yylloc.last_column = yycolumn + yyleng - 1; \
  yycolumn += yyleng;
%}
%option yylineno
digit [0-9]
nonzerodigit [1-9]
letter [_a-zA-Z]
ID {letter}({letter}|{digit})*
DECINTEGER ({nonzerodigit}{digit}*)|0
SPACETAB [ \t]
OCTINTEGER 0[0-7]+
HEXINTEGER 0(x|X)[0-9a-fA-F]+
INTEGER {OCTINTEGER}|{HEXINTEGER}|{DECINTEGER}
DIGITPART {digit}+
FRACTION "."{DIGITPART}
EXPONENT (e|E)(("+"{DIGITPART})|(-{DIGITPART})|{DIGITPART})
POINTFLOAT {FRACTION}|({DIGITPART}{FRACTION})|({DIGITPART}".")
EXPONENTFLOAT ({POINTFLOAT}){EXPONENT}
FLOAT {POINTFLOAT}|{EXPONENTFLOAT}
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS -
STAR \*
DIV "/"
AND &&
OR \|\|
DOT "."
NOT !
TYPE (int)|(float)
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT "struct"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
%%
{SPACETAB}
{INTEGER} {//printf("LINE %d: INTEGER, %s\n",yylineno,yytext);
	  yylval.type_node = new_node("INT",0,yylineno,yytext,-1);
	  return INT;}
{FLOAT} {//printf("LINE %d: FLOAT, %s\n",yylineno,yytext);
	yylval.type_node = new_node("FLOAT",0,yylineno,yytext,-1);
	return FLOAT;}
{SEMI} {//printf("LINE %d: SEMI, %s\n",yylineno,yytext);
       yylval.type_node = new_node("SEMI",0,yylineno,yytext,-1);
       return SEMI;}
{COMMA} {//printf("LINE %d: COMMA, %s\n",yylineno,yytext);
	yylval.type_node = new_node("COMMA",0,yylineno,yytext,-1);
	return COMMA;}
{ASSIGNOP} {//printf("LINE %d: ASSIGNOP, %s\n",yylineno,yytext);
	   yylval.type_node = new_node("ASSIGNOP",0,yylineno,yytext,-1);
	   return ASSIGNOP;}
{RELOP} {//printf("LINE %d: RELOP, %s\n",yylineno,yytext);
	   yylval.type_node = new_node("RELOP",0,yylineno,yytext,-1);
	   return RELOP;}
{PLUS} {//printf("LINE %d: PLUS, %s\n",yylineno,yytext);
          yylval.type_node = new_node("PLUS",0,yylineno,yytext,-1);
          return PLUS;}
{MINUS} {//printf("LINE %d: MINUS, %s\n",yylineno,yytext);
	  yylval.type_node = new_node("MINUS",0,yylineno,yytext,-1);
	  return MINUS;}
{STAR} {//printf("LINE %d: STAR, %s\n",yylineno,yytext);
          yylval.type_node = new_node("STAR",0,yylineno,yytext,-1);
          return STAR;}
{DIV} {//printf("LINE %d: DIV, %s\n",yylineno,yytext);
      yylval.type_node = new_node("DIV",0,yylineno,yytext,-1);
      return DIV;}
{AND} {//printf("LINE %d: AND, %s\n",yylineno,yytext);
      yylval.type_node = new_node("AND",0,yylineno,yytext,-1);
      return AND;}
{OR} {//printf("LINE %d: OR, %s\n",yylineno,yytext);
     yylval.type_node = new_node("OR",0,yylineno,yytext,-1);
     return OR;}
{DOT} {//printf("LINE %d: DOT, %s\n",yylineno,yytext);
      yylval.type_node = new_node("DOT",0,yylineno,yytext,-1);
      return DOT;}
{NOT} {//printf("LINE %d: NOT, %s\n",yylineno,yytext);
      yylval.type_node = new_node("NOT",0,yylineno,yytext,-1);
      return NOT;}
{TYPE} {//printf("LINE %d: TYPE, %s\n",yylineno,yytext);
       yylval.type_node = new_node("TYPE",0,yylineno,yytext,-1);
       //printf("0x%x\n",yylval.type_node);
       return TYPE;}
{LP} {//printf("LINE %d: LP, %s\n",yylineno,yytext);
     yylval.type_node = new_node("LP",0,yylineno,yytext,-1);
     return LP;}
{RP} {//printf("LINE %d: RP, %s\n",yylineno,yytext);
     yylval.type_node = new_node("RP",0,yylineno,yytext,-1);
     return RP;}
{LB} {//printf("LINE %d: LB, %s\n",yylineno,yytext);
     yylval.type_node = new_node("LB",0,yylineno,yytext,-1);
     return LB;}
{RB} {//printf("LINE %d: RB, %s\n",yylineno,yytext);
     yylval.type_node = new_node("RB",0,yylineno,yytext,-1);
     return RB;}
{LC} {//printf("LINE %d: LC, %s\n",yylineno,yytext);
     yylval.type_node = new_node("LC",0,yylineno,yytext,-1);
     return LC;}
{RC} {//printf("LINE %d: RC, %s\n",yylineno,yytext);
     yylval.type_node = new_node("RC",0,yylineno,yytext,-1);
     return RC;}
{STRUCT} {//printf("LINE %d: STRUCT, %s\n",yylineno,yytext);
	 yylval.type_node = new_node("STRUCT",0,yylineno,yytext,-1);
	 return STRUCT;}
{RETURN} {//printf("LINE %d: RETURN, %s\n",yylineno,yytext);
	 yylval.type_node = new_node("RETURN",0,yylineno,yytext,-1);
	 return RETURN;}
{IF} {//printf("LINE %d: IF, %s\n",yylineno,yytext);
     yylval.type_node = new_node("IF",0,yylineno,yytext,-1);
     return IF;}
{ELSE} {//printf("LINE %d: ELSE, %s\n",yylineno,yytext);
       yylval.type_node = new_node("ELSE",0,yylineno,yytext,-1);
       return ELSE;}
{WHILE} {//printf("LINE %d: WHILE, %s\n",yylineno,yytext);
	yylval.type_node = new_node("WHILE",0,yylineno,yytext,-1);
	return WHILE;}
{ID} {//printf("LINE %d: ID, %s\n",yylineno,yytext);
	if(strlen(yytext)<33)
            yylval.type_node = new_node("ID",0,yylineno,yytext,-1);
	else {
		error_flag = 1;
		printf("Error type A at line %d: ID too long\n",yylineno);
		yylval.type_node = new_node("ID",0,yylineno,"LONG_ID",-1);
	}
     return ID;}
\n { yycolumn = 1;}
"//" {
	//printf("type 1 comment start at line %d\n",yylineno);
	char c = input();
	while(c != '\n') c = input();
     }
"/*" {
	char c,ct;
	//printf("type 2 comment start at line %d\n",yylineno);
	while(1) {
		c = input();
		if(c == '*'){
			ct = input();
			if(ct == '/'){
				//printf("type 2 comment end at line %d\n",yylineno);
				break;
			} else {
				unput(ct);
			}
		}
		if(c == EOF){
			error_flag = 1;
			printf("Error type A at line %d: Comment has no end\n",yylineno);break;
		}
	}
     }
. {error_flag = 1;printf("Error type A at line %d: Mysterious character \'%s\'\n",yylineno,yytext);}
%%
/*void yyerror(char *s, ...){
	extern int yylineno;
	va_list ap;
	va_start(ap, s);
	fprintf(stderr, "%d: error: ", yylineno);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
}
*/
