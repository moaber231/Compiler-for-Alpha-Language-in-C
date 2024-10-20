#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic

#define execute_jle execute_compare
#define execute_jge execute_compare
#define execute_jlt execute_compare
#define execute_jgt execute_compare

#include "avm.h"
extern int currInstruction ;
extern double* numConsts;
extern unsigned totalNumConsts;
extern char** stringConsts;
extern unsigned totalStringConsts;
extern char** namedLibfuncs;
extern unsigned totalNamedLibfuncs;
extern userfunc* userFuncs;
extern unsigned totalUserFuncs;
static void red()
{
    printf("\033[1;31m");
}

static void green()
{
    printf("\033[1;32m");
}

static void blue()
{
    printf("\033[1;34m");
}

static void yellow()
{
    printf("\033[1;33m");
}

static void orange()
{
    printf("\033[38;5;208m");
}
static void reset()
{
    printf("\033[0m");
}
static void purple()
{
    printf("\033[1;35m");
}

static void grey()
{
    printf("\033[1;30m");
}

double consts_getnumber (unsigned index)
{
	return numConsts[index];
}
char* consts_getstring (unsigned index)
{
	return stringConsts[index];
}
char* libfuncs_getused (unsigned index)
{
	return namedLibfuncs[index];
}
userfunc* userfuncs_getfunc (unsigned index)
{
	return &userFuncs[index];
}
unsigned avm_totalactuals (void){
	return avm_get_envvalue(topsp + AVM_NUMACTUALS_OFFSET);
}

avm_memcell* avm_getactual(unsigned i)
{
	assert(i < avm_totalactuals());
	return &stack[topsp + AVM_STACKENV_SIZE + 1 + i];
}

/* Implementation of the library function 'print'
	it displays every argument at the console.
*/

void libfunc_print(void)
{
	unsigned n = avm_totalactuals();
	for(unsigned i = 0; i < n; i++)
	{
		char *s = avm_tostring(avm_getactual(i));
		puts(s);
		//free(s);
	}
}
/* with the following every library function is manually
added int the VM library function resolution map.
*/
int counter_libr=0;
map_library_funcs* libraryFuncs;
void insert_libr(char *key,library_func_t value)
{
	counter_libr++;
	map_library_funcs * temp=NULL;
	temp= realloc(libraryFuncs,counter_libr*(sizeof(map_library_funcs)));
	libraryFuncs=temp;
	libraryFuncs[counter_libr-1].key=strdup(key);
	libraryFuncs[counter_libr-1].value=value;
}
library_func_t  lookup_libr(char *key)
{
	for(int i=0; i<counter_libr; i++)
	{
		if(strcmp(libraryFuncs[i].key,key)==0)
		{
			return libraryFuncs[i].value;
		}
	}
	return NULL;
}
void avm_registerlibfunc(char* id ,library_func_t addr)
{
	insert_libr(id,addr);
}

typedef char* (*tostring_func_t)(avm_memcell*);

char* number_tostring(avm_memcell* m)
{
	char *s=(char*)malloc(sizeof(char*));
	sprintf(s,"%.3f",m->data.numVal);
	return s;
}
 char* string_tostring(avm_memcell* m)
 {
 	return m->data.strVal;
 }
 char* bool_tostring(avm_memcell* m)
 {
 	if(m->data.boolVal==1)return "true";
 	else return "false";
 }
 char* table_tostring(avm_memcell* m)
 {
 	return NULL;
 }
 char* userfunc_tostring(avm_memcell* m)
 {
 	return userFuncs[m->data.funcVal].id;
 }
 char* libfunc_tostring(avm_memcell* m )
 {
 	return m->data.libfuncVal;
 }
 char* nil_tostring(avm_memcell* m)
 {
 	return "nil";
 }
 char* undef_tostring(avm_memcell* m)
 {
 	return "undef";
 }
