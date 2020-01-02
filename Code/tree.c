#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include"tree.h" 
#include<assert.h>
void add_node(int num,...) {
	//printf("good\n");
	va_list valist;
	struct tree_node * p =(struct tree_node *)malloc(sizeof(struct tree_node));
	struct tree_node * ch =(struct tree_node *)malloc(sizeof(struct tree_node));
	//printf("num : %d\n",num);
        if(num < 2)
		assert(0);
	va_start(valist,num);
	p = va_arg(valist, struct tree_node*);
	//printf("good\n");
	//printf("0x%x\n",p);
	//printf("  NT %s\n",p->name);
	ch = va_arg(valist, struct tree_node*);
	//printf("  NT %s\n",p->name);
	p->child_node = ch;
	for(int i=0;i<num-2;i++){
		ch->bro_node = va_arg(valist, struct tree_node*);
		ch = ch->bro_node;
	}
	ch->bro_node = NULL;
	return;
}
void pstree_t(struct tree_node * p,int indent) {
	if(p) {
		for(int i = 0; i<indent;i++){
			printf(" ");
		}
		printf("%s (%d)\n",p->name,p->line);
		struct tree_node * temp = p->child_node;
		while(temp){
			pstree(temp,indent+2);
			temp = temp->bro_node;
		}
	} else {
		assert(0);
	}
	return;

}
void pstree(struct tree_node * p,int indent) {
	if(p) { if(p->type!=2){
		for(int i = 0; i<indent;i++){
				printf(" ");
		}
		//printf("%d",indent);
		if(p->type == 0) {
			printf("%d ",p->gno);
			if((!strcmp(p->name,"ID"))||(!strcmp(p->name,"TYPE"))){
				printf("%s: %s\n",p->name,p->idname);
			} else {
			if(!strcmp(p->name,"INT")){
				printf("%s: %d\n",p->name,p->val);
			} else {
			if(!strcmp(p->name,"FLOAT")){
				printf("%s: %lf\n",p->name,p->dval);
			} else {
				printf("%s\n",p->name);
			}
			}
			}
		}
		if(p->type == 1) {
			printf("%d ",p->gno);
			printf("%s (%d)\n",p->name,p->line);
		}
		if((p->type > 2) || (p->type < 0))
			assert(0);
		struct tree_node * temp = p->child_node;
		while(temp){
			pstree(temp,indent+2);
			temp = temp->bro_node;
		}
	} else {

	}
	} else {
		assert(0);
	}
	return;
}
