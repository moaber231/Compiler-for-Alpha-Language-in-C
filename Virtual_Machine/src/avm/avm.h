#define AVM_NUMACTUALS_OFFSET +4
#define AVM_SAVEDPC_OFFSET +3
#define AVM_SAVEDTOP_OFFSET +2
#define AVM_SAVEDTOPSP_OFFSET +1
#define AVM_ENDING_PC codeSize
#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#define AVM_TABLE_HASHSIZE 211
#define AVM_STACKENV_SIZE 4
#define AVM_MAX_INSTRUCTIONS (unsigned) not_v
#define MAGICNUMBER 340200501
#include "../vmarg/vmarg.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef enum avm_memcell_t {
	number_m =0,
	string_m =1,
	bool_m =2,
	table_m =3,
	userfunc_m =4,
	libfunc_m =5,
	nil_m =6,
	undef_m =7
}avm_memcell_t;
char * typeStrings[] = {
    "number",
    "string",
    "bool",
    "table",
    "userfunc",
    "libfunc",
    "nil",    
    "undef"
};
typedef struct avm_memcell avm_memcell;
typedef struct avm_table_bucket avm_table_bucket;
typedef struct avm_table avm_table;

typedef struct avm_memcell {
	avm_memcell_t type;
	union{
		double numVal;
		char* strVal;
		unsigned char boolVal;
		avm_table* tableVal;
		unsigned funcVal;
		char* libfuncVal;	
	}data;
}avm_memcell;
typedef struct avm_table_bucket {
	avm_memcell key;
	avm_memcell value;
	avm_table_bucket* next;
}avm_table_bucket;
typedef struct avm_table{
	unsigned	refCounter;
	avm_table_bucket* strIndexed[AVM_TABLE_HASHSIZE];
	avm_table_bucket* numIndexed[AVM_TABLE_HASHSIZE];
	unsigned total;
}avm_table;

typedef void (*memclear_func_t)(avm_memcell*);
void memclear_string (avm_memcell* m);
void memclear_table (avm_memcell* m);

memclear_func_t memclearFuncs[] ={
	0,
	memclear_string,
	0,
	memclear_table,
	0,
	0,
	0,
	0
};
extern instruction *instructions;
typedef void (*execute_func_t) (instruction*);
void execute_assign (instruction*);
void execute_add (instruction*);
void execute_sub (instruction*);
void execute_mul (instruction*);
void execute_div (instruction*);
void execute_mod (instruction*);
void execute_uminus (instruction*);
void execute_and (instruction*);
void execute_or (instruction*);
void execute_not (instruction*);
void execute_jeq (instruction*);
void execute_jne (instruction*);
void execute_jle (instruction*);
void execute_jge (instruction*);
void execute_jlt (instruction*);
void execute_jgt (instruction*);
void execute_call (instruction*);
void execute_pusharg (instruction*);
void execute_funcenter (instruction*);
void execute_funcexit (instruction*);
void execute_newtable (instruction*);
void execute_tablegetelem (instruction*);
void execute_tablesetelem (instruction*);
void execute_nop (instruction*);
void execute_jump(instruction*);

void execute_return(instruction* instr) ;
void execute_getreval(instruction* instr) ;
execute_func_t executeFuncs[]={
	execute_assign,
	execute_add,
	execute_sub, 
	execute_mul, 
	execute_div, 
	execute_mod, 
	execute_uminus, 
	execute_jeq, 
	execute_jlt, 
	execute_jle, 
	execute_jgt, 
	execute_jge, 
	execute_jne, 
	execute_call,
	execute_return, 
	execute_funcenter, 
	execute_funcexit, 
	execute_tablegetelem,
	execute_tablesetelem,
	execute_getreval, 
	execute_newtable, 
	execute_jump ,
	execute_pusharg, 
	execute_nop
	
};
typedef void(*library_func_t)(void);

typedef struct map_library_funcs
{
	char * key;
	library_func_t value;
}map_library_funcs;

void insert_libr(char *key,library_func_t value);
library_func_t lookup_libr(char *key);

unsigned char executionFinished =0;
unsigned pc=0;
unsigned currLine =0; 
unsigned codeSize =0;
extern int globalmem;
//extern datamem;
instruction* code = (instruction*) 0;
avm_memcell stack[AVM_STACKSIZE];
avm_memcell ax,bx,cx;
avm_memcell retval;
unsigned top,topsp;
unsigned totalActuals = 0;

void run_vm();
double consts_getnumber (unsigned index);
char* consts_getstring (unsigned index);
char* libfuncs_getused (unsigned index);
userfunc* userfuncs_getfunc (unsigned index);
avm_memcell *avm_translate_operand (vmarg* arg, avm_memcell* reg);
void execute_cycle (void);
typedef void (*memclear_func_t)(avm_memcell*);
void avm_memcellclear (avm_memcell* m);
void avm_warning(char* format);
void avm_assign(avm_memcell*lv,avm_memcell* rv);
void avm_error(char* format);
char* avm_tostring(avm_memcell*);
void avm_calllibfunc (char* funcName);//de exoume kanei
void avm_callsaveenvironment (void);
void avm_call_functor (avm_table* t);
void avm_push_arg (avm_table* t);//de ec
void avm_dec_top(void);
void avm_push_envvalue(unsigned val);
userfunc * avm_getfuncinfo (unsigned adress);//de
unsigned avm_get_envvalue(unsigned i );
void execute_funcexit(instruction * unused);
void avm_callibfunc(char * id );
avm_table* avm_tablenew (void);
void avm_tabledestroy (avm_table* t);
avm_memcell * avm_tablegetelem (avm_table * table,	avm_memcell* index);//de 
void avm_tablesetelem(avm_table* table,avm_memcell * index ,avm_memcell* contenet);//de
void avm_tablebucketsinit(avm_table_bucket** p);
void avm_tablebucketsdestroy(avm_table_bucket** p);
void avm_tableincrefcounter(avm_table *t);
