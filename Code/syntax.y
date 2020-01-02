%{
   #include "lex.yy.c"
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <stdarg.h>
%}

%locations
%union {
   struct tree_node * type_node;
}
%token <type_node> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE LOWER_THAN_ELSE
%type <type_node> Program ExtDefList ExtDef Specifier ExtDecList FunDec CompSt VarDec StructSpecifier OptTag DefList Tag VarList ParamDec StmtList Stmt Exp Def DecList Dec Args

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left MINUS PLUS
%left STAR DIV
%right NOT NEGT
%left LP RP LB RB DOT
%start Program
%%
Program : ExtDefList {//printf("Program : ExtDefList \n");
	               $$ = new_node("Program",1,@$.first_line,NULL,0);
                       add_node(2,$$,$1);
                       //printf("print tree!!!\n");
                       //if(error_flag == 0)
                         //semantic_check($$);
                       root_tree_node = $$;
                     }
	;
ExtDefList : {//printf("ExtDefList : NULL \n");
	       $$ = new_node("ExtDefList",2,@$.first_line,NULL,1);
             }
	   | ExtDef ExtDefList {//printf("ExtDefList : ExDef ExDefList\n");
                                 $$ = new_node("ExtDefList",1,@$.first_line,NULL,0);
                                 add_node(3,$$,$1,$2);
                               }
           ;
ExtDef : Specifier ExtDecList SEMI {//printf("ExtDef : Specifier ExtDecList SEMI \n");
                                     $$ = new_node("ExtDef",1,@$.first_line,NULL,0);
                                     add_node(4,$$,$1,$2,$3);}
       | Specifier SEMI {//printf("ExtDef : Specifier SEMI \n");
                        $$ = new_node("ExtDef",1,@$.first_line,NULL,1);
                        add_node(3,$$,$1,$2);}
       | Specifier FunDec CompSt {//printf("ExtDef : Specifier FunDec CompSt\n");
                                 $$ = new_node("ExtDef",1,@$.first_line,NULL,2);
                                 add_node(4,$$,$1,$2,$3);}
       ;
ExtDecList : VarDec {//printf("ExtDecList : VarDec \n");
	            $$ = new_node("ExtDecList",1,@$.first_line,NULL,0);
                    add_node(2,$$,$1);}
	   | VarDec COMMA ExtDecList {//printf("ExtDecList : VarDec COMMA ExtDecList");
                                     $$ = new_node("ExtDecList",1,@$.first_line,NULL,1);
                                     add_node(4,$$,$1,$2,$3);}
           ;
Specifier : TYPE {//printf("Specifier : TYPE \n");
	         $$ = new_node("Specifier",1,@$.first_line,NULL,0);
                 //printf("0x%x  0x%x\n",$$,$1);
                 add_node(2,$$,$1);}
	  | StructSpecifier {//printf("Specifier StructSpecifier \n");
                            $$ = new_node("Specifier",1,@$.first_line,NULL,1);
                            add_node(2,$$,$1);}
          ;
StructSpecifier : STRUCT OptTag LC DefList RC {//printf("StructSpecifier : STRUCT OptTag LC DefList RC\n");
		                              $$ = new_node("StructSpecifier",1,@$.first_line,NULL,0);
                                              add_node(6,$$,$1,$2,$3,$4,$5);}
		| STRUCT Tag {//printf("StructSpecifier | STRUCT Tag");
                             $$ = new_node("StructSpecifier",1,@$.first_line,NULL,1);
                             add_node(3,$$,$1,$2);}
                ;
OptTag : {//printf("OptTag : NULL\n");
         $$ = new_node("OptTag",2,@$.first_line,NULL,1);}
       | ID {//printf("OptTag | ID\n");
            $$ = new_node("OptTag",1,@$.first_line,NULL,0);
            add_node(2,$$,$1);}
       ;
Tag : ID {//printf("TAG : ID\n");
         $$ = new_node("Tag",1,@$.first_line,NULL,0);
         add_node(2,$$,$1);}
    ;