void avm_tableincrefcounter (avm_table* t)
{++t->refCounter; }
void avm_tabledestroy (avm_table* t)
{
	avm_tablebucketsdestroy(t->strIndexed);
	avm_tablebucketsdestroy(t->numIndexed);
	free(t);
}
void avm_tabledecrefcounter (avm_table* t){
	assert(t->refCounter > 0);
	if(!--t->refCounter)
		avm_tabledestroy(t);
}
void avm_tablebucketsinit(avm_table_bucket** p){
	for(unsigned i=0; i<AVM_TABLE_HASHSIZE; ++i)
	p[i] = (avm_table_bucket*) 0;
}
avm_table* avm_tablenew (void){
	avm_table* t = (avm_table*)malloc(sizeof(avm_table));
	AVM_WIPEOUT(*t);
	
	t->refCounter = t->total =0;
	avm_tablebucketsinit(t->numIndexed);
	avm_tablebucketsinit(t->strIndexed);
	
	return t;
}
void avm_tablebucketsdestroy(avm_table_bucket** p){
	for(unsigned i=0; i<AVM_TABLE_HASHSIZE; ++i,++p){
		for(avm_table_bucket* b = *p; b;){
			avm_table_bucket* del = b;
			b=b->next;
			avm_memcellclear(&del->key);
			avm_memcellclear(&del->value);
			free(del);
		}
		p[i]=(avm_table_bucket*) 0;
	}
}
extern void avm_push_table_arg(avm_table* t)
{
	stack[top].type = table_m;
	avm_tableincrefcounter(stack[top].data.tableVal=t);
	++totalActuals;
	avm_dec_top();
}

void execute_pusharg(instruction* instr)
{
	avm_memcell * arg = avm_translate_operand(&instr->arg1,&ax);
	assert(arg);
	avm_assign(&stack[top],arg);
	++totalActuals;
	avm_dec_top();
}

tostring_func_t tostringFuncs[]={
	number_tostring,
	string_tostring,
	bool_tostring,
	table_tostring,
	userfunc_tostring,
	libfunc_tostring,
	nil_tostring,
	undef_tostring
};

char* avm_tostring (avm_memcell* m)
{
	assert(m->type >= 0 && m->type <= undef_m);
	return (*tostringFuncs[m->type]) (m);
}

typedef double (*arithmetic_func_t)(double x,double y);

double add_impl(double x, double y){return x+y;}
double sub_impl(double x, double y){return x-y;}
double mul_impl(double x, double y){return x*y;}
double div_impl(double x, double y){if(y==0){printf("ERROR div with zero\n");exit(EXIT_FAILURE);}else {return x/y;}}
double mod_impl(double x, double y){if(y==0){printf("ERROR div with zero\n");exit(EXIT_FAILURE);}else {return ((unsigned)x)%((unsigned)y);}}

arithmetic_func_t arithmeticFuncs[]={
	add_impl,
	sub_impl,
	mul_impl,
	div_impl,
	mod_impl
};

void execute_arithmetic(instruction* instr)
{
	avm_memcell* lv = avm_translate_operand(&instr->result,(avm_memcell*)0);
	avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
	avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);
	//assert(lv && (&stack[N-1] >= lv && lv > &stack[top] || lv==&retval));
	assert(rv1 && rv2);

	if(rv1->type != number_m || rv2->type!=number_m)
	{
		avm_error("not a number in arithmetic!");
		executionFinished=1;
	}
	else
	{
		arithmetic_func_t op = arithmeticFuncs[instr->opcode - add_v];
		avm_memcellclear(lv);
		lv->type = number_m;
		lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
	}
}
void execute_jump(instruction*t)
{
	pc=t->result.val;
}
void execute_compare(instruction* instr)
{
	avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
	avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);

	assert(rv1 && rv2);
	bool r=false;
	if(!(	(rv1->type ==number_m || rv1->type ==bool_m)  && (rv2->type==number_m || rv2->type==bool_m)	) )
	{
		avm_error("not a number in compare");
	}
	else
	{
		unsigned n1,n2;
		if(rv1->type==bool_m)
		{
			n1=rv1->data.boolVal;
		}
		else
		{
			n1=rv1->data.numVal;
		}
		if(rv2->type==bool_m)
		{
			n2=rv2->data.boolVal;
		}
		else
		{
			n2=rv2->data.numVal;
		}
		
		if(instr->opcode == jge_v)
		{
			r=n1>=n2;
		}
		else if( instr -> opcode == jgt_v)
		{
			r=n1>n2;
		}
		else if( instr -> opcode == jle_v)
		{
			r=n1<=n2;
		}
		else if( instr->opcode == jlt_v)
		{
			r=n1<n2;
		}
		else 
			avm_error("wrong call of execute compare");
	}
	if(!executionFinished && r)
		pc = instr->result.val;
	return;
}
typedef unsigned char (*tobool_func_t)(avm_memcell*);

