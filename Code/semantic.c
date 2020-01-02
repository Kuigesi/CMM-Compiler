#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include"tree.h" 
#include<assert.h>
int type_equal(Type* type1,Type* type2);
void exp_error_handler(struct tree_node * p,int task);
unsigned int hash(char* name){
    unsigned int val = 0, i;
    for (; *name; ++name){
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) val = (val ^ (i >> 12)) & 0x3fff;
        }
    return val;
}
struct tree_node * get_child_node (struct tree_node * p,int i){
    i--;
    struct tree_node * r;
    r = p->child_node;
    while(i>0){
        r = r->bro_node;
        i--;
    }
    return r;
}
Type * new_type(){
    return (Type *)malloc(sizeof(Type));
}
Fieldlist * new_field(){
    return (Fieldlist *)malloc(sizeof(Fieldlist));
}
void field_connect(Fieldlist * f1,Fieldlist * f2){
    if(f1==NULL)
        assert(0);
    Fieldlist * p1, * p2;
    p1 = f1;
    p2 = f1->next;
    while(p2){
        p1 = p2;
        p2 = p1->next;
    }
    p1->next = f2;
    return;
}
char * malloc_copy(char * str){
    char * r = (char *)malloc(strlen(str) + 1);
    strcpy(r,str);
    return r;
}
int type_array_equal(Array *array1,Array * array2){
    if((array1==NULL)||(array2==NULL)){
        assert(0);
    }
    if (array1->size == array2->size) {
        return type_equal(array1->type,array2->type);
    } else {
        return 0;
    }
}
int field_equal(Fieldlist * fieldlist1,Fieldlist * fieldlist2){
    if((fieldlist1 == NULL)&&(fieldlist2 == NULL)){
        return 1;
    }else {
        if((fieldlist1 == NULL)||(fieldlist2 == NULL))
            return 0;
    }
    if(type_equal(fieldlist1->type,fieldlist2->type) != 1){
        return 0;
    }else {
        if((fieldlist1->next == NULL)&&(fieldlist2->next == NULL)){
            return 1;
        }else {
            if((fieldlist1->next == NULL)||(fieldlist2->next == NULL)){
                return 0;
            } else {
                return field_equal(fieldlist1->next,fieldlist2->next);
            }
        }
    }     
}
Type * symbol_in_field(char * name,Fieldlist * f){
    if(f == NULL)
        return NULL;
    while (f!=NULL) {
        if(strcmp(f->name,name)==0)
            return f->type;
        f = f->next;
    }
    return NULL;
}
int type_fieldlist_equal(Fieldlist * fieldlist1,Fieldlist * fieldlist2){
    if((fieldlist1 == NULL)&&(fieldlist2 == NULL)){
        return 1;
    }else {
        if((fieldlist1 == NULL)||(fieldlist2 == NULL))
            return 0;
    }
    if(strcmp(fieldlist1->name,fieldlist2->name)!=0){
        return 0;
    }else{
        if(type_equal(fieldlist1->type,fieldlist2->type) != 1){
            return 0;
        }else {
            if((fieldlist1->next == NULL)&&(fieldlist2->next == NULL)){
                return 1;
            }else {
                if((fieldlist1->next == NULL)||(fieldlist2->next == NULL)){
                    return 0;
                } else {
                    return type_fieldlist_equal(fieldlist1->next,fieldlist2->next);
                }
            }
        }     
    }
}
int type_struct_equal(Structure * structure1,Structure * structure2){
    if((structure1==NULL)||(structure2==NULL)){
        assert(0);
    }
    if(strcmp(structure1->structname,structure2->structname) == 0){
        return type_fieldlist_equal(structure1->field,structure2->field);
    }else {
        return 0;
    }
}
int type_func_equal(Function * func1,Function * func2){
    if((func1==NULL)||(func2==NULL)){
        assert(0);
    }
    if(strcmp(func1->funcname,func2->funcname) == 0){
        if(type_equal(func1->returntype,func2->returntype) == 1){
            return type_fieldlist_equal(func1->varlist,func2->varlist);
        }else {
            return 0;
        }
    }else{
        return 0;
    }
}
int type_equal(Type* type1,Type* type2){
    //printf("%d  %d\n",type1->kind,type2->kind);
    if((type1==NULL)||(type2==NULL)){
        return 0;
    }
    if(type1->kind == type2->kind){
        switch (type1->kind){
            case basic:
                if(type1->basic == type2->basic){
                    return 1;
                }else{
                    return 0;
                }
                break;
            case array:
                return type_array_equal(&(type1->array),&(type2->array));
                break;
            case structure:
                return type_struct_equal(&(type1->structure),&(type2->structure));
                break;
            case function:
                return type_func_equal(&(type1->function),&(type2->function));
                break;
            default:
                assert(0);
        }
    }else{
        return 0;
    }
}
Type * entry_list_exist(ID_table_ebtry *head,char* name){
    ID_table_ebtry * temp = head;
    while(temp!=NULL){
        if(strcmp(temp->name,name)==0){
            return temp->type;
        }
        temp = temp->next;
    }
    return NULL;
}
Type * id_exist (ID_table * table,char * name){
    ID_table_ebtry * head = table->entry[hash(name)];
    return entry_list_exist(head,name);
}
void id_insert (ID_table * table,char * name,Type * type){
    if(id_exist(table,name) != NULL)
        assert(0);
    ID_table_ebtry * head = table->entry[hash(name)];
    ID_table_ebtry * temp1, * temp2;
    temp1 = head;
    ID_table_ebtry * p =(ID_table_ebtry *)malloc(sizeof(ID_table_ebtry));
    p->type = type;
    p->name = malloc_copy(name);
    p->next = NULL;
    if(head == NULL){
        table->entry[hash(name)] = p;
    }else{
        temp2 = temp1->next;
        while(temp2!=NULL){
            temp1 = temp2;
            temp2 = temp1->next;
        }
        temp1->next = p;
    }
}
void pstype(Type * type);
void psfield(Fieldlist * field){
    if(field == NULL)
        return;
    pstype(field->type);
    printf("%s ",field->name);
    psfield(field->next);
}
void pstype(Type * type){
    switch (type->kind){
        case basic:
            if (type->basic == 0) {
                printf("int ");
            } else{
                printf("float ");
            }
            break;
        case array:
            printf("[%d]",type->array.size);
            pstype(type->array.type);
            break;
        case structure:
            if(type->structure.structname == NULL){
                printf("struct ");
                psfield(type->structure.field);
            } else {
                printf("struct %s ",type->structure.structname);
                psfield(type->structure.field);
            }
            break;
        case function:
            pstype(type->function.returntype);   
            printf("%s ",type->function.funcname);
            psfield(type->function.varlist);
            break;
        default:
            assert(0);
            break;
    }
}
void psentry(Type *type,char * name){
    pstype(type);
    printf("%s\n",name);
}
void semantic_check (struct tree_node * p){
    if(strcmp(p->name,"Program")==0){
        semantic_check(p->child_node);
    }else if(strcmp(p->name,"ExtDefList")==0){
        switch (p->gno){
            case 0:
                semantic_check(p->child_node);
                semantic_check(p->child_node->bro_node);
                break;
            case 1:
                break;
            default:
                assert(0);
        }
    }else if(strcmp(p->name,"ExtDef")==0){
        switch (p->gno){
            case 0:
                semantic_check(p->child_node);
                if(p->child_node->semantic_error_flag == 1)
                    break;
                p->child_node->bro_node->base_type = p->child_node->var_type;
                semantic_check(p->child_node->bro_node);
                break;
            case 1:
                semantic_check(p->child_node);
                break;
            case 2:
                semantic_check(p->child_node);
                //if(p->child_node->semantic_error_flag == 1)
                    //break;
                p->child_node->bro_node->var_type = new_type();
                p->child_node->bro_node->var_type->kind = function;
                p->child_node->bro_node->var_type->function.returntype = p->child_node->var_type;
                semantic_check(p->child_node->bro_node);
                p->child_node->bro_node->bro_node->func_return_type = p->child_node->bro_node->var_type->function.returntype;
                semantic_check(p->child_node->bro_node->bro_node);
                break;
            default:
                assert(0);
        }
    } else if (strcmp(p->name,"ExtDecList")==0) {
        switch (p->gno){
            case 0:
                p->child_node->base_type = p->base_type;
                semantic_check(p->child_node);
                //psentry(p->child_node->var_type,p->child_node->idname);
                if(id_exist(Global_id_table,p->child_node->idname) != NULL)
                    printf("Error type 3 at Line %d: Redefined variable \"%s\"\n",p->child_node->line,p->child_node->idname);
                else 
                    id_insert(Global_id_table,p->child_node->idname,p->child_node->var_type);
                break;
            case 1:
                p->child_node->base_type = p->base_type;
                semantic_check(p->child_node);
                //psentry(p->child_node->var_type,p->child_node->idname);
                if(id_exist(Global_id_table,p->child_node->idname) != NULL)
                    printf("Error type 3 at Line %d: Redefined variable \"%s\"\n",p->child_node->line,p->child_node->idname);
                else 
                    id_insert(Global_id_table,p->child_node->idname,p->child_node->var_type);
                p->child_node->bro_node->bro_node->base_type = p->base_type;
                semantic_check(p->child_node->bro_node->bro_node);
                break;
            default:
                assert(0);
        }
    } else if (strcmp(p->name,"Specifier")==0) {
        semantic_check(p->child_node);
        p->var_type = p->child_node->var_type;
        p->semantic_error_flag = p->child_node->semantic_error_flag;
    }else if (strcmp(p->name,"TYPE")==0) {
        p->var_type = new_type();
        p->var_type->kind = basic;
        p->var_type->basic = (strcmp(p->idname,"int")==0) ? 0 : 1;
    }else if (strcmp(p->name,"StructSpecifier")==0) {
        int old_in_struct_flag;
        switch (p->gno){
            case 0:
                p->var_type = new_type();
                p->var_type->kind = structure;
                semantic_check(p->child_node->bro_node);
                p->var_type->structure.structname = malloc_copy(p->child_node->bro_node->idname);
                old_in_struct_flag = in_struct_flag;
                in_struct_flag = 1;
                semantic_check(p->child_node->bro_node->bro_node->bro_node);
                in_struct_flag = old_in_struct_flag;
                p->var_type->structure.field = p->child_node->bro_node->bro_node->bro_node->field;
                //psentry(p->var_type,"struct");
                if(strcmp(p->var_type->structure.structname,"0unnamed")!=0){
                    if(id_exist(Global_id_table,p->var_type->structure.structname) != NULL)
                        printf("Error type 16 at Line %d: Redefined struct \"%s\"\n",p->line,p->var_type->structure.structname);
                    else 
                        id_insert(Global_id_table,p->var_type->structure.structname,p->var_type);
                }
                break;
            case 1:
                semantic_check(p->child_node->bro_node);
                p->var_type = id_exist(Global_id_table,p->child_node->bro_node->idname);
                if(p->var_type == NULL){
                    printf("Error type 17 at Line %d: Undefined struct \"%s\"\n",p->line,p->child_node->bro_node->idname);
                    p->semantic_error_flag = 1;
                }
                break;
            default:
                assert(0);
        }
    }else if (strcmp(p->name,"OptTag")==0) {
        if(p->type!=2)
            strcpy(p->idname,p->child_node->idname);
        else 
            strcpy(p->idname,"0unnamed");
    }else if (strcmp(p->name,"Tag")==0) {
        strcpy(p->idname,p->child_node->idname);
    }else if (strcmp(p->name,"VarDec")==0) {
        Type * tptemp = new_type();
        switch (p->gno){
            case 0:
                p->var_type = p->base_type;
                strcpy(p->idname,p->child_node->idname);
                break;
            case 1:
                tptemp->kind = array;
                tptemp->array.size = p->child_node->bro_node->bro_node->val;
                tptemp->array.type = p->base_type;
                p->child_node->base_type = tptemp;
                semantic_check(p->child_node);
                p->var_type = p->child_node->var_type;
                strcpy(p->idname,p->child_node->idname);
                break;
            default:
                assert(0);
        }
    }else if (strcmp(p->name,"FunDec")==0) {
        switch (p->gno){
            case 0:
                p->var_type->function.funcname = malloc_copy(p->child_node->idname);
                semantic_check(p->child_node->bro_node->bro_node);
                p->var_type->function.varlist = p->child_node->bro_node->bro_node->field;
                if(id_exist(Global_id_table,p->var_type->function.funcname) != NULL)
                    printf("Error type 4 at Line %d: Redefined function \"%s\"\n",p->line,p->var_type->function.funcname);
                else 
                    id_insert(Global_id_table,p->var_type->function.funcname,p->var_type);
                //psentry(p->var_type,"func");
                break;
            case 1:
                p->var_type->function.funcname = malloc_copy(p->child_node->idname);
                p->var_type->function.varlist = NULL;
                //psentry(p->var_type,"func");
                if(id_exist(Global_id_table,p->var_type->function.funcname) != NULL)
                    printf("Error type 4 at Line %d: Redefined function \"%s\"\n",p->line,p->var_type->function.funcname);
                else 
                    id_insert(Global_id_table,p->var_type->function.funcname,p->var_type);
                break;
            default:
                assert(0);
        }
    } else if (strcmp(p->name,"VarList")==0) {
        Fieldlist * fdtemp = new_field();
        switch (p->gno){
            case 0:
                semantic_check(p->child_node);
                fdtemp->name = malloc_copy(p->child_node->idname);
                fdtemp->type = p->child_node->var_type;
                semantic_check(p->child_node->bro_node->bro_node);
                fdtemp->next = p->child_node->bro_node->bro_node->field;
                p->field = fdtemp;
                break;
            case 1:
                semantic_check(p->child_node);
                fdtemp->name = malloc_copy(p->child_node->idname);
                fdtemp->type = p->child_node->var_type;
                fdtemp->next = NULL;
                p->field = fdtemp;
                break;
            default:
                assert(0);
        }
    } else if (strcmp(p->name,"ParamDec")==0) {
        semantic_check(p->child_node);
        p->child_node->bro_node->base_type = p->child_node->var_type;
        semantic_check(p->child_node->bro_node);
        p->var_type = p->child_node->bro_node->var_type;
        strcpy(p->idname,p->child_node->bro_node->idname);
        //psentry(p->var_type,p->idname);
        if(id_exist(Global_id_table,p->idname) != NULL)
            printf("Error type 3 at Line %d: Redefined variable \"%s\"\n",p->line,p->idname);
        else 
            id_insert(Global_id_table,p->idname,p->var_type);
    } else if (strcmp(p->name,"CompSt")==0) {
        semantic_check(p->child_node->bro_node);
        p->child_node->bro_node->bro_node->func_return_type = p->func_return_type;
        semantic_check(p->child_node->bro_node->bro_node);
    } else if (strcmp(p->name,"StmtList")==0) {
        switch (p->gno){
            case 0:
                p->child_node->func_return_type = p->func_return_type;
                semantic_check(p->child_node);
                p->child_node->bro_node->func_return_type = p->func_return_type;
                semantic_check(p->child_node->bro_node);
                break;
            case 1:
                break;
            default:
                assert(0);
        }
    } else if (strcmp(p->name,"Stmt")==0) {
        switch (p->gno){
            case 0:
                p->child_node->func_return_type = p->func_return_type;
                semantic_check(p->child_node);
                break;
            case 1:
                p->child_node->func_return_type = p->func_return_type;
                semantic_check(p->child_node);
                break;
            case 2:
                get_child_node(p,2)->func_return_type = p->func_return_type;
                semantic_check(get_child_node(p,2));
                if(type_equal(get_child_node(p,2)->var_type,p->func_return_type) != 1)
                    printf("Error type 8 at Line %d: return type wrong\n",p->line);
                break;
            case 3:
                p->child_node->bro_node->bro_node->func_return_type = p->func_return_type;
                in_logic_flag = 1;
                semantic_check(p->child_node->bro_node->bro_node);
                in_logic_flag = 0;
                get_child_node(p,5)->func_return_type = p->func_return_type;
                semantic_check(get_child_node(p,5));
                break;
            case 4:
                p->child_node->bro_node->bro_node->func_return_type = p->func_return_type;
                in_logic_flag = 1;
                semantic_check(p->child_node->bro_node->bro_node);
                in_logic_flag = 0;
                get_child_node(p,5)->func_return_type = p->func_return_type;
                semantic_check(get_child_node(p,5));
                get_child_node(p,7)->func_return_type = p->func_return_type;
                semantic_check(get_child_node(p,7));
                break;
            case 5:
                p->child_node->bro_node->bro_node->func_return_type = p->func_return_type;
                in_logic_flag = 1;
                semantic_check(p->child_node->bro_node->bro_node);
                in_logic_flag = 0;
                get_child_node(p,5)->func_return_type = p->func_return_type;
                semantic_check(get_child_node(p,5));
                break;
            default:
                assert(0);
        }
    } else if (strcmp(p->name,"DefList")==0) {
        Fieldlist * fdtemp;
        switch (p->gno){
            case 0:
                semantic_check(p->child_node);
                semantic_check(p->child_node->bro_node);
                field_connect(p->child_node->field,p->child_node->bro_node->field);
                p->field = p->child_node->field;
                break;
            case 1:
                p->field = NULL;
                break;
            default:
                assert(0);
        }
    } else if (strcmp(p->name,"Def")==0) {
        semantic_check(p->child_node);
        p->child_node->bro_node->base_type = p->child_node->var_type;
        semantic_check(p->child_node->bro_node);
        p->field = p->child_node->bro_node->field;
    } else if (strcmp(p->name,"DecList")==0) {
        Fieldlist * fdtemp = new_field();
        switch (p->gno){
            case 0:
                p->child_node->base_type = p->base_type;
                semantic_check(p->child_node);
                fdtemp->name = malloc_copy(p->child_node->idname);
                fdtemp->type = p->child_node->var_type;
                fdtemp->next = NULL;
                p->field = fdtemp;
                break;
            case 1:
                p->child_node->base_type = p->base_type;
                semantic_check(p->child_node);
                fdtemp->name = malloc_copy(p->child_node->idname);
                fdtemp->type = p->child_node->var_type;
                p->child_node->bro_node->bro_node->base_type = p->base_type;
                semantic_check(p->child_node->bro_node->bro_node);
                fdtemp->next = p->child_node->bro_node->bro_node->field;
                p->field = fdtemp;
                break;
            default:
                assert(0);
        }
    } else if (strcmp(p->name,"Dec")==0) {
        int side_type_equal;
        switch (p->gno){
            case 0:
                p->child_node->base_type = p->base_type;
                semantic_check(p->child_node);
                strcpy(p->idname,p->child_node->idname);
                p->var_type = p->child_node->var_type;
                //psentry(p->var_type,p->idname);
                if(id_exist(Global_id_table,p->idname) != NULL){
                    if(in_struct_flag == 0)
                        printf("Error type 3 at Line %d: Redefined variable \"%s\"\n",p->line,p->idname);
                    else 
                        printf("Error type 15 at Line %d: Redefined field \"%s\"\n",p->line,p->idname);
                }
                else 
                    id_insert(Global_id_table,p->idname,p->var_type);
                break;
            case 1:
                p->child_node->base_type = p->base_type;
                semantic_check(p->child_node);
                strcpy(p->idname,p->child_node->idname);
                p->var_type = p->child_node->var_type;
                //psentry(p->var_type,p->idname);
                if(in_struct_flag == 1){
                    printf("Error type 15 at Line %d: initialize in struct \n",p->line);
                    break;
                }
                if(id_exist(Global_id_table,p->idname) != NULL)
                    printf("Error type 3 at Line %d: Redefined variable \"%s\"\n",p->line,p->idname);
                else 
                    id_insert(Global_id_table,p->idname,p->var_type);
                semantic_check(get_child_node(p,3));
                side_type_equal = ((type_equal(get_child_node(p,1)->var_type,get_child_node(p,3)->var_type) == 1)&&(get_child_node(p,1)->var_type!=NULL)) ? 1:0;
                p->var_type = (side_type_equal == 1) ? get_child_node(p,1)->var_type : NULL;
                if(side_type_equal == 0)
                    printf("Error type 5 at Line %d: assignop type unequal\n",p->line);
                break;
            default:
                assert(0);
        }
    } else if (strcmp(p->name,"Exp")==0) {
        int side_type_equal;
        Type * tptemp;
        switch (p->gno){
        case 0:
            exp_error_handler(p,0);
            break;
        case 1:
            exp_error_handler(p,1);
            break;
        case 2:
            exp_error_handler(p,1);
            break;
        case 3:
            exp_error_handler(p,1);
            break; 
        case 4:
            exp_error_handler(p,2);
            break;
        case 5:
            exp_error_handler(p,2);
            break;
        case 6:
            exp_error_handler(p,2);
            break;
        case 7:
            exp_error_handler(p,2);
            break;
        case 8:
            semantic_check(get_child_node(p,2));
            p->var_type = get_child_node(p,2)->var_type;
            break;
        case 9:
            semantic_check(get_child_node(p,2));
            if(get_child_node(p,2)->var_type->kind == basic){
                p->var_type = get_child_node(p,2)->var_type;
            } else{
                p->var_type = NULL;
                printf("Error type 7 at Line %d: operation type unequal\n",p->line);
            }
            break;
        case 10:
            semantic_check(get_child_node(p,2));
            if((get_child_node(p,2)->var_type->kind == basic)&&(get_child_node(p,2)->var_type->basic == 0)){
                p->var_type = get_child_node(p,2)->var_type;
            } else{
                p->var_type = NULL;
                printf("Error type 7 at Line %d: operation type unequal\n",p->line);
            }
            break;
        case 11:
            tptemp = id_exist(Global_id_table,p->child_node->idname);
            if(tptemp  == NULL){
                printf("Error type 2 at Line %d: Undefined function\n",p->line);
                p->var_type = NULL;
                break;
            }
            if(tptemp->kind != function){
                printf("Error type 11 at Line %d: cast to a non-function\n",p->line);
                p->var_type = NULL;
                break;
            }
            semantic_check(get_child_node(p,3));
            //psfield(get_child_node(p,3)->field);printf("\n");
            //psfield(tptemp->function.varlist);printf("\n");
            if(field_equal(get_child_node(p,3)->field,tptemp->function.varlist) == 1){
                p->var_type = tptemp->function.returntype;
            } else {
                printf("Error type 9 at Line %d: param not match\n",p->line);
                p->var_type = NULL;
            }
            break;
        case 12:
            tptemp = id_exist(Global_id_table,p->child_node->idname);
            if(tptemp  == NULL){
                printf("Error type 2 at Line %d: Undefined function\n",p->line);
                p->var_type = NULL;
                break;
            }
            if(tptemp->kind != function){
                printf("Error type 11 at Line %d: cast to a non-function\n",p->line);
                p->var_type = NULL;
                break;
            }
            if(field_equal(NULL,tptemp->function.varlist) == 1){
                p->var_type = tptemp->function.returntype;
            } else {
                printf("Error type 9 at Line %d: param not match\n",p->line);
                p->var_type = NULL;
            }
            break;
        case 13:
            //printf("in!!\n");
            semantic_check(get_child_node(p,1));
            semantic_check(get_child_node(p,3));
            //psentry(p->child_node->var_type,"yes");
            if(get_child_node(p,1)->var_type->kind != array){
                printf("Error type 10 at Line %d: cast to a non-array\n",p->line);
                p->var_type = NULL;
            } else {
                p->var_type = get_child_node(p,1)->var_type->array.type;
                p->left_flag = get_child_node(p,1)->left_flag;
            }
            if ((get_child_node(p,3)->var_type->kind!=basic)||(get_child_node(p,3)->var_type->basic != 0)){
                printf("Error type 12 at Line %d: array offset non-integer\n",p->line);
                p->var_type = NULL;
                p->left_flag = 0;
            }
            //printf("out!!\n");
            break;
        case 14:
            semantic_check(p->child_node);
            if(p->child_node->var_type == NULL){
                printf("Error type 13 at Line %d: cast to a non-structure\n",p->line);
                p->var_type = NULL;
                break;
            }
            if(p->child_node->var_type->kind != structure){
                printf("Error type 13 at Line %d: cast to a non-structure\n",p->line);
                p->var_type = NULL;
                break;
            }
            tptemp = symbol_in_field(get_child_node(p,3)->idname,p->child_node->var_type->structure.field);
            if(tptemp == NULL){
                printf("Error type 14 at Line %d: Undefined field\n",p->line);
                p->var_type = NULL;
                break;
            } else {
                p->var_type = tptemp;
                p->left_flag = p->child_node->left_flag;
            }
            break;
        case 15:
            p->var_type = id_exist(Global_id_table,p->child_node->idname);
            //psentry(p->var_type,p->child_node->idname);
            if(p->var_type != NULL){
                p->left_flag = 1;
                if((p->var_type->kind == structure)){
                    if (strcmp(p->var_type->structure.structname,p->child_node->idname)==0){
                        p->left_flag = 0;
                        p->var_type = NULL;
                        printf("Error type 1 at Line %d: Undefined variable\n",p->line);
                    }
                }
            } else {
                p->left_flag = 0;
                printf("Error type 1 at Line %d: Undefined variable\n",p->line);
            }
            break;
        case 16:
            p->var_type = new_type();
            p->var_type->kind = basic;
            p->var_type->basic = 0;
            p->left_flag = 0;
            break;
        case 17:
            p->var_type = new_type();
            p->var_type->kind = basic;
            p->var_type->basic = 1;
            p->left_flag = 0;
            break;
        default:
            assert(0);
        }
    } else if (strcmp(p->name,"Args")==0) {
        Fieldlist * fdtemp = new_field();
        switch (p->gno) {
        case 0:
            semantic_check(p->child_node);
            fdtemp->name = malloc_copy("0arg");
            fdtemp->type = p->child_node->var_type;
            semantic_check(get_child_node(p,3));
            fdtemp->next = get_child_node(p,3)->field;
            p->field = fdtemp;
            break;
        case 1:
            semantic_check(p->child_node);
            fdtemp->name = malloc_copy("0arg");
            fdtemp->type = p->child_node->var_type;
            fdtemp->next = NULL;
            p->field = fdtemp;
            break;
        default:
            assert(0);
        }
    } else {
        assert(0);
    }
    
    
    
    
    return;
    
}
void exp_error_handler(struct tree_node * p,int task){
    int side_type_equal;
    switch (task){
    case 0:
        semantic_check(get_child_node(p,1));
        semantic_check(get_child_node(p,3));
        side_type_equal = ((type_equal(get_child_node(p,1)->var_type,get_child_node(p,3)->var_type) == 1)&&(get_child_node(p,1)->var_type!=NULL)) ? 1:0;
        p->var_type = (side_type_equal == 1) ? get_child_node(p,1)->var_type : NULL;
        if(side_type_equal == 0)
            printf("Error type 5 at Line %d: assignop type unequal\n",p->line);
        if(get_child_node(p,1)->left_flag == 0)
            printf("Error type 6 at Line %d: assign to a right-value\n",p->line);
        break;
    case 1:
        semantic_check(get_child_node(p,1));
        semantic_check(get_child_node(p,3));
        side_type_equal = ((type_equal(get_child_node(p,1)->var_type,get_child_node(p,3)->var_type) == 1)&&(get_child_node(p,1)->var_type!=NULL)) ? 1:0;
        if(side_type_equal == 1)
            side_type_equal = ((get_child_node(p,1)->var_type->kind == basic)&&(get_child_node(p,1)->var_type->basic == 0)) ?1:0;
        p->var_type = (side_type_equal == 1) ? get_child_node(p,1)->var_type : NULL;
        if(side_type_equal == 0)
            printf("Error type 7 at Line %d: operation type unequal\n",p->line);
        break;
    case 2:
        semantic_check(get_child_node(p,1));
        semantic_check(get_child_node(p,3));
        side_type_equal = ((type_equal(get_child_node(p,1)->var_type,get_child_node(p,3)->var_type) == 1)&&(get_child_node(p,1)->var_type!=NULL)) ? 1:0;
        if(side_type_equal == 1)
            side_type_equal = (get_child_node(p,1)->var_type->kind == basic) ?1:0;
        p->var_type = (side_type_equal == 1) ? get_child_node(p,1)->var_type : NULL;
        if(side_type_equal == 0)
            printf("Error type 7 at Line %d: operation type unequal\n",p->line);
        break;
    default:
        break;
    }
}