VarDec : ID {//printf("VarDec : ID\n");
            $$ = new_node("VarDec",1,@$.first_line,NULL,0);
            add_node(2,$$,$1);}
       | VarDec LB INT RB {//printf("TAG | VarDec LB INT RB\n");
                          $$ = new_node("VarDec",1,@$.first_line,NULL,1);
                          add_node(5,$$,$1,$2,$3,$4);}
       ;
FunDec : ID LP VarList RP {//printf("FunDec : ID LP VarList RP \n");
                          $$ = new_node("FunDec",1,@$.first_line,NULL,0);
                          add_node(5,$$,$1,$2,$3,$4);}
       | ID LP RP {//printf("FunDec | ID LP RP\n");
                  $$ = new_node("FunDec",1,@$.first_line,NULL,1);
                  add_node(4,$$,$1,$2,$3);}
       ;
VarList : ParamDec COMMA VarList {//printf("VarList : ParaDec COMMA VarList \n");
	                         $$ = new_node("VarList",1,@$.first_line,NULL,0);
                                 add_node(4,$$,$1,$2,$3);}
	| ParamDec {//printf("VarList | ParamDec \n");
                   $$ = new_node("VarList",1,@$.first_line,NULL,1);
                   add_node(2,$$,$1);}
        ;
ParamDec : Specifier VarDec {//printf("ParamDec : Specifier \n");
	                    $$ = new_node("ParamDec",1,@$.first_line,NULL,0);
                            add_node(3,$$,$1,$2);}
	 ;
CompSt : LC DefList StmtList RC {//printf("CompSt : LC DefList StmtList RC \n");
                                $$ = new_node("CompSt",1,@$.first_line,NULL,0);
                                add_node(5,$$,$1,$2,$3,$4);}
       | error RC {//printf("CompSt | error RC\n");
                   $$ = new_node("CompSt",1,@$.first_line,NULL,1);}
       ;
StmtList : {//printf("StmtList : NULL\n");
	   $$ = new_node("StmtList",2,@$.first_line,NULL,1);}
	 | Stmt StmtList {//printf("StmtList | Stmt StmtList\n");
                         $$ = new_node("StmtList",1,@$.first_line,NULL,0);
                         add_node(3,$$,$1,$2);}
         ;
Stmt : Exp SEMI {//printf("Stmt : Exp SEMI\n");
                $$ = new_node("Stmt",1,@$.first_line,NULL,0);
                add_node(3,$$,$1,$2);}
     | error SEMI {//printf("Stmt : error SEMI\n");
                    //yyerror();
                    $$ = new_node("stmt",1,@$.first_line,NULL,6); 
                  }
     | CompSt {//printf("Stmt | CompSt\n");
              $$ = new_node("Stmt",1,@$.first_line,NULL,1);
              add_node(2,$$,$1);}
     | RETURN Exp SEMI {//printf("Stmt | RETURN Exp SEMI\n");
                       $$ = new_node("Stmt",1,@$.first_line,NULL,2);
                       add_node(4,$$,$1,$2,$3);}
     | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {//printf("Stmt | IF LP Exp RP Stmt\n");
                         $$ = new_node("Stmt",1,@$.first_line,NULL,3);
                         add_node(6,$$,$1,$2,$3,$4,$5);}
     | IF LP Exp RP Stmt ELSE Stmt {//printf("Stmt | IF LP Exp RP Stmt ELSE Stmt\n");
                                   $$ = new_node("Stmt",1,@$.first_line,NULL,4);
                                   add_node(8,$$,$1,$2,$3,$4,$5,$6,$7);}
     | WHILE LP Exp RP Stmt {//printf("Stmt | WHILE LP Exp RP Stmt\n");
                            $$ = new_node("Stmt",1,@$.first_line,NULL,5);
                            add_node(6,$$,$1,$2,$3,$4,$5);}
     ;