unsigned char number_tobool (avm_memcell *m){return m->data.numVal !=0;}
unsigned char string_tobool (avm_memcell *m){return m->data.strVal[0]!=0;}
unsigned char bool_tobool (avm_memcell *m){return m->data.boolVal;}
unsigned char table_tobool (avm_memcell *m){return 1;}
unsigned char userfunc_tobool (avm_memcell *m){return 1;}
unsigned char libfunc_tobool (avm_memcell *m){return 1;}
unsigned char nil_tobool (avm_memcell *m) {return 0;}
unsigned char undef_tobool (avm_memcell *m) {assert(0);return 0;}

tobool_func_t toboolFuncs[]={
	number_tobool,
	string_tobool,
	bool_tobool,
	table_tobool,
	userfunc_tobool,
	libfunc_tobool,
	nil_tobool,
	
};
bool number_compare(avm_memcell * mA,avm_memcell* mB)
{
	return(mA->data.numVal==mB->data.numVal);
}
bool string_compare(avm_memcell * mA,avm_memcell* mB)
{
	return strcmp(mA->data.strVal,mB->data.strVal)==0?true:false;
}
bool bool_compare(avm_memcell * mA,avm_memcell* mB)
{
	return(mA->data.boolVal==mB->data.boolVal);
}
bool table_compare(avm_memcell * mA,avm_memcell* mB)
{
	return(mA->data.tableVal==mB->data.tableVal);
}
bool userfunc_compare(avm_memcell * mA,avm_memcell* mB)
{
	return(userFuncs[mA->data.funcVal].address==userFuncs[mB->data.funcVal].address);
}
bool libfunc_compare(avm_memcell * mA,avm_memcell* mB)
{
	return strcmp(mA->data.libfuncVal,mB->data.libfuncVal)==0?true:false;
}
typedef bool (*compare_same_types)(avm_memcell*,avm_memcell*);
compare_same_types compare_types[]=
{
	number_compare,
	string_compare,
	bool_compare,
	table_compare,
	userfunc_compare,
	libfunc_compare,
};
unsigned avm_tobool (avm_memcell* m)
{
	//assert(m->type >= 0 && m->type <= undef_m);
	return (*toboolFuncs[m->type])(m);

}

library_func_t avm_getlibraryfunc (char *id){	
	return lookup_libr(id);  ;
};
bool avm_jeq (instruction* t)
{
	assert(t->result.type == label_a);

	avm_memcell *rv1 = avm_translate_operand(&t->arg1,&ax);
	avm_memcell *rv2 = avm_translate_operand(&t->arg2,&bx);

	int index=rv1->type;

	bool result = 0;

	if (rv1->type == undef_m || rv2->type == undef_m)
		avm_error("'undef' involved in equality!");
	else if(rv1->type==bool_m || rv2->type==bool_m)
		result = (avm_tobool(rv1) == avm_tobool(rv2));
	else if(rv1->type==nil_m || rv2->type == nil_m)
		result = rv1->type == nil_m && rv2->type==nil_m;
	else if(rv1->type!=rv2->type)
		avm_error("illegal in jeq!"/*,typeStrings[rv1->type],typeStrings[rv2->type]*/);
	else{
		result=(*compare_types[index])(rv1,rv2);
	}
	return result;

}
void execute_jeq (instruction *instr)
{
	bool result=avm_jeq(instr);
	if(!executionFinished && result)
		pc = instr->result.val;
}
void execute_jne(instruction *instr)
{
	bool result = !avm_jeq(instr);
	if(!executionFinished && result)
		pc = instr->result.val;
}
void libfunc_typeof(void)
{
	unsigned n = avm_totalactuals();
	if(n!=1)
		printf("one argument (not %d) expected in typeof !",n);
	else
	{
		avm_memcellclear(&retval);
		retval.type=string_m;
		retval.data.strVal=strdup(typeStrings[avm_getactual(0)->type]);
	}
}
void execute_newtable (instruction * instr)
{
	avm_memcell * lv = avm_translate_operand(&instr->result,(avm_memcell*)0);
	//assert(lv && ( &stack[N-1] >= lv && lv > &stack[top] || lv == &retval ));
	avm_memcellclear(lv);
	lv->type = table_m;
	lv->data.tableVal= avm_tablenew();
	avm_tableincrefcounter(lv->data.tableVal);

}
int hash(unsigned address)
{
	return ((unsigned)address)%211;
}
avm_memcell * avm_tablegetelem (avm_table * table, avm_memcell* index) {
    avm_table_bucket *list = NULL;
    if(index->type == number_m) {
        list = table->numIndexed[hash(index->data.numVal)];
        while(list != NULL) {
            if(list->key.data.numVal == index->data.numVal)
                return &list->value;
            else
                list = list->next;
        }
        return NULL;
    }
    else if(index->type == string_m) {
        int i = 0, length = strlen(index->data.strVal), sum = 0;
        for(i = 0; i < length; i++) sum += index->data.strVal[i];

        list = table->strIndexed[hash(sum)];
        while(list != NULL) {
            if(strcmp(list->key.data.strVal, index->data.strVal) == 0)
                return &list->value;
            else
                list = list->next;
        }
        return NULL;
    }
    else avm_error("Searching for index neither number nor string");
}

