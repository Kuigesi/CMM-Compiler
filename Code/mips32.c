#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include"tree.h" 
#include<assert.h>
void asm_code(FILE *stream,Intercode * code);
void asm_code_generate(FILE *stream,Intercode * code);
int op_equal_check(Oprand * op1,Oprand * op2){
    if(op1->kind != op2->kind)
        return 0;
    switch (op1->kind){
    case OP_VARIABLE:
        if (op1->u.var_no == op2->u.var_no)
            return 1;
        else 
            return 0;
        break;
    case OP_TEMP:
        if (op1->u.temp_no == op2->u.temp_no)
            return 1;
        else 
            return 0;
        break;
    default:
        assert(0);
        break;
    }
    return 0;
}
int op_offset_lookup(Oprand * op){
    if(Function_op_offset_table == NULL)
        assert(0);
    //print_oprand(stdout,op);
    //printf("%d\n",op->kind);
    if ((op->kind != 0)&&(op->kind != 6))
        assert(0);
    op_offset_map * temp1,*temp2;
    temp1 = Function_op_offset_table;
    temp2 = temp1->next;
    while(temp2 != NULL){
        if(op_equal_check(temp1->op,op) == 1)
            return temp1->offset;
        temp1 = temp2;
        temp2 = temp1->next;
    }
    if(op_equal_check(temp1->op,op) == 1)
        return temp1->offset;
    assert(0);
}
void op_offset_insert(Oprand * op,int size){
    if((op->kind == 1)||(op->get_addr == 1))
        return;
    if ((op->kind != 0)&&(op->kind != 6))
        assert(0);
    if (op->get_val == 1)
        assert(0);
    op_offset_map * temp1,*temp2;
    op_offset_map * new_temp;
    if(Function_op_offset_table == NULL){
        new_temp = (op_offset_map *)malloc(sizeof(op_offset_map));
        new_temp->next = NULL;
        new_temp->op = op;
        new_temp->offset = -(8 + size);
        Function_op_offset_table = new_temp;
        return;
    }
    temp1 = Function_op_offset_table;
    temp2 = temp1->next;
    while(temp2 != NULL){
        if(op_equal_check(temp1->op,op) == 1)
            return;
        temp1 = temp2;
        temp2 = temp1->next;
    }
    if(op_equal_check(temp1->op,op) == 1)
        return;
    new_temp = (op_offset_map *)malloc(sizeof(op_offset_map));
    new_temp->next = NULL;
    new_temp->op = op;
    new_temp->offset = temp1->offset - size;
    temp1->next = new_temp;
    return;
}
void print_offset(){
    op_offset_map * temp1;
    temp1 = Function_op_offset_table;
    while(temp1 != NULL){
        print_oprand(stdout,temp1->op);
        printf(" offset: %d\n",temp1->offset);
        temp1 = temp1->next;
    }
    printf("\n");
}
int stack_size(){
    op_offset_map * temp1;
    int off_size;
    temp1 = Function_op_offset_table;
    off_size = 8;
    while(temp1 != NULL){
        off_size = -temp1->offset;
        temp1 = temp1->next;
    }
    return off_size;
}
void stack_offset_find(FILE *stream,Intercode * code){
    Function_op_offset_table = NULL;
    if(code == NULL)
        return;
    int cno = code->kind;
    if(cno != CODE_FUNCTION)
        assert(0);
    //printf("FUNCTION: %s\n",code->u.function_def.f->u.name);
    Intercode * temp_code = code->next;
    Intercode * function_start = temp_code;
    while(temp_code != NULL){
        if (temp_code->kind == CODE_FUNCTION)
            break;
        switch (temp_code->kind) {
        case CODE_ASSIGN:
            if(temp_code->u.assign.result->get_val != 1)
                op_offset_insert(temp_code->u.assign.result,4);
            if(temp_code->u.assign.op1->get_val != 1)
                op_offset_insert(temp_code->u.assign.op1,4);
            break;
        case CODE_BINOP:
            op_offset_insert(temp_code->u.binop.result,4);op_offset_insert(temp_code->u.binop.op1,4);op_offset_insert(temp_code->u.binop.op2,4);
            break;
        case CODE_CONDJMP:
            op_offset_insert(temp_code->u.condjmp.x,4);op_offset_insert(temp_code->u.condjmp.y,4);
            break;
        case CODE_RETURN:
            op_offset_insert(temp_code->u.return_val.x,4);
            break;
        case CODE_MEMDEC:
            op_offset_insert(temp_code->u.memdec.x,temp_code->u.memdec.size->u.value);
            break;
        case CODE_ARG:
            op_offset_insert(temp_code->u.arg.x,4);
            break;
        case CODE_CALL:
            op_offset_insert(temp_code->u.function_call.x,4);
            break;
        case CODE_PARAM:
            op_offset_insert(temp_code->u.param.x,4);
            break;
        case CODE_READ:
            op_offset_insert(temp_code->u.read.x,4);
            break;
        case CODE_WRITE:
            op_offset_insert(temp_code->u.write.x,4);
            break;
        default:
            break;
        }
        temp_code = temp_code->next;
    }
    //print_offset();
    return;
} 
void op_to_reg(FILE * stream,Oprand * op,int regno){
    if(op->kind == 1)
        fprintf(stream,"  li $t%d, %d\n",regno,op->u.value);
    else {
        if((op->kind != 0)&&(op->kind != 6))
            assert(0);
        fprintf(stream,"  lw $t%d, %d($fp)\n",regno,op_offset_lookup(op));
    }
    return;
}
void return_op_to_reg(FILE * stream,Oprand * op){
    if(op->kind == 1)
        fprintf(stream,"  li $v0, %d\n",op->u.value);
    else {
        if((op->kind != 0)&&(op->kind != 6))
            assert(0);
        fprintf(stream,"  lw $v0, %d($fp)\n",op_offset_lookup(op));
    }
    return;
}
void arg_op_to_reg(FILE * stream,Oprand * op,int regno){
    if(op->kind == 1)
        fprintf(stream,"  li $a%d, %d\n",regno,op->u.value);
    else {
        if((op->kind != 0)&&(op->kind != 6))
            assert(0);
        fprintf(stream,"  lw $a%d, %d($fp)\n",regno,op_offset_lookup(op));
    }
    return;
}
int max_num(int i,int j){
    if(i > j)
        return i;
    return j;
}
Intercode *  asm_param_code_generate(FILE * stream,Intercode * code){
    if(code->kind != CODE_PARAM)
        assert(0);
    if(code->u.param.x->get_val == 1)
        assert(0);
    Intercode * temp,*old_temp;
    temp = code;
    int param_no = 0;
    while (temp != NULL){
        if(temp->kind != CODE_PARAM)
            break;
        param_no = param_no + 1;
        if(param_no < 5)
            fprintf(stream,"  sw $a%d, %d($fp)\n",param_no-1,op_offset_lookup(temp->u.param.x));
        else{
            fprintf(stream,"  lw $t0, %d($fp)\n",4*(param_no -5));
            fprintf(stream,"  sw $t0, %d($fp)\n",op_offset_lookup(temp->u.param.x));
        }
        old_temp = temp;
        temp = temp->next;
    }
    return old_temp;
}
void asm_arg_code_generate(FILE * stream,Intercode * code,int arg_no,int arg_start){ // notice //
    if(code->kind != CODE_ARG)
        assert(0);
    if(code->u.arg.x->get_val == 1)
        assert(0);
    if(arg_start == 1){
        if(arg_no > 4)
            fprintf(stream,"  subu $sp, $sp, %d\n",4*(arg_no - 4));
    }
    switch (arg_no){
    case 0:
        assert(0);break;
    case 1:
    case 2:
    case 3:
    case 4:
        arg_op_to_reg(stream,code->u.arg.x,arg_no - 1);
        break;
    default:
        op_to_reg(stream,code->u.arg.x,0);
        fprintf(stream,"  sw $t0, %d($sp)\n",4*(arg_no - 5));
        break;
    }
}
void asm_call_code_generate(FILE * stream,Intercode * code,int arg_total_num){ // notice //
    if(code->kind != CODE_CALL)
        assert(0);
    if(code->u.function_call.x->get_val == 1)
        assert(0);
    fprintf(stream,"  jal %s\n",code->u.function_call.f->u.name);
    fprintf(stream,"  sw $v0, %d($fp)\n",op_offset_lookup(code->u.function_call.x));
    if(arg_total_num > 4)
        fprintf(stream,"  addi $sp, $sp, %d\n",4*(arg_total_num - 4));
}
int arg_num(Intercode * code){
    Intercode * temp;
    temp = code;
    int num = 0;
    while(temp != NULL){
        if(temp->kind != CODE_ARG){
            if(temp->kind != CODE_CALL)
                assert(0);
            return num;
        }
        num = num + 1;
        temp = temp->next;
    }
    return num;
}
void asm_condjmp_code_generate(FILE * stream,Intercode * code){
    if(code->kind != CODE_CONDJMP)
        assert(0);
    if((code->u.condjmp.x->get_val == 1)||(code->u.condjmp.y->get_val == 1))
        assert(0);
    op_to_reg(stream,code->u.condjmp.x,0);
    op_to_reg(stream,code->u.condjmp.y,1);
    switch (code->u.condjmp.relop) {
    case RLP_EQ:
        fprintf(stream,"  beq ");break;
    case RLP_NEQ:
        fprintf(stream,"  bne ");break;
    case RLP_G:
        fprintf(stream,"  bgt ");break;
    case RLP_L:
        fprintf(stream,"  blt ");break;
    case RLP_GEQ:
        fprintf(stream,"  bge ");break;
    case RLP_LEQ:
        fprintf(stream,"  ble ");break;
    default:
        assert(0);break;
    }
    fprintf(stream,"$t0, $t1, label%d\n",code->u.condjmp.des->u.label_no);
    return;
}
void asm_binop_code_generate(FILE * stream,Intercode * code){
    if(code->kind != CODE_BINOP)
        assert(0);
    if((code->u.binop.result->get_val == 1)||(code->u.binop.op1->get_val == 1)||(code->u.binop.op2->get_val == 1))
        assert(0);
    switch (code->u.binop.bop){
    case BOP_ADD:
        if(code->u.binop.op2->kind == 1){// notice addr //
            if(code->u.binop.op1->get_addr == 1){
                fprintf(stream,"  move $t1, $fp\n");
                fprintf(stream,"  addi $t1, $t1, %d\n",op_offset_lookup(code->u.binop.op1));
            } else 
                op_to_reg(stream,code->u.binop.op1,1);
            fprintf(stream,"  addi $t0, $t1, %d\n",code->u.binop.op2->u.value);
            fprintf(stream,"  sw $t0, %d($fp)\n",op_offset_lookup(code->u.binop.result));
        } else{
            if(code->u.binop.op1->get_addr == 1){
                fprintf(stream,"  move $t1, $fp\n");
                fprintf(stream,"  addi $t1, $t1, %d\n",op_offset_lookup(code->u.binop.op1));
            } else 
                op_to_reg(stream,code->u.binop.op1,1);
            fprintf(stream,"  lw $t2, %d($fp)\n",op_offset_lookup(code->u.binop.op2));
            fprintf(stream,"  add $t0, $t1, $t2\n");
            fprintf(stream,"  sw $t0, %d($fp)\n",op_offset_lookup(code->u.binop.result));
        }
        break;
    case BOP_SUB:
        if(code->u.binop.op2->kind == 1){// notice addr //
            if(code->u.binop.op1->get_addr == 1){
                fprintf(stream,"  move $t1, $fp\n");
                fprintf(stream,"  addi $t1, $t1, %d\n",op_offset_lookup(code->u.binop.op1));
            } else 
                op_to_reg(stream,code->u.binop.op1,1);
            fprintf(stream,"  addi $t0, $t1, %d\n",-code->u.binop.op2->u.value);
            fprintf(stream,"  sw $t0, %d($fp)\n",op_offset_lookup(code->u.binop.result));
        } else{
            if(code->u.binop.op1->get_addr == 1){
                fprintf(stream,"  move $t1, $fp\n");
                fprintf(stream,"  addi $t1, $t1, %d\n",op_offset_lookup(code->u.binop.op1));
            } else 
                op_to_reg(stream,code->u.binop.op1,1);
            fprintf(stream,"  lw $t2, %d($fp)\n",op_offset_lookup(code->u.binop.op2));
            fprintf(stream,"  sub $t0, $t1, $t2\n");
            fprintf(stream,"  sw $t0, %d($fp)\n",op_offset_lookup(code->u.binop.result));
        }
        break;
    case BOP_MUL:
        op_to_reg(stream,code->u.binop.op1,1);
        op_to_reg(stream,code->u.binop.op2,2);
        fprintf(stream,"  mul $t0, $t1, $t2\n");
        fprintf(stream,"  sw $t0, %d($fp)\n",op_offset_lookup(code->u.binop.result));
        break;
    case BOP_DIV:
        op_to_reg(stream,code->u.binop.op1,1);
        op_to_reg(stream,code->u.binop.op2,2);
        fprintf(stream,"  div $t1, $t2\n");
        fprintf(stream,"  mflo $t0\n");
        fprintf(stream,"  sw $t0, %d($fp)\n",op_offset_lookup(code->u.binop.result));
        break;
    default:
        assert(0);
        break;
    }
}
void asm_assign_code_generate(FILE * stream,Intercode * code){
    if(code->kind != CODE_ASSIGN)
        assert(0);
    if(code->u.assign.result->get_val == 1){
        if(code->u.assign.op1->get_val == 1)
            assert(0);
        if(code->u.assign.op1->kind == 1){
            fprintf(stream,"  lw $t0, %d($fp)\n",op_offset_lookup(code->u.assign.result));
            fprintf(stream,"  li $t1, %d\n",code->u.assign.op1->u.value);
            fprintf(stream,"  sw $t1, 0($t0)\n");
        } else {
            fprintf(stream,"  lw $t0, %d($fp)\n",op_offset_lookup(code->u.assign.result));
            fprintf(stream,"  lw $t1, %d($fp)\n",op_offset_lookup(code->u.assign.op1));
            fprintf(stream,"  sw $t1, 0($t0)\n");
        }
    } else if(code->u.assign.op1->get_val == 1){
        fprintf(stream,"  lw $t1, %d($fp)\n",op_offset_lookup(code->u.assign.op1));
        fprintf(stream,"  lw $t0, 0($t1)\n");
        fprintf(stream,"  sw $t0, %d($fp)\n",op_offset_lookup(code->u.assign.result));
    } else {
        if(code->u.assign.op1->kind == 1){
            fprintf(stream,"  li $t0, %d\n",code->u.assign.op1->u.value);
            fprintf(stream,"  sw $t0, %d($fp)\n",op_offset_lookup(code->u.assign.result));
        } else {
            fprintf(stream,"  lw $t0, %d($fp)\n",op_offset_lookup(code->u.assign.op1));
            fprintf(stream,"  sw $t0, %d($fp)\n",op_offset_lookup(code->u.assign.result));
        }
    }
}
void asm_code_generate(FILE *stream,Intercode * code){
    //printf("yes\n");
    fprintf(stream,".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n");
    fprintf(stream,"read:\n  li $v0, 4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n\n");
    fprintf(stream,"write:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n");
    int arg_no = 0,arg_start = 1,arg_total_num = 0;
    while(code != NULL){
        //printf("code :%d\n",code->kind);
        switch (code->kind) {
        case CODE_LABEL:
            fprintf(stream,"label%d:\n",code->u.label.l->u.label_no);break;
        case CODE_FUNCTION:
            stack_offset_find(stream,code);
            fprintf(stream,"\n");
            fprintf(stream,"%s:\n",code->u.function_def.f->u.name);
            fprintf(stream,"  subu $sp, $sp, %d\n",stack_size());
            fprintf(stream,"  sw $ra, %d($sp)\n",stack_size()-4);
            fprintf(stream,"  sw $fp, %d($sp)\n",stack_size()-8);
            fprintf(stream,"  addi $fp, $sp, %d\n",stack_size());
            //printf("size:%d\n",stack_size());
            break;
        case CODE_ASSIGN:
            asm_assign_code_generate(stream,code);break;
        case CODE_BINOP:
            asm_binop_code_generate(stream,code);break;
        case CODE_JMP:
            fprintf(stream,"  j label%d\n",code->u.jmp.des->u.label_no);break;
        case CODE_CONDJMP:
            asm_condjmp_code_generate(stream,code);break;
        case CODE_RETURN:
            return_op_to_reg(stream,code->u.return_val.x);
            fprintf(stream,"  lw $ra, %d($sp)\n",stack_size()-4);
            fprintf(stream,"  lw $fp, %d($sp)\n",stack_size()-8);
            fprintf(stream,"  addi $sp, $sp, %d\n",stack_size());
            fprintf(stream,"  jr $ra\n");break;
        case CODE_MEMDEC:
            break;
        case CODE_ARG:
            if(arg_no == 0){
                arg_total_num = arg_no = arg_num(code);
            }
            asm_arg_code_generate(stream,code,arg_no,arg_start);
            arg_no = arg_no - 1;
            arg_start = 0;
            break;
        case CODE_CALL:
            arg_no = 0;arg_start = 1;
            asm_call_code_generate(stream,code,arg_total_num);
            arg_total_num = 0;
            break;
        case CODE_PARAM:// to be implemented //
            code = asm_param_code_generate(stream,code);
            break;
        case CODE_READ:
            fprintf(stream,"  jal read\n");
            fprintf(stream,"  sw $v0, %d($fp)\n",op_offset_lookup(code->u.read.x));
            break;
        case CODE_WRITE:
            if(code->u.write.x->kind == 1)
                fprintf(stream,"  li $a0, %d\n",code->u.write.x->u.value);
            else 
                fprintf(stream,"  lw $a0, %d($fp)\n",op_offset_lookup(code->u.write.x));
            fprintf(stream,"  jal write\n");
            break;
        default:
            assert(0);
            break;
        }
        code = code->next;
    }
    return;
}