DefList : {//printf("DefList : NULL\n");
	  $$ = new_node("DefList",2,@$.first_line,NULL,1);}
	| Def DefList {//printf("DefList | Def DefList\n");
                      $$ = new_node("DefList",1,@$.first_line,NULL,0);
                      add_node(3,$$,$1,$2);}
        ;
Def : Specifier DecList SEMI {//printf("Def : Specifier DecList SEMI\n");
                             $$ = new_node("Def",1,@$.first_line,NULL,0);
                             add_node(4,$$,$1,$2,$3);}
    | Specifier error SEMI {$$ = new_node("stmt",1,@$.first_line,NULL,1);}
    ;
DecList : Dec {//printf("DecList : Dec\n");
	      $$ = new_node("DecList",1,@$.first_line,NULL,0);
              add_node(2,$$,$1);}
	| Dec COMMA DecList {//printf("DecList | Dec COMMA DecList\n");
                            $$ = new_node("DecList",1,@$.first_line,NULL,1);
                            add_node(4,$$,$1,$2,$3);}
        ;
Dec : VarDec {//printf("Dec : VarDec\n");
             $$ = new_node("Dec",1,@$.first_line,NULL,0);
             add_node(2,$$,$1);}
    | VarDec ASSIGNOP Exp {//printf("Dec | VarDec ASSIGNOP Exp\n");
                          $$ = new_node("Dec",1,@$.first_line,NULL,1);
                          add_node(4,$$,$1,$2,$3);}
    ;
Exp : Exp ASSIGNOP Exp {//printf("Exp : Exp ASSIGNOP Exp\n");
                       $$ = new_node("Exp",1,@$.first_line,NULL,0);
                       add_node(4,$$,$1,$2,$3);}
    | Exp AND Exp {//printf("Exp | Exp AND Exp\n");
                  $$ = new_node("Exp",1,@$.first_line,NULL,1);
                  add_node(4,$$,$1,$2,$3);}
    | Exp OR Exp {//printf("Exp | Exp OR Exp\n");
                 $$ = new_node("Exp",1,@$.first_line,NULL,2);
                 add_node(4,$$,$1,$2,$3);}
    | Exp RELOP Exp {//printf("Exp | Exp RELOP Exp\n");
                    $$ = new_node("Exp",1,@$.first_line,NULL,3);
                    add_node(4,$$,$1,$2,$3);}
    | Exp PLUS Exp {//printf("Exp | Exp PLUS Exp\n");
                   $$ = new_node("Exp",1,@$.first_line,NULL,4);
                   add_node(4,$$,$1,$2,$3);}
    | Exp MINUS Exp {//printf("Exp | Exp MINUS Exp\n");
                    $$ = new_node("Exp",1,@$.first_line,NULL,5);
                    add_node(4,$$,$1,$2,$3);}
    | Exp STAR Exp {//printf("Exp | Exp STAR Exp\n");
                   $$ = new_node("Exp",1,@$.first_line,NULL,6);
                   add_node(4,$$,$1,$2,$3);}
    | Exp DIV Exp {//printf("Exp | Exp DIV Exp\n");
                  $$ = new_node("Exp",1,@$.first_line,NULL,7);
                  add_node(4,$$,$1,$2,$3);}
    | LP Exp RP {//printf("Exp | Exp AND Exp\n");
                $$ = new_node("Exp",1,@$.first_line,NULL,8);
                add_node(4,$$,$1,$2,$3);}
    | MINUS Exp %prec NEGT {//printf("Exp | MINUS Exp\n");
                $$ = new_node("Exp",1,@$.first_line,NULL,9);
                add_node(3,$$,$1,$2);}
    | NOT Exp {//printf("Exp | NOT Exp\n");
              $$ = new_node("Exp",1,@$.first_line,NULL,10);
              add_node(3,$$,$1,$2);}
    | ID LP Args RP {//printf("Exp | ID LP Args RP\n");
                    $$ = new_node("Exp",1,@$.first_line,NULL,11);
                    add_node(5,$$,$1,$2,$3,$4);}
    | ID LP RP {//printf("Exp | ID LP RP\n");
               $$ = new_node("Exp",1,@$.first_line,NULL,12);
               add_node(4,$$,$1,$2,$3);}
    | Exp LB Exp RB {//printf("Exp | Exp LB Exp RB\n");
                    $$ = new_node("Exp",1,@$.first_line,NULL,13);
                    add_node(5,$$,$1,$2,$3,$4);}
    | Exp DOT ID {//printf("Exp | Exp DOT ID\n");
                 $$ = new_node("Exp",1,@$.first_line,NULL,14);
                 add_node(4,$$,$1,$2,$3);}
    | ID {//printf("Exp | ID\n");
         $$ = new_node("Exp",1,@$.first_line,NULL,15);
         add_node(2,$$,$1);}
    | INT {//printf("Exp | INT\n");
          $$ = new_node("Exp",1,@$.first_line,NULL,16);
          add_node(2,$$,$1);}
    | FLOAT {//printf("Exp | FLOAT\n");
            $$ = new_node("Exp",1,@$.first_line,NULL,17);
            add_node(2,$$,$1);}
    /*| error RP {printf("Exp | error RP Line: %d Poision: %d\n",@2.first_line,@2.first_column);
                $$ = new_node("Exp",1,@$.first_line,NULL);} */
    /*| error RB {printf("Exp | error RB Line: %d Poision: %d\n",@2.first_line,@2.first_column);
                  $$ = new_node("Exp",1,@$.first_line,NULL);} */
    /*| error SEMI {printf("Exp | error SEMMI Line: %d Poision: %d\n",@2.first_line,@2.first_column);
                   $$ = new_node("Exp",1,@$.first_line,NULL);} */
    ;