avm_table_bucket* insert_number(avm_table* table, avm_table_bucket *list, avm_memcell *index, avm_memcell* content) {
	avm_table_bucket *temp=list;
    while(temp != NULL) {
        if(temp->key.data.numVal == index->data.numVal)
        {
        	temp->value=*content;
            return list;
        }
        else
            temp = temp->next;
    }
    avm_table_bucket* new_bucket = (avm_table_bucket*)malloc(sizeof(avm_table_bucket));
    new_bucket->key = *index;
    new_bucket->value = *content;
    new_bucket->next = list;
    table->total++;
    return new_bucket;
}
avm_table_bucket* insert_string(avm_table* table, avm_table_bucket *list, avm_memcell *index, avm_memcell* content) {
	avm_table_bucket *temp=list;
    while(temp != NULL) {
        if(strcmp(temp->key.data.strVal ,index->data.strVal)==0)
        {
        	temp->value=*content;
            return list;
        }
        else
            temp = temp->next;
    }
    avm_table_bucket* new_bucket = (avm_table_bucket*)malloc(sizeof(avm_table_bucket));
    new_bucket->key = *index;
    new_bucket->value = *content;
    new_bucket->next = list;
    table->total++;
    return new_bucket;
}
avm_table_bucket* delete_content_number(avm_table* table,avm_table_bucket *list,avm_memcell*index)
{
	avm_table_bucket* current=list;
	avm_table_bucket* prev=NULL;
	if(avm_tablegetelem(table,index)==NULL)return list;
	while(list!=NULL)
	{
		if(list->key.data.numVal==index->data.numVal)
		{
			break;
		}
		prev=list;
		list=list->next;
	}
	if(prev==NULL)
	{
		prev=list;
		list=list->next;
		free(prev);
	}
	else
	{
		prev->next=list->next;
		free(list);
	}
	table->total--;

	return current;
}
avm_table_bucket* delete_content_string(avm_table* table,avm_table_bucket *list,avm_memcell*index)
{
	avm_table_bucket* current=list;
	avm_table_bucket* prev=NULL;
	if(avm_tablegetelem(table,index)==NULL)return list;
	while(list!=NULL)
	{
		if(strcmp(list->key.data.strVal,index->data.strVal)==0)
		{
			break;
		}
		prev=list;
		list=list->next;
	}
	if(prev==NULL)
	{
		prev=list;
		list=list->next;
		free(prev);
	}
	else
	{
		prev->next=list->next;
		free(list);
	}
	return current;
}
void avm_tablesetelem(avm_table* table, avm_memcell *index, avm_memcell* content) {
    if(index->type == number_m) {
    	avm_table_bucket *list=table->numIndexed[hash(index->data.numVal)];
	    if(content->type==nil_m)
	    {
	    	table->numIndexed[hash(index->data.numVal)]=delete_content_number(table,list,index);
	    	return ;
	    }
	   /* if(content->type==string_m)
	    {
	       int  length = strlen(content->data.strVal);
		   char* temp;
	       if(content->data.strVal[0]==34)
	        {
	        	temp=strdup(content->data.strVal+1);
	        	length--;
	        }
	        else temp=strdup(content->data.strVal);
	        if(content->data.strVal[length]==34)
	        {
	       		temp[length-1]='\0';
	       		length--;
	       	}
	        content->data.strVal=temp;
	    	if(strcmp(content->data.strVal,"print")==0)
	    	{
	    		content->data.libfuncVal=strdup("print");
	    	}
	    	content->type=libfunc_m;
	    }*/
        table->numIndexed[hash(index->data.numVal)]=insert_number(table, list, index, content);
    }
    else if(index->type == string_m) {
        int i = 0, length = strlen(index->data.strVal), sum = 0;
        char* temp;
       
        if(index->data.strVal[0]==34)
        {
        	temp=strdup(index->data.strVal+1);
        	length--;
        }
        else temp=strdup(index->data.strVal);
        if(index->data.strVal[length]==34)
        {
       		temp[length-1]='\0';
       		length--;
       	}
       
        for(i = 0; i < length; i++)
        {
        		sum+=temp[i];
        }
        index->data.strVal=temp;
        avm_table_bucket * list=table->strIndexed[hash(sum)];
        if(content->type==nil_m)
	    {
	    	table->numIndexed[hash(sum)]=delete_content_string(table,list,index);
	    	return ;
	    }
	    if(content->type==string_m)
	    {
	    	if(strcmp(content->data.strVal,"print")==0)
	    	{
	    		content->data.strVal=strdup("print");
	    	}
	    	content->type=libfunc_m;
	    }
        table->strIndexed[hash(sum)]=insert_string(table, list, index, content);
    }
    else avm_error("Index neither number nor string");
}
void execute_tablegetelem ( instruction * instr)
{
	avm_memcell * lv = avm_translate_operand(&instr->result,(avm_memcell*)0);
	avm_memcell * t = avm_translate_operand(&instr->arg1,(avm_memcell *)0);
	avm_memcell * i = avm_translate_operand(&instr->arg2,&ax);
	avm_memcellclear(lv);

	lv->type=nil_m;

	if( t->type != table_m )
	{
		printf("illegal use of type %s as table!",typeStrings[t->type]);
	}
	else
	{
		avm_memcell * contenet = avm_tablegetelem(t->data.tableVal,i);
		if(contenet)
			avm_assign(lv,contenet);
		else
		{
			char * ts = avm_tostring(t);
			char * is = avm_tostring(i);
			//printf("%s[%s] not found",ts,is);//avm_warning
			free(ts);
			free(is);
		}
	}
}
void execute_tablesetelem(instruction * instr)
{
	avm_memcell* t = avm_translate_operand(&instr->result,(avm_memcell*)0);
	avm_memcell *i = avm_translate_operand(&instr->arg1,&ax);
	avm_memcell *c = avm_translate_operand(&instr->arg2,&bx);


	//assert(t && &stack[N-1] >= t && t > &stack[top]);
	//assert(i & c);
	if(t->type != table_m)
		printf("illegal use of type %s as table!",typeStrings[t->type]);
	else
		avm_tablesetelem(t->data.tableVal,i,c);
}
void avm_initstack (void)
{
	for(unsigned i=0; i<AVM_STACKSIZE; ++i)
	{
		AVM_WIPEOUT(stack[i]);
		stack[i].type = undef_m;
	}
}	
void libfunc_totalarguments(void)
{
	unsigned p_topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
	avm_memcellclear(&retval);
	if(!p_topsp){
		avm_error("totalarguments called outsied a function!");
		retval.type=nil_m;
	}
	else
	{
		retval.type=number_m;
		retval.data.numVal = avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET);
	}
}
void libfunc_objectmemberkeys(){return ;}
void libfunc_objecttotalmembers(){return ;}
void libfunc_objectcopy(){return ;}
void libfunc_argument(){return ;}

