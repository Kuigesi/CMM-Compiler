#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include"tree.h" 
#include<assert.h>
void translate_CompSt (struct tree_node * p,Variable_table * table);
void translate_VarDec (struct tree_node * p,Variable_table * table);
void translate_Exp (struct tree_node * p,Variable_table * table);
void translate_Cond (struct tree_node * p,Oprand * true_label,Oprand * false_label,Variable_table * table);
Oprand * variable_entry_list_lookup(Variable_table_entry *head,char * name){
    Variable_table_entry * temp = head;
    while(temp!=NULL){
        if(strcmp(temp->id,name)==0){
            return temp->var;
        }
        temp = temp->next;
    }
    return NULL;
}
Oprand * variable_lookup (Variable_table * table,char * name){
    Variable_table_entry * head = table->entry[hash(name)];
    return variable_entry_list_lookup(head,name);
}
void variable_insert (Variable_table * table,char * name){
    if(variable_lookup(table,name) != NULL)
        assert(0);
    global_variable_no = global_variable_no + 1;
    Oprand * variable_temp = (Oprand *)malloc(sizeof(Oprand));
    variable_temp->kind = OP_VARIABLE;
    variable_temp->get_addr = variable_temp->get_val = variable_temp->in_dec = 0;
    variable_temp->u.var_no = global_variable_no;

    Variable_table_entry * head = table->entry[hash(name)];
    Variable_table_entry * temp1, * temp2;
    temp1 = head;
    Variable_table_entry * p =(Variable_table_entry *)malloc(sizeof(Variable_table_entry));
    p->var = variable_temp;
    p->id = malloc_copy(name);
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
Oprand * new_op_temp(){
    global_temp_no = global_temp_no +1;
    Oprand * temp = (Oprand *)malloc(sizeof(Oprand));
    temp->kind = OP_TEMP;
    temp->get_addr = temp->get_val = temp->in_dec = 0;
    temp->u.temp_no = global_temp_no;
    return temp;
}
Oprand * new_op_label(){
    global_label_no = global_label_no +1;
    Oprand * temp = (Oprand *)malloc(sizeof(Oprand));
    temp->kind = OP_LABEL;
    temp->get_addr = temp->get_val =temp->in_dec = 0;
    temp->u.label_no = global_label_no;
    return temp;
}

Oprand * new_op_constant(int value){
    Oprand * temp = (Oprand *)malloc(sizeof(Oprand));
    temp->kind = OP_CONSTANT;
    temp->get_addr = temp->get_val = temp->in_dec = 0;
    temp->u.value = value;
    return temp;
}

Oprand * new_op_function(char * name){
    Oprand * temp = (Oprand *)malloc(sizeof(Oprand));
    temp->kind = OP_FUNCTION;
    temp->get_addr = temp->get_val = temp->in_dec = 0;
    temp->u.name = malloc_copy(name);
    return temp;
}

//*****若修改oprand结构体需要修改此处******//

Oprand * mlloc_copy_oprand(Oprand * op){ 
    Oprand * new_op = (Oprand *)malloc(sizeof(Oprand));
    if(op == NULL)
        assert(0);
    new_op->kind = op->kind;
    new_op->in_dec = op->in_dec;
    new_op->get_val = op->get_val;
    new_op->get_addr = op->get_addr;
    switch (op->kind){
    case OP_LABEL:
        new_op->u.label_no = op->u.label_no;
        break;
    case OP_FUNCTION:
        new_op->u.name = op->u.name;
        break;
    case OP_VARIABLE:
        new_op->u.var_no = op->u.var_no;
        break;
    case OP_TEMP:
        new_op->u.temp_no = op->u.temp_no;
        break;
    case OP_CONSTANT:
        new_op->u.value = op->u.value;
        break;
    default:
        assert(0);
        break;
    }
    return new_op;
}
Oprand * set_oprand(Oprand * op,int get_addr,int get_val,int in_dec){
    Oprand * new_op = mlloc_copy_oprand(op);
    new_op->get_addr = get_addr;
    new_op->get_val = get_val;
    new_op->in_dec = in_dec;
    return new_op;
}
Intercode * generate_code(int cno,...){
    va_list valist;
    va_start(valist,cno);
    Intercode * tempcode = (Intercode *)malloc(sizeof(Intercode));
    tempcode->kind = cno;
    tempcode->next = tempcode->prev = NULL;
    switch (cno){
    case CODE_LABEL:
        tempcode->u.label.l = va_arg(valist, Oprand *);
        break;
    case CODE_FUNCTION:
        tempcode->u.function_def.f = va_arg(valist, Oprand *);
        break;
    case CODE_ASSIGN:
        tempcode->u.assign.result = va_arg(valist, Oprand *);
        tempcode->u.assign.op1 = va_arg(valist, Oprand *);
        break;
    case CODE_BINOP:
        tempcode->u.binop.result = va_arg(valist, Oprand *);
        tempcode->u.binop.op1 = va_arg(valist, Oprand *);
        tempcode->u.binop.op2 = va_arg(valist, Oprand *);
        tempcode->u.binop.bop = va_arg(valist, int);
        break;
    case CODE_JMP:
        tempcode->u.jmp.des = va_arg(valist, Oprand *);
        break;
    case CODE_CONDJMP:
        tempcode->u.condjmp.des = va_arg(valist, Oprand *);
        tempcode->u.condjmp.x = va_arg(valist, Oprand *);
        tempcode->u.condjmp.y = va_arg(valist, Oprand *);
        tempcode->u.condjmp.relop = va_arg(valist,int);
        break;
    case CODE_RETURN:
        tempcode->u.return_val.x = va_arg(valist, Oprand *);
        break;
    case CODE_MEMDEC:
        tempcode->u.memdec.x = va_arg(valist, Oprand *);
        tempcode->u.memdec.size = set_oprand(va_arg(valist, Oprand *),0,0,1);
        break;
    case CODE_ARG:
        tempcode->u.arg.x = va_arg(valist, Oprand *);
        break;
    case CODE_CALL:
        tempcode->u.function_call.x = va_arg(valist, Oprand *);
        tempcode->u.function_call.f = va_arg(valist, Oprand *);
        break;
    case CODE_PARAM:
        tempcode->u.param.x = va_arg(valist, Oprand *);
        break;
    case CODE_READ:
        tempcode->u.read.x = va_arg(valist, Oprand *);
        break;
    case CODE_WRITE:
        tempcode->u.write.x = va_arg(valist, Oprand *);
        break;
    default:
        assert(0);
        break;
    }
    return tempcode;
} 
void print_oprand(FILE * stream,Oprand * op){
    if(op == NULL)
        assert(0);
    //printf("%d\n",op->kind);
    switch (op->kind){
    case OP_LABEL:
        fprintf(stream,"label%d",op->u.label_no);
        break;
    case OP_FUNCTION:
        fprintf(stream,"%s",op->u.name);
        break;
    case OP_VARIABLE:
        if(op->get_addr == 1){
            if(op->get_val == 1)
                assert(0);
            fprintf(stream,"&v%d",op->u.var_no);
        } else if (op->get_val == 1){
            fprintf(stream,"*v%d",op->u.var_no);
        } else {
            fprintf(stream,"v%d",op->u.var_no);
        }
        break;
    case OP_TEMP:
        if(op->get_addr == 1){
            if(op->get_val == 1)
                assert(0);
            fprintf(stream,"&t%d",op->u.temp_no);
        } else if (op->get_val == 1){
            fprintf(stream,"*t%d",op->u.temp_no);
        } else {
            fprintf(stream,"t%d",op->u.temp_no);
        }
        break;
    case OP_CONSTANT:
        if(op->in_dec == 0)
            fprintf(stream,"#%d",op->u.value);
        else 
            fprintf(stream,"%d",op->u.value);
        break;
    default:
        assert(0);
        break;
    }
}
void print_code(FILE *stream,Intercode * code){
    if(code == NULL)
        return;
    int cno = code->kind;
    //printf("code : %d\n",cno);
    switch (cno){
    case CODE_LABEL:
        fprintf(stream,"LABEL ");print_oprand(stream,code->u.label.l);fprintf(stream," :");
        break;
    case CODE_FUNCTION:
        fprintf(stream,"FUNCTION ");print_oprand(stream,code->u.function_def.f);fprintf(stream," :");
        break;
    case CODE_ASSIGN:
        print_oprand(stream,code->u.assign.result);fprintf(stream," := ");print_oprand(stream,code->u.assign.op1);
        break;
    case CODE_BINOP:
        print_oprand(stream,code->u.binop.result);fprintf(stream," := ");print_oprand(stream,code->u.binop.op1);
        switch (code->u.binop.bop) {
        case BOP_ADD:
            fprintf(stream," + ");
            break;
        case BOP_DIV:
            fprintf(stream," / ");
            break;
        case BOP_MUL:
            fprintf(stream," * ");
            break;
        case BOP_SUB:
            fprintf(stream," - ");
            break;
        default:
            assert(0);
            break;
        }
        print_oprand(stream,code->u.binop.op2);
        break;
    case CODE_JMP:
        fprintf(stream,"GOTO ");print_oprand(stream,code->u.jmp.des);
        break;
    case CODE_CONDJMP:
        fprintf(stream,"IF ");print_oprand(stream,code->u.condjmp.x);
        switch (code->u.condjmp.relop) {
        case RLP_EQ:
            fprintf(stream," == ");
            break;
        case RLP_G:
            fprintf(stream," > ");
            break;
        case RLP_GEQ:
            fprintf(stream," >= ");
            break;
        case RLP_L:
            fprintf(stream," < ");
            break;
        case RLP_LEQ:
            fprintf(stream," <= ");
            break;
        case RLP_NEQ:
            fprintf(stream," != ");
            break;
        default:
            assert(0);
            break;
        }
        print_oprand(stream,code->u.condjmp.y);fprintf(stream," GOTO ");print_oprand(stream,code->u.condjmp.des);
        break;
    case CODE_RETURN:
        fprintf(stream,"RETURN ");print_oprand(stream,code->u.return_val.x);
        break;
    case CODE_MEMDEC:
        fprintf(stream,"DEC ");print_oprand(stream,code->u.memdec.x);fprintf(stream," ");print_oprand(stream,code->u.memdec.size);
        break;
    case CODE_ARG:
        fprintf(stream,"ARG ");print_oprand(stream,code->u.arg.x);
        break;
    case CODE_CALL:
        print_oprand(stream,code->u.function_call.x);fprintf(stream," := CALL ");print_oprand(stream,code->u.function_call.f);
        break;
    case CODE_PARAM:
        fprintf(stream,"PARAM ");print_oprand(stream,code->u.param.x);
        break;
    case CODE_READ:
        fprintf(stream,"READ ");print_oprand(stream,code->u.read.x);
        break;
    case CODE_WRITE:
        fprintf(stream,"WRITE ");print_oprand(stream,code->u.write.x);
        break;
    default:
        assert(0);
        break;
    }
    fprintf(stream,"\n");
    print_code(stream,code->next);
    return;
} 
Intercode * merge_code(Intercode * c1,Intercode * c2){
    if(c1 == NULL)
        return c2;
    if(c2 == NULL)
        return c1;
    
    Intercode * temp1, * temp2, * tempprev;
    temp1 = c1;
    temp2 = temp1->next;
    while(temp2 != NULL){
        tempprev = temp1;
        temp1 = temp2;
        temp2 = temp1->next;
        if(temp1->prev != tempprev)
            assert(0);
    }
    //printf("1\n");
    Intercode * check1,*check2,*checkprev;
    check1 = c2;
    check2 = check1->next;
    while(check2 != NULL){
        checkprev = check1;
        check1 = check2;
        check2 = check1->next;
        if(check1->prev != checkprev)
            assert(0);
    }
    //printf("2\n");
    temp1->next = c2;
    c2->prev = temp1;
    return c1;
}
void translate_program_test (struct tree_node * p,Variable_table * table){
        Oprand * v1,*v2,*v3,*v4,*l1,*f,*x,*size;
        variable_insert(table,"a");
        variable_insert(table,"b");
        variable_insert(table,"c");
        v1 = variable_lookup(table,"a");
        v2 = variable_lookup(table,"b");
        v3 = variable_lookup(table,"c");
        v4 = new_op_constant(4);
        l1 = new_op_label();
        f = new_op_function("f");
        x = v1;
        size = v4;
        Intercode * c1 = generate_code(CODE_LABEL,l1);
        c1 =  merge_code(c1,generate_code(CODE_FUNCTION,f));
        c1 =  merge_code(c1,generate_code(CODE_ASSIGN,v1,v2));
        c1 =   merge_code(c1,generate_code(CODE_BINOP,v1,set_oprand(v2,1,0,0),v3,BOP_ADD));
        c1 =  merge_code(c1,generate_code(CODE_BINOP,v1,set_oprand(v2,0,1,0),size,BOP_DIV));
        c1 =  merge_code(c1,generate_code(CODE_JMP,l1));
        c1 = merge_code(c1,generate_code(CODE_CONDJMP,l1,v1,v2,RLP_EQ));
        c1 = merge_code(c1,generate_code(CODE_RETURN,v1));
        c1 = merge_code(c1,generate_code(CODE_MEMDEC,x,size));
        c1 = merge_code(c1,generate_code(CODE_ARG,x));
        c1 = merge_code(c1,generate_code(CODE_CALL,x,f));
        c1 = merge_code(c1,generate_code(CODE_PARAM,x));
        c1 = merge_code(c1,generate_code(CODE_READ,x));
        c1 = merge_code(c1,generate_code(CODE_WRITE,x));
        print_code(stdout,c1);
        
}
Intercode * merge_child_code(struct tree_node * p,int child1,int child2){
    return merge_code(get_child_node(p,child1)->code,get_child_node(p,child2)->code); 
}
Intercode * generate_bop_code(struct tree_node * p,int exp_bop){
    return generate_code(CODE_BINOP,p->store_place,get_child_node(p,1)->store_place,get_child_node(p,3)->store_place,exp_bop);
}
Intercode * generate_Args_code (struct tree_node * p){
    Intercode * temp;
    switch (p->gno) {
    case 0:
        temp = generate_Args_code(get_child_node(p,3));
        return merge_code(temp,generate_code(CODE_ARG,get_child_node(p,1)->store_place));
    case 1:
        return generate_code(CODE_ARG,get_child_node(p,1)->store_place);
    default:
        assert(0);break;
    }
}
void translate_Args (struct tree_node * p,Variable_table * table){
    Oprand * temp;
    switch (p->gno) {
    case 0:
        translate_Exp(get_child_node(p,1),table);
        if(get_child_node(p,1)->store_place->get_val == 1){
            temp = new_op_temp();
            get_child_node(p,1)->code = merge_code(get_child_node(p,1)->code,generate_code(CODE_ASSIGN,temp,get_child_node(p,1)->store_place));
            get_child_node(p,1)->store_place = temp;   
        }
        translate_Args(get_child_node(p,3),table);
        p->code = merge_child_code(p,1,3);
        break;
    case 1:
        translate_Exp(get_child_node(p,1),table);
        if(get_child_node(p,1)->store_place->get_val == 1){
            temp = new_op_temp();
            get_child_node(p,1)->code = merge_code(get_child_node(p,1)->code,generate_code(CODE_ASSIGN,temp,get_child_node(p,1)->store_place));
            get_child_node(p,1)->store_place = temp;
            p->code = get_child_node(p,1)->code;
        } else {
            p->code = get_child_node(p,1)->code;
        }
        break;
    default:
        break;
    }
}
int get_relop(char * name){
    if (strcmp(name,">")==0)
        return 0;
    if (strcmp(name,"<")==0)
        return 1;
    if (strcmp(name,"==")==0)
        return 2;
    if (strcmp(name,">=")==0)
        return 3;
    if (strcmp(name,"<=")==0)
        return 4;
    if (strcmp(name,"!=")==0)
        return 5;
    assert(0);
}
void translate_Cond (struct tree_node * p , Oprand * true_label,Oprand * false_label,Variable_table * table){
    Oprand * l1,* l2;
    int exp_rlp;
    switch (p->gno) {
    case 1: //AND//
        l1 = new_op_label();
        translate_Cond(get_child_node(p,1),l1,false_label,table);
        translate_Cond(get_child_node(p,3),true_label,false_label,table);
        p->code = merge_code(get_child_node(p,1)->code,generate_code(CODE_LABEL,l1));
        p->code = merge_code(p->code,get_child_node(p,3)->code);break;
    case 2: //OR//
        l1 = new_op_label();
        translate_Cond(get_child_node(p,1),true_label,l1,table);
        translate_Cond(get_child_node(p,3),true_label,false_label,table);
        p->code = merge_code(get_child_node(p,1)->code,generate_code(CODE_LABEL,l1));
        p->code = merge_code(p->code,get_child_node(p,3)->code);break;
    case 3: //RELOP//
        translate_Exp(get_child_node(p,1),table);
        translate_Exp(get_child_node(p,3),table);
        exp_rlp = get_relop(get_child_node(p,2)->idname);
        p->code = merge_child_code(p,1,3);
        p->code = merge_code(p->code,generate_code(CODE_CONDJMP,true_label,get_child_node(p,1)->store_place,get_child_node(p,3)->store_place,exp_rlp));
        p->code = merge_code(p->code,generate_code(CODE_JMP,false_label));break;
    case 8: //( )//
        translate_Cond(get_child_node(p,2),true_label,false_label,table);
        p->code = get_child_node(p,2)->code;break;
    case 10: //NOT//
        translate_Cond(get_child_node(p,2),false_label,true_label,table);
        p->code = get_child_node(p,2)->code;break;
    default: //OTHER//
        translate_Exp(p,table);
        p->code = merge_code(p->code,generate_code(CODE_CONDJMP,true_label,p->store_place,new_op_constant(0),RLP_NEQ));
        p->code = merge_code(p->code,generate_code(CODE_JMP,false_label));break;
    }
}
int check_is_var_id(struct tree_node * p){
    if(p->gno == 15){
        return 1;
    } else if (p->gno == 13){
        return 0;
    } else {
        if(p->gno != 8)
            assert(0);
        return check_is_var_id(get_child_node(p,2));
    }
}
void translate_Exp (struct tree_node * p,Variable_table * table){
    int exp_bop;
    Oprand * temp,* l1,* l2,* temp2,* temp3;
    switch (p->gno) {
    case 0:
        if(check_is_var_id(get_child_node(p,1)) == 1){
            get_child_node(p,1)->left_flag = 1;
            translate_Exp(get_child_node(p,1),table);
            get_child_node(p,3)->store_place = get_child_node(p,1)->store_place;
            translate_Exp(get_child_node(p,3),table);
            p->code = get_child_node(p,3)->code;
            if(p->store_place != NULL)
                p->code = merge_code(p->code,generate_code(CODE_ASSIGN,p->store_place,get_child_node(p,3)->store_place));
            p->store_place = get_child_node(p,3)->store_place;
        } else {
            get_child_node(p,3)->store_place = NULL;
            translate_Exp(get_child_node(p,3),table);
            get_child_node(p,1)->left_flag = 1;
            translate_Exp(get_child_node(p,1),table);
            p->code = get_child_node(p,3)->code;
            p->code = merge_code(p->code,get_child_node(p,1)->code);
            p->code = merge_code(p->code,generate_code(CODE_ASSIGN,get_child_node(p,1)->store_place,get_child_node(p,3)->store_place));
            if(p->store_place != NULL)
                p->code = merge_code(p->code,generate_code(CODE_ASSIGN,p->store_place,get_child_node(p,3)->store_place));
            p->store_place = get_child_node(p,3)->store_place;
        }
        break;
    case 1:
    case 2:
    case 3: 
        l1 = new_op_label();
        l2 = new_op_label();
        translate_Cond(p,l1,l2,table);
        if(p->store_place == NULL)
            p->store_place = new_op_temp();
        p->code = merge_code(generate_code(CODE_ASSIGN,p->store_place,new_op_constant(0)),p->code);
        p->code = merge_code(p->code,generate_code(CODE_LABEL,l1));
        p->code = merge_code(p->code,generate_code(CODE_ASSIGN,p->store_place,new_op_constant(1)));
        p->code = merge_code(p->code,generate_code(CODE_LABEL,l2));break;
    case 4:
    case 5:
    case 6:
    case 7:
        exp_bop = p->gno - 4;
        translate_Exp(get_child_node(p,1),table);
        translate_Exp(get_child_node(p,3),table);
        if(p->store_place == NULL)
            p->store_place = new_op_temp();
        p->code = merge_child_code(p,1,3);
        p->code = merge_code(p->code,generate_bop_code(p,exp_bop));
        break;
    case 8:
        get_child_node(p,2)->store_place = p->store_place;
        get_child_node(p,2)->left_flag = p->left_flag;
        translate_Exp(get_child_node(p,2),table);
        p->store_place = get_child_node(p,2)->store_place;
        p->code = get_child_node(p,2)->code;
        break;
    case 9:
        if(get_child_node(p,2)->gno == 16){
            temp = new_op_constant(-get_child_node(p,2)->child_node->val);
            if(p->store_place != NULL)
                p->code =  generate_code(CODE_ASSIGN,p->store_place,temp);
            p->store_place = temp;
        } else {
            temp = new_op_constant(0);
            translate_Exp(get_child_node(p,2),table);
            if(p->store_place == NULL)
                p->store_place = new_op_temp();
            p->code = get_child_node(p,2)->code;
            p->code = merge_code(p->code, generate_code(CODE_BINOP,p->store_place,temp,get_child_node(p,2)->store_place,BOP_SUB));
        }
        break;
    case 10:
        l1 = new_op_label();
        l2 = new_op_label();
        translate_Cond(p,l1,l2,table);
        if(p->store_place == NULL)
            p->store_place = new_op_temp();
        p->code = merge_code(generate_code(CODE_ASSIGN,p->store_place,new_op_constant(0)),p->code);
        p->code = merge_code(p->code,generate_code(CODE_LABEL,l1));
        p->code = merge_code(p->code,generate_code(CODE_ASSIGN,p->store_place,new_op_constant(1)));
        p->code = merge_code(p->code,generate_code(CODE_LABEL,l2));break;
    case 11:
        if(strcmp(get_child_node(p,1)->idname,"write") == 0){
            translate_Exp(get_child_node(p,3)->child_node,table);
            p->code = get_child_node(p,3)->child_node->code;
            if(get_child_node(p,3)->child_node->store_place->get_val == 1){
                temp = new_op_temp();
                p->code = merge_code(p->code,generate_code(CODE_ASSIGN,temp,get_child_node(p,3)->child_node->store_place));
                p->code = merge_code(p->code, generate_code(CODE_WRITE,temp));
            } else {
                p->code = merge_code(p->code, generate_code(CODE_WRITE,get_child_node(p,3)->child_node->store_place));
            }
        } else {
            translate_Args(get_child_node(p,3),table);
            p->code = get_child_node(p,3)->code;
            p->code = merge_code(p->code,generate_Args_code(get_child_node(p,3)));
            if(p->store_place == NULL)
                p->store_place = new_op_temp();
            if(p->store_place->get_val == 1){
                temp = new_op_temp();
                p->code = merge_code(p->code, generate_code(CODE_CALL,temp,new_op_function(get_child_node(p,1)->idname)));
                p->code = merge_code(p->code,generate_code(CODE_ASSIGN,p->store_place,temp));
            } else {
                p->code = merge_code(p->code, generate_code(CODE_CALL,p->store_place,new_op_function(get_child_node(p,1)->idname)));
            }
        }
        break;
    case 12:
        if(p->store_place == NULL)
            p->store_place = new_op_temp();
        if(strcmp(get_child_node(p,1)->idname,"read") == 0){ 
            if(p->store_place->get_val == 1){
                temp = new_op_temp();
                p->code = generate_code(CODE_READ,temp);
                p->code = merge_code(p->code,generate_code(CODE_ASSIGN,p->store_place,temp));
            } else {
                p->code = generate_code(CODE_READ,p->store_place);
            }
        } else {
            if(p->store_place->get_val == 1){
                temp = new_op_temp();
                p->code = generate_code(CODE_CALL,temp,new_op_function(get_child_node(p,1)->idname));
                p->code = merge_code(p->code,generate_code(CODE_ASSIGN,p->store_place,temp));
            } else {
                p->code = generate_code(CODE_CALL,p->store_place,new_op_function(get_child_node(p,1)->idname));
            }
        }
        break;
    case 13:
        if(p->left_flag == 1){
            translate_Exp(get_child_node(p,1),table);
            translate_Exp(get_child_node(p,3),table);
            temp = new_op_temp();
            if(get_child_node(p,3)->gno == 16){
                p->code = generate_code(CODE_BINOP,temp,set_oprand(get_child_node(p,1)->store_place,1,0,0),new_op_constant(4*get_child_node(p,3)->child_node->val),BOP_ADD);
                p->store_place = set_oprand(temp,0,1,0);
            }
            else{
                p->code = generate_code(CODE_BINOP,temp,new_op_constant(4),get_child_node(p,3)->store_place,BOP_MUL);
                temp2 = new_op_temp();
                p->code = merge_code(p->code,generate_code(CODE_BINOP,temp2,set_oprand(get_child_node(p,1)->store_place,1,0,0),temp,BOP_ADD));
                p->store_place = set_oprand(temp2,0,1,0);
            }    
        } else {
            if(p->store_place != NULL){
                translate_Exp(get_child_node(p,1),table);
                translate_Exp(get_child_node(p,3),table);
                temp = new_op_temp();
                if(get_child_node(p,3)->gno == 16){
                    p->code = generate_code(CODE_BINOP,temp,set_oprand(get_child_node(p,1)->store_place,1,0,0),new_op_constant(4*get_child_node(p,3)->child_node->val),BOP_ADD);
                    temp2 = new_op_temp();
                    p->code = merge_code(p->code,generate_code(CODE_ASSIGN,temp2,set_oprand(temp,0,1,0)));
                    p->code = merge_code(p->code,generate_code(CODE_ASSIGN,p->store_place,temp2));
                    p->store_place = temp2;
                } else{
                    p->code = get_child_node(p,3)->code;
                    p->code = merge_code(p->code,generate_code(CODE_BINOP,temp,new_op_constant(4),get_child_node(p,3)->store_place,BOP_MUL));
                    temp2 = new_op_temp();
                    temp3 = new_op_temp();
                    p->code = merge_code(p->code,generate_code(CODE_BINOP,temp2,set_oprand(get_child_node(p,1)->store_place,1,0,0),temp,BOP_ADD));
                    p->code = merge_code(p->code,generate_code(CODE_ASSIGN,temp3,set_oprand(temp2,0,1,0)));
                    p->code = merge_code(p->code,generate_code(CODE_ASSIGN,p->store_place,temp3));
                    p->store_place = temp3;
                }
            } else {
                translate_Exp(get_child_node(p,1),table);
                translate_Exp(get_child_node(p,3),table);
                temp = new_op_temp();
                if(get_child_node(p,3)->gno == 16){
                    p->code = generate_code(CODE_BINOP,temp,set_oprand(get_child_node(p,1)->store_place,1,0,0),new_op_constant(4*get_child_node(p,3)->child_node->val),BOP_ADD);
                    temp2 = new_op_temp();
                    p->code = merge_code(p->code,generate_code(CODE_ASSIGN,temp2,set_oprand(temp,0,1,0)));
                    p->store_place = temp2;
                }
                else{
                    p->code = get_child_node(p,3)->code;
                    p->code = merge_code(p->code,generate_code(CODE_BINOP,temp,new_op_constant(4),get_child_node(p,3)->store_place,BOP_MUL));
                    temp2 = new_op_temp();
                    p->code = merge_code(p->code,generate_code(CODE_BINOP,temp2,set_oprand(get_child_node(p,1)->store_place,1,0,0),temp,BOP_ADD));
                    temp3 = new_op_temp();
                    p->code = merge_code(p->code,generate_code(CODE_ASSIGN,temp3,set_oprand(temp2,0,1,0)));
                    p->store_place = temp3;
                }
            }
        }
        break;
    case 14:
        assert(0);break;
    case 15:
        if(p->store_place != NULL)
            p->code = generate_code(CODE_ASSIGN,p->store_place,variable_lookup(table,get_child_node(p,1)->idname));
        p->store_place = variable_lookup(table,get_child_node(p,1)->idname);
        break;
    case 16:
        temp = new_op_constant(get_child_node(p,1)->val);
        if(p->store_place != NULL)
            p->code =  generate_code(CODE_ASSIGN,p->store_place,temp);
        p->store_place = temp;
        break;
    case 17:
        assert(0);
        break;
    default:
        assert(0);
        break;
    }
    
}

void translate_Dec (struct tree_node * p,Variable_table * table){
    switch (p->gno) {
    case 0:
        translate_VarDec(get_child_node(p,1),table);
        p->code = get_child_node(p,1)->code;break;
    case 1:
        translate_VarDec(get_child_node(p,1),table);
        get_child_node(p,3)->store_place =  get_child_node(p,1)->store_place;
        translate_Exp(get_child_node(p,3),table);
        p->code = merge_child_code(p,1,3);break;
    default:
        assert(0);
    }
}
void translate_DecList (struct tree_node * p,Variable_table * table){
    switch (p->gno) {
    case 0:
        translate_Dec(get_child_node(p,1),table);
        p->code = get_child_node(p,1)->code;break;
    case 1:
        translate_Dec(get_child_node(p,1),table);
        translate_DecList(get_child_node(p,3),table);
        p->code = merge_child_code(p,1,3);break;
    default:
        assert(0);
    }
}
void translate_Def (struct tree_node * p,Variable_table * table){
    translate_DecList(get_child_node(p,2),table);
    p->code = get_child_node(p,2)->code;
}
void translate_DefList (struct tree_node * p,Variable_table * table){
    switch (p->gno) {
    case 0:
        translate_Def(get_child_node(p,1),table);
        translate_DefList(get_child_node(p,2),table);
        p->code = merge_child_code(p,1,2);break;
    case 1:
        break;
    default:
        assert(0);
    }
}
void translate_Stmt (struct tree_node * p,Variable_table * table){
    Oprand * l1,* l2,* l3,* temp;
    switch (p->gno) {
    case 0:
        translate_Exp(get_child_node(p,1),table);
        p->code = get_child_node(p,1)->code;break;
    case 1:
        translate_CompSt(get_child_node(p,1),table);
        p->code = get_child_node(p,1)->code;break;
    case 2:
        translate_Exp(get_child_node(p,2),table);
        p->code = get_child_node(p,2)->code;
        if( get_child_node(p,2)->store_place->get_val == 1){
            temp = new_op_temp();
            p->code = merge_code(p->code,generate_code(CODE_ASSIGN,temp,get_child_node(p,2)->store_place));
            p->code = merge_code(p->code,generate_code(CODE_RETURN,temp));
        } else {
            p->code = merge_code(p->code,generate_code(CODE_RETURN,get_child_node(p,2)->store_place));
        }
        break;
    case 3:
        l1 = new_op_label();
        l2 = new_op_label();
        translate_Cond(get_child_node(p,3),l1,l2,table);
        translate_Stmt(get_child_node(p,5),table);
        p->code = merge_code(get_child_node(p,3)->code,generate_code(CODE_LABEL,l1));
        p->code = merge_code(p->code,get_child_node(p,5)->code);
        p->code = merge_code(p->code,generate_code(CODE_LABEL,l2));break;
    case 4:
        l1 = new_op_label();
        l2 = new_op_label();
        l3 = new_op_label();
        translate_Cond(get_child_node(p,3),l1,l2,table);
        translate_Stmt(get_child_node(p,5),table);
        translate_Stmt(get_child_node(p,7),table);
        p->code = merge_code(get_child_node(p,3)->code,generate_code(CODE_LABEL,l1));
        p->code = merge_code(p->code,get_child_node(p,5)->code);
        p->code = merge_code(p->code,generate_code(CODE_JMP,l3));
        p->code = merge_code(p->code,generate_code(CODE_LABEL,l2));
        p->code = merge_code(p->code,get_child_node(p,7)->code);
        p->code = merge_code(p->code,generate_code(CODE_LABEL,l3));break;
    case 5:
        l1 = new_op_label();
        l2 = new_op_label();
        l3 = new_op_label();
        translate_Cond(get_child_node(p,3),l2,l3,table);
        translate_Stmt(get_child_node(p,5),table);
        p->code = generate_code(CODE_LABEL,l1);
        p->code = merge_code(p->code,get_child_node(p,3)->code);
        p->code = merge_code(p->code,generate_code(CODE_LABEL,l2));
        p->code = merge_code(p->code,get_child_node(p,5)->code);
        p->code = merge_code(p->code,generate_code(CODE_JMP,l1));
        p->code = merge_code(p->code,generate_code(CODE_LABEL,l3));break;
    default:
        assert(0);
    }
}
void translate_StmtList (struct tree_node * p,Variable_table * table){
    switch (p->gno) {
    case 0:
        translate_Stmt(get_child_node(p,1),table);
        translate_StmtList(get_child_node(p,2),table);
        p->code = merge_code(get_child_node(p,1)->code,get_child_node(p,2)->code);break;
    case 1:
        break;
    default:
        assert(0);
    }
}
void translate_CompSt (struct tree_node * p,Variable_table * table){
    translate_DefList(get_child_node(p,2),table);
    translate_StmtList(get_child_node(p,3),table);
    p->code = merge_code(get_child_node(p,2)->code,get_child_node(p,3)->code);
}
void translate_ParamDec (struct tree_node * p,Variable_table * table){
    translate_VarDec(get_child_node(p,2),table);p->code = get_child_node(p,2)->code;
    p->code = merge_code(p->code,generate_code(CODE_PARAM,get_child_node(p,2)->store_place));
}
void translate_VarList (struct tree_node * p,Variable_table * table){ // **** 需要修改 *** //
    switch (p->gno) {
    case 0:
        translate_ParamDec(get_child_node(p,1),table);
        translate_VarList(get_child_node(p,3),table);
        p->code = merge_code(get_child_node(p,1)->code,get_child_node(p,3)->code);break;
    case 1:
        translate_ParamDec(get_child_node(p,1),table);p->code = get_child_node(p,1)->code;break;
    default:
        assert(0);
    }
}
void translate_FunDec (struct tree_node * p,Variable_table * table){
    Oprand * f;
    switch (p->gno) {
    case 0:
        f = new_op_function(get_child_node(p,1)->idname);
        p->code = generate_code(CODE_FUNCTION,f);
        translate_VarList(get_child_node(p,3),table);
        p->code = merge_code(p->code,get_child_node(p,3)->code);
        break;
    case 1:
        f = new_op_function(get_child_node(p,1)->idname);
        p->code = generate_code(CODE_FUNCTION,f);
        break;
    default:
        assert(0);
    }
}
void translate_VarDec (struct tree_node * p,Variable_table * table){ 
    Oprand * size;
    switch (p->gno) {
    case 0:
        variable_insert(table,get_child_node(p,1)->idname);
        p->store_place = variable_lookup(table,get_child_node(p,1)->idname); break;
    case 1:
        if(!(get_child_node(p,1)->child_node->type == 0 && strcmp(get_child_node(p,1)->child_node->name,"ID") == 0))
            assert(0);
        variable_insert(table,get_child_node(p,1)->child_node->idname);
        size = new_op_constant(get_child_node(p,3)->val*4);
        p->code = generate_code(CODE_MEMDEC,variable_lookup(table,get_child_node(p,1)->child_node->idname),size);
        p->store_place = variable_lookup(table,get_child_node(p,1)->child_node->idname);break;
    default:
        assert(0);
    }
}
void translate_ExtDecList (struct tree_node * p,Variable_table * table){
    switch (p->gno) {
    case 0:
        translate_VarDec(get_child_node(p,1),table);p->code = get_child_node(p,1)->code; break;
    case 1:
        translate_VarDec(get_child_node(p,1),table);translate_ExtDecList(get_child_node(p,3),table);
        p->code = merge_code(get_child_node(p,1)->code,get_child_node(p,3)->code) ;break;
    default:
        assert(0);
    }
}
void translate_ExtDef (struct tree_node * p,Variable_table * table){
    switch (p->gno) {
    case 0:
        translate_ExtDecList(get_child_node(p,2),table);p->code = get_child_node(p,2)->code;break;
    case 1:
        break;
    case 2:
        translate_FunDec(get_child_node(p,2),table);translate_CompSt(get_child_node(p,3),table);
        p->code = merge_code(get_child_node(p,2)->code,get_child_node(p,3)->code) ;break;
    default:
        assert(0);
    }
}
void translate_ExtDefList (struct tree_node * p,Variable_table * table){
    switch (p->gno){
        case 0:
            translate_ExtDef(get_child_node(p,1),table);translate_ExtDefList(get_child_node(p,2),table);
            p->code = merge_code(get_child_node(p,1)->code,get_child_node(p,2)->code);break;
        case 1:
            break;
        default:
            assert(0);
        }
}
void translate_Program (struct tree_node * p,Variable_table * table){
    translate_ExtDefList(get_child_node(p,1),table);
    p->code = get_child_node(p,1)->code;
    print_code(Code_output_file,p->code);
    Global_intercode = p->code;
}