Args : Exp COMMA Args {//printf("Args : Exp COMMA Args\n");
                      $$ = new_node("Args",1,@$.first_line,NULL,0);
                      add_node(4,$$,$1,$2,$3);}
     | Exp {//printf("Args | Exp\n");
           $$ = new_node("Args",1,@$.first_line,NULL,1);
           add_node(2,$$,$1);}
     ;
%%
struct tree_node * new_node(char * type_name,int type,int line,char * content,int geneno){
  struct tree_node *a=(struct tree_node*)malloc(sizeof(struct tree_node));
  a->left_flag = 0;
  a->code = NULL;
  a->type = type;
  a->gno = geneno;
  a->store_place = NULL;
  strcpy(a->name,type_name);
  //printf("   Line : %d %s\n",line,a->name);
  if(type == 0) {
    if(!(strcmp(a->name,"ID"))) {
          strcpy(a->idname,content);
      }
    if(!(strcmp(a->name,"RELOP"))) {
          strcpy(a->idname,content);
      }
    if(!(strcmp(a->name,"TYPE"))) {
          strcpy(a->idname,content);
      }
    if(!(strcmp(a->name,"INT"))) {
         if((content[0]=='0')&&(strlen(content)>1)&&(content[1]!='x')&&(content[1]!='X')){
              sscanf(content,"0%o",&(a->val));
              //printf("Dec %d Oct 0%o 0x%x\n",a->val,a->val,a);
          } else {
         if((content[0]=='0')&&(strlen(content)>1)&&((content[1]=='x')||(content[1]=='X'))){
              sscanf(content,"0x%x",&(a->val));
              //printf("Dec %d Hex 0x%x \n",a->val,a->val);
          } else {
              sscanf(content,"%d",&(a->val));
          }
      }
      }
    if(!(strcmp(a->name,"FLOAT"))) {
        a->dval = atof(content);
        //printf("float : %lf \n",a->dval);
     }
  }
  a->line = line;
  //printf("   finished\n");
  return a;
}
void yyerror(char * s)
{

    extern int yylineno;
    extern char * yytext;
    error_flag = 1;
    printf("Error type B at line %d: syntax error\n", yylineno);
    //printf("finished\n");

}