void libfunc_strtonum(){return ;}
void libfunc_sqrt(){return ;}
void libfunc_cos(){return ;}
void libfunc_sin(){return ;}

void avm_intitialize (void)
{
	avm_initstack();

	avm_registerlibfunc("print",libfunc_print);
	avm_registerlibfunc("typeof",libfunc_typeof);
	avm_registerlibfunc("objectmemberkeys",libfunc_objectmemberkeys);
	avm_registerlibfunc("objecttotalmembers",libfunc_objecttotalmembers);
	avm_registerlibfunc("objectcopy",libfunc_objectcopy);
	avm_registerlibfunc("totalarguments",libfunc_totalarguments);
	avm_registerlibfunc("argument",libfunc_argument);
	avm_registerlibfunc("typeof",libfunc_typeof);
	avm_registerlibfunc("strtonum",libfunc_strtonum);
	avm_registerlibfunc("sqrt",libfunc_sqrt);
	avm_registerlibfunc("cos",libfunc_cos);
	avm_registerlibfunc("sin",libfunc_sin);
}


avm_memcell *avm_translate_operand (vmarg* arg, avm_memcell* reg){
	switch(arg->type)
	{
		case global_a:	return &stack[AVM_STACKSIZE-1-arg->val];
		case local_a:	return &stack[topsp-arg->val];
		case formal_a:	return &stack[topsp+AVM_STACKENV_SIZE+1-arg->val];
		
		case retval_a: return &retval;

		case number_a:	{
			if(reg==NULL)
			{
				reg=malloc(sizeof(avm_memcell));
			}
			reg->type = number_m;
			reg->data.numVal = consts_getnumber(arg->val);
			return reg;
		}
		
		case string_a:	{
			reg->type = string_m;
			reg->data.strVal = strdup(consts_getstring(arg->val));
			return reg;
		}

		case bool_a:	{
			reg->type = bool_m;
			reg->data.boolVal = arg->val;
			return reg;
		}

		case nil_a: reg->type = nil_m; return reg;

		case userfunc_a: {
			reg->type = userfunc_m;
			reg->data.funcVal = arg->val;
			reg->data.funcVal = userfuncs_getfunc(arg->val)->address;
			return reg;
		}
		
		case libfunc_a: {
			reg->type = libfunc_m;
			reg->data.libfuncVal = libfuncs_getused(arg->val);
			return reg;
		}
		default: assert(0);

	}
}
extern int global_temp_counter;
void run_vm()
{
	globalmem=AVM_STACKSIZE-globalmem-global_temp_counter-1;
	top=globalmem;
    topsp=top;
	avm_intitialize(); 
	while(executionFinished==0)
	{
		execute_cycle();
	}
}
void execute_cycle (void)
{
	if(executionFinished)
		return;
	else
		if(pc == AVM_ENDING_PC) {
			executionFinished = 1;
			return;
		}
		else
		{
			assert(pc< AVM_ENDING_PC);
			instruction *instr = code + pc;
			assert(	
				instr->opcode>=0 && 
				instr->opcode <= AVM_MAX_INSTRUCTIONS
			);
		if(instr->srcLine)
			currLine = instr->srcLine;
		unsigned oldPC= pc;
			(*executeFuncs[instr->opcode])(instr);
			if(pc == oldPC)
				++pc;
		}
}
void memclear_string (avm_memcell* m){
	assert(m->data.strVal);
	(m->data.strVal==NULL)?free(m->data.strVal):(void)0;
}
void memclear_table (avm_memcell* m)
{
	assert(m->data.tableVal);
	avm_tabledecrefcounter(m->data.tableVal);
}
void avm_memcellclear (avm_memcell* m)
{
	if(m->type != undef_m){
		memclear_func_t f = memclearFuncs[m->type];
		if(f)
			(*f)(m);
		m->type = undef_m;
	}
}
void avm_warning(char* format)
{
	yellow();
	printf("WARNING:\n%s\n",format);
	reset();
}
void execute_assign (instruction * instr)
{
	avm_memcell* lv = avm_translate_operand(&instr->result,(avm_memcell*) 0);
	avm_memcell* rv = avm_translate_operand(&instr->arg1,&ax);
	
	//assert(lv && (&stack[N-1] >= && lv > &stack[top] || lv==&retval));
	assert(rv);
	
	avm_assign(lv,rv);
}
void avm_assign (avm_memcell* lv, avm_memcell* rv){
	if(lv==rv)
	return;

	if(	lv->type == table_m && 
		rv->type == table_m && 
		lv->data.tableVal == rv->data.tableVal)
		return;
	if(rv->type == undef_m)
		avm_warning("assigning from 'undef' content!");

	avm_memcellclear(lv);

	memcpy(lv,rv,sizeof(avm_memcell));
	
	if(lv->type == string_m)
		lv->data.strVal = strdup(rv->data.strVal);
	else
	if (lv->type == table_m)
		avm_tableincrefcounter(lv->data.tableVal);
}
void execute_call (instruction* instr) {
	avm_memcell* func = avm_translate_operand(&instr->arg1, &ax);
	assert(func);
	switch(func->type) {
		
		case userfunc_m:{
			avm_callsaveenvironment();
			pc = func->data.funcVal;
			assert(pc< AVM_ENDING_PC);
			assert(code[pc].opcode == funcenter_v);
			break;
		}

		case string_m: avm_calllibfunc(func->data.strVal);	break;
		case libfunc_m: avm_calllibfunc(func->data.libfuncVal);	break;
		case table_m: avm_call_functor(func->data.tableVal);	break;
	
		default: {
			char* s= avm_tostring(func);
			avm_error("call: cannot bind to function");
			free(s);
			executionFinished =1;		
		}
	}	
}
void avm_error(char* format)
{
	red();
	printf("ERROR:\n%s\n",format);
	reset();
	exit(EXIT_FAILURE);
}
void avm_callsaveenvironment (void)
{
	avm_push_envvalue(totalActuals);
	assert(code[pc].opcode == call_v);
	avm_push_envvalue(pc+1);
	avm_push_envvalue(top+ totalActuals +2);
	avm_push_envvalue(topsp);
}
void avm_call_functor (avm_table* t){
	cx.type = string_m;
	cx.data.strVal= "()";
	avm_memcell* f = avm_tablegetelem(t,&cx);
	if(!f)
		avm_error("in calling table: no '()' element found!");
	else
		if(f->type == table_m)
			avm_call_functor(f->data.tableVal);
		else
			if(f->type == userfunc_a)
				{
					avm_push_table_arg(t);
					avm_callsaveenvironment();
					pc = f->data.funcVal;
					assert(pc<AVM_ENDING_PC && 
					code[pc].opcode	==funcenter_v);
				}
			else
				avm_error("in calling table: illegal '()' element value!");
	return;
}














/*meiwnete to top kai exoume thema me stack overflow */



//THEMA




void avm_dec_top(void)
{
	if(top==0){
		avm_error("Runtime error: Stack overflow");
		executionFinished =1;
	}
	else
		--top;
}
void avm_push_envvalue(unsigned val){
	stack[top].type	= number_m;
	stack[top].data.numVal = val;
	avm_dec_top();
}

void execute_funcenter(instruction * instr)
{
	avm_memcell* func = avm_translate_operand(&instr->result,&ax);
	assert(func);
	assert(pc == func->data.funcVal);

	totalActuals = 0;
	//userfunc * funcInfo = avm_getfuncinfo(pc);
	topsp=top;
	top = top - userFuncs[func->data.funcVal].localSize;//func->data.funcVal->localSize;
}
unsigned avm_get_envvalue(unsigned i )
{
	assert(stack[i].type == number_m);
	unsigned val = (unsigned) stack[i].data.numVal;
	assert(stack[i].data.numVal == ((double)val));
	return val;
}
void execute_funcexit(instruction * unused)
{
	    unsigned oldTop = top;
	    top = avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET);
	    pc = avm_get_envvalue(topsp + AVM_SAVEDPC_OFFSET);
	    topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
	    while(++oldTop <= top)
	        avm_memcellclear(&stack[oldTop]);
}
void avm_calllibfunc(char * id )
{
    library_func_t f = avm_getlibraryfunc(id);
    if(!f)
    {
        avm_error("unsuppored lib func called !");
        executionFinished = 1;
    }
    else{
        

     	 avm_callsaveenvironment(); //Allagi logw xristinas



        topsp = top;
        totalActuals = 0;
        (*f)();
        if(!executionFinished)
            execute_funcexit((instruction *)0);
    }
}
void execute_uminus(instruction* instr) 
{

}
void execute_and(instruction* instr) {}
void execute_or(instruction* instr) {}
void execute_not(instruction* instr) {}
void execute_nop(instruction* instr) {}

void execute_return(instruction* instr) {}
void execute_getreval(instruction* instr) {}

