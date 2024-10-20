#include "vmarg.h"
#include <string.h>
#include <stdlib.h>
#include "../stack/stack.h"
extern unsigned int total;
instruction *instructions = NULL;
int currInstruction = 0;
int total_instructions = 0;
double *numConsts;
unsigned totalNumConsts;
char **stringConsts;
unsigned totalStringConsts;
char **namedLibfuncs;
unsigned totalNamedLibFuncs;
userfunc *userFuncs;
unsigned totalUserFuncs;
char **globalVar;
int globalmem=0;
char **localVar;
unsigned localVarCounter=0;
char **formalVar;
unsigned formalVarCounter=0;

static struct StackNode* stack_userfunc = NULL;
static bool funcenter=false;
static bool callfunction=false;
static bool NoFunction=false;

struct StackNode *funcstack = NULL;
generator_func_t generators[] = {
    generate_ASSIGN,
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_UMINUS,
    generate_IF_EQ,
    generate_IF_LESS,
    generate_IF_LESSEQ,
    generate_IF_GREATER,
    generate_IF_GREATEREQ,
    generate_IF_NOTEQ,
    generate_CALL,
    generate_RETURN,
    generate_FUNCSTART,
    generate_FUNCEND,
    generate_TABLEGETELM,
    generate_TABLESETELEM,
    generate_GETRETVAL,
    generate_NEWTABLE,
    generate_JUMP,
    generate_PARAM,
    generate_NOT,
    generate_OR,
    generate_NOP,
};
unsigned consts_newnumber(double n)
{
    double *temp = NULL;
    totalNumConsts++;
    temp = realloc(numConsts, totalNumConsts * sizeof(double));
    if(temp==NULL)exit(EXIT_FAILURE);
    numConsts = temp;
    numConsts[totalNumConsts - 1] = n;
    return totalNumConsts - 1;
}
unsigned consts_newstring(char *s)
{
    char **temp = NULL;
    totalStringConsts++;
    temp = realloc(stringConsts, totalStringConsts * sizeof(char *));
    if(temp==NULL)exit(EXIT_FAILURE);
    stringConsts = temp;
           int  length = strlen(s);
           char* temp1;
           if(s[0]==34)
            {
                temp1=strdup(s+1);
                length--;
            }
            else temp1=strdup(s);
            if(s[length]==34)
            {
                temp1[length-1]='\0';
                length--;
            }
            s=temp1;
    stringConsts[totalStringConsts - 1] = strdup(s);
    return totalStringConsts - 1;
}
unsigned libfucns_newused(char *s)
{
    char **temp = NULL;
    totalNamedLibFuncs++;
    temp = realloc(namedLibfuncs, totalNamedLibFuncs * sizeof(char *));
    if(temp==NULL)exit(EXIT_FAILURE);
    namedLibfuncs = temp;
    namedLibfuncs[totalNamedLibFuncs - 1] = strdup(s);
    return totalNamedLibFuncs - 1;
}
void global_newused(symbol *s)
{
    char **temp = NULL;
    globalmem++;
    temp = realloc(globalVar, globalmem * sizeof(char *));
    if(temp==NULL)exit(EXIT_FAILURE);
    globalVar = temp;
    globalVar[globalmem - 1] = strdup(s->name);
    return;
}
void local_newused(symbol *s)
{
    char **temp = NULL;
    localVarCounter++;
    temp = realloc(localVar, localVarCounter * sizeof(char *));
    if(temp==NULL)exit(EXIT_FAILURE);
    localVar = temp;
    localVar[localVarCounter - 1] = strdup(s->name);
    return;
}
void formal_newused(symbol *s)
{
    char **temp = NULL;
    formalVarCounter++;
    temp = realloc(formalVar, formalVarCounter * sizeof(char *));
    if(temp==NULL)exit(EXIT_FAILURE);
    formalVar = temp;
    formalVar[formalVarCounter - 1] = strdup(s->name);
    return;
}
unsigned userfuncs_newfunc(symbol *sym)
{
    userfunc *temp = NULL;
    totalUserFuncs++;
    temp = realloc(userFuncs, totalUserFuncs * sizeof(userfunc));
    if(temp==NULL)exit(EXIT_FAILURE);
    userFuncs = temp;
    userFuncs[totalUserFuncs - 1].address = sym->taddress;
    userFuncs[totalUserFuncs - 1].id = strdup(sym->name);
    userFuncs[totalUserFuncs - 1].localSize = sym->totalLocals;
    return totalUserFuncs - 1;
}
void add_incomplete_jump(unsigned instrNo, unsigned iaddress)
{
    instructions[instrNo].result.val = iaddress;
}
void make_operand(expr *e, vmarg *arg)
{
    if(e!=NULL && e->sym!=NULL && e->sym->name!=NULL)
    {
        arg->name=strdup(e->sym->name);
    }
    switch (e->type)
    {
    case var_e:
    case tableitem_e:
    case arithexpr_e:
    case boolexpr_e:
    case newtable_e:
    {
        assert(e->sym);
        arg->val = e->sym->offset;
        switch (e->sym->space)
        {
        case programvar:
            arg->type = global_a;
            break;
        case functionlocal:
            arg->type = local_a;
            break;
        case formalarg:
            arg->type = formal_a;
            break;
        default:
            assert(0);
        }
        break;
    }
    case constbool_e:
    {
        arg->val = e->boolConst;
        arg->type = bool_a;
        break;
    }
    case conststring_e:
    {
        arg->val = consts_newstring(e->strConst);
        arg->type = string_a;
        break;
    }
    case constnum_e:
    {
        arg->val = consts_newnumber(e->numConst);
        arg->type = number_a;
        break;
    }
    case nil_e:
        arg->type = nil_a;
        break;
    case programfunc_e:
    {
        arg->type = userfunc_a;
        //arg->val=e->sym->taddress
        //arg->val = userfuncs_newfunc(e->sym);
        if(callfunction==true || NoFunction==true)
        {
            for(int i=totalUserFuncs;i>0;i--)
            {
                if(strcmp(e->sym->name,userFuncs[i-1].id)==0)
                    {
                        arg->val=i-1;
                        break;
                    }
            }
        }
        else if(funcenter==true)
        {
            arg->val = userfuncs_newfunc(e->sym);
            //if(genrate_call==false)
            push(&stack_userfunc,NULL,arg->val,NULL,NULL);
        }
        else
        {
            //if(genrate_call==true)
            arg->val=(unsigned)peek(stack_userfunc);
            pop(&stack_userfunc);
        }
        break;
    }
    case libraryfunc_e:
    {
        arg->type = libfunc_a;
        arg->val = libfucns_newused(e->sym->name);
        break;
    }
    default:
        assert(0);
    }
}
void make_numberoperand(vmarg *arg, double val)
{
    arg->val = consts_newnumber(val);
    arg->type = number_a;
}
void make_booloperand(vmarg *arg, unsigned val)
{
    arg->val = val;
    arg->type = bool_a;
}
void make_retvaloperand(vmarg *arg)
{
    arg->type = retval_a;
}
static void expand_instruction(void)
{
    int newSize = total_instructions + 1024;
    instructions = (instruction *)realloc(instructions, newSize * sizeof(instruction));
    assert(instructions != NULL);
    total_instructions = newSize;
}
void emit_instruction(instruction t)
{
    if (currInstruction == total_instructions)
    {
        expand_instruction();
    }
    instructions[currInstruction] = t;
    currInstruction++;
}
unsigned nextinstructionlabel()
{
    return currInstruction;
}
static void check_function_arg1(quad *q)
{
    if(q!=NULL && q->arg1!=NULL &&  q->arg1->sym!=NULL && q->arg1->sym->type==programmfunc_s)
    {
        NoFunction=true;
    } 
}
static void check_function_arg2(quad *q)
{
    if(q!=NULL && q->arg2!=NULL &&  q->arg2->sym!=NULL && q->arg2->sym->type==programmfunc_s)
    {
        NoFunction=true;
    } 
}
static void check_function_result(quad *q)
{
    if(q!=NULL && q->result!=NULL &&  q->result->sym!=NULL && q->result->sym->type==programmfunc_s)
    {
        NoFunction=true;
    } 
}
void generate(vmoopcode op, quad *q)
{
    instruction *t = (instruction *)malloc(sizeof(instruction));
    t->opcode = op;
    
    check_function_arg1(q);
    if(q->arg1!=NULL)make_operand(q->arg1, &t->arg1);
    else t->arg1.type=nop_v;
    NoFunction=false;
    
    check_function_arg2(q);
    if(q->arg2!=NULL)make_operand(q->arg2, &t->arg2);
    else t->arg2.type=nop_v;
    NoFunction=false;
    
    check_function_result(q);
    if(q->result!=NULL)make_operand(q->result, &t->result);
    else t->result.type=nop_v;
    NoFunction=false;
    
    q->taddress = nextinstructionlabel();
    t->srcLine=q->line;
    emit_instruction(*t);
    free(t);
}
void Generate_Ins(void)
{
    for (unsigned i = 0; i < actual_nextquad(); ++i)
    {
        if(quads[i].op<24)
        {
            generators[quads[i].op](quads + i);
        }
    }
}
unsigned currprocessedquad(quad *q)
{
    int counter = 0;
    for (int i = 0; i < total; i++)
    {
        if (quads[i].op != -1)
        {
            if (&quads[i] == q)
            {
                return (unsigned)counter;
            }
            else
            {
                counter++;
            }
        }
    }
}
void generate_relational(vmoopcode op, quad *q)
{
    instruction *t = (instruction *)malloc(sizeof(instruction));
    t->opcode = op;
    
    check_function_arg1(q);    
    make_operand(q->arg1, &t->arg1);
    NoFunction=false;

    check_function_arg2(q);    
    make_operand(q->arg2, &t->arg2);
    NoFunction=false;
   
    t->result.type = label_a;
    t->srcLine=q->line;
    if (q->label < currprocessedquad(q))
    {
        t->result.val = q[q->label].taddress;
    }
    else
    {   //Den kseroume an xreiazetaix
        //add_incomplete_jump(nextinstructionlabel(), q->label);
        t->result.val=q->label;
    }
    q->taddress = nextinstructionlabel();
    emit_instruction(*t);
}
void generate_ADD(quad *q) { generate(add_v, q); }
void generate_SUB(quad *q) { generate(sub_v, q); }
void generate_MUL(quad *q) { generate(mul_v, q); }
void generate_UMINUS(quad *q) 
{ 
    q->arg2=new_expr_constnum(-1);
    generate(mul_v, q); 
}
void generate_DIV(quad *q) { generate(div_v, q); }
void generate_MOD(quad *q) { generate(mod_v, q); }
void generate_NEWTABLE(quad *q) { generate(newtable_v, q); }
void generate_TABLEGETELM(quad *q) { generate(tablegetelem_v, q); }
void generate_TABLESETELEM(quad *q) {generate(tablesetelem_v, q);}
void generate_ASSIGN(quad *q) { 

    generate(assign_v, q); 
    
}
void generate_NOP(quad *q)
{
    instruction *t = malloc(sizeof(instruction));
    t->opcode = nop_v;
    emit_instruction(*t);
    free(t);
}
void generate_JUMP(quad *q) {
    instruction *t = malloc(sizeof(instruction));
    t->opcode = jump_v;
    t->srcLine=q->line;
    t->result.val=q->label;
    emit_instruction(*t); 
    free(t);
}
void generate_IF_EQ(quad *q) { generate_relational(jeq_v, q); }
void generate_IF_NOTEQ(quad *q) { generate_relational(jne_v, q); }
void generate_IF_GREATER(quad *q) { generate_relational(jgt_v, q); }
void generate_IF_GREATEREQ(quad *q) { generate_relational(jge_v, q); }
void generate_IF_LESS(quad *q) { generate_relational(jlt_v, q); }
void generate_IF_LESSEQ(quad *q) { generate_relational(jle_v, q); }
void generate_PARAM(quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction *t = malloc(sizeof(instruction));
    t->opcode = pusharg_v;
    t->srcLine=q->line;
    

    t->arg2.type=nop_v;
    t->result.type=nop_v;

    check_function_arg1(q);    
    make_operand(q->arg1, &t->arg1);
    NoFunction=false;
 
    emit_instruction(*t);
    free(t);
}
void generate_CALL(quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction *t;
    t =(instruction*) malloc(sizeof(instruction));
    if(t==NULL)exit(EXIT_FAILURE);
    t->opcode = call_v;
    t->srcLine=q->line;

    t->arg2.type=nop_v;
    t->result.type=nop_v;
    callfunction=true;

    check_function_arg1(q);    
    make_operand(q->arg1, &t->arg1);
    NoFunction=false;
    
    callfunction=false;

    emit_instruction(*t);
    free(t);
}
void generate_GETRETVAL(quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction *t = malloc(sizeof(instruction));
    t->opcode = assign_v;
    t->srcLine=q->line;

    check_function_result(q);    
    make_operand(q->result, &t->result);
    NoFunction=false;

    make_retvaloperand(&t->arg1);
    t->arg2.type=nop_v;
    emit_instruction(*t);
    free(t);
}
void generate_FUNCSTART(quad *q)
{
    symbol *f = q->result->sym;  
    f->taddress = nextinstructionlabel();
    q->taddress = nextinstructionlabel();
    instruction *t = (instruction *)malloc(sizeof(instruction));
    t->opcode = funcenter_v;
    t->srcLine=q->line;

    funcenter=true;
    make_operand(q->result, &(t->result));
    NoFunction=false;

    t->arg1.type=nop_v;
    t->arg2.type=nop_v;
    emit_instruction(*t);
    free(t);
}
void generate_RETURN(quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction *t = (instruction *)malloc(sizeof(instruction));
    t->opcode = assign_v;
    t->srcLine=q->line;
    make_retvaloperand(&(t->result));
    
    check_function_arg1(q);    
    make_operand(q->arg1, &(t->arg1));
    NoFunction=false;

    t->arg2.type=nop_v;
    emit_instruction(*t);
    free(t);
}
void generate_FUNCEND(quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction *t = (instruction *)malloc(sizeof(instruction));
    t->opcode = funcexit_v;
    t->srcLine=q->line;

    funcenter=false;

    
    make_operand(q->result, &(t->result));
    
    t->arg1.type=nop_v;
    t->arg2.type=nop_v;

    emit_instruction(*t);
    free(t);
}
void generate_NOT(quad *q)
{
    return;
}
void generate_OR(quad *q)
{
    return;
}
const char* opcodeToString(vmoopcode opcode) {
    switch(opcode) {
        case assign_v: return "assign_v";
        case add_v: return "add_v";
        case sub_v: return "sub_v";
        case mul_v: return "mul_v";
        case div_v: return "div_v";
        case mod_v: return "mod_v";
        case uminus_v: return "uminus_v";
        case and_v: return "and_v";
        case or_v: return "or_v";
        case not_v: return "not_v";
        case jeq_v: return "jeq_v";
        case jne_v: return "jne_v";
        case jle_v: return "jle_v";
        case jge_v: return "jge_v";
        case jlt_v: return "jlt_v";
        case jgt_v: return "jgt_v";
        case call_v: return "call_v";
        case pusharg_v: return "pusharg_v";
        case funcenter_v: return "funcenter_v";
        case funcexit_v: return "funcexit_v";
        case newtable_v: return "newtable_v";
        case tablegetelem_v: return "tablegetelem_v";
        case tablesetelem_v: return "tablesetelem_v";
        case nop_v: return "nop_v";
        case jump_v: return "jump_v";
        default: return "UNKNOWN";
    }
}
void printTables(void)
{
    int i=0;
    printf("-------------LOCAL---------------\n");
    for(i=0;i<localVarCounter;i++)
    {
        printf("Name: %s Count:%d \n",localVar[i],i);
    }
    printf("-------------GLOBAL---------------\n");
    for(i=0;i<globalmem;i++)
    {
        printf("Name: %s Count:%d \n",globalVar[i],i);
    }
    printf("-------------FORMAL---------------\n");
    for(i=0;i<formalVarCounter;i++)
    {
        printf("Name: %s Count:%d \n",formalVar[i],i);
    }
    printf("-------------NUMBERS---------------\n");
    for(i=0;i<totalNumConsts;i++)
    {
        printf("Name: %f Count:%d \n",numConsts[i],i);
    }
    printf("-------------STRINGS---------------\n");
    for(i=0;i<totalStringConsts;i++)
    {
        printf("Name: %s Count:%d \n",stringConsts[i],i);
    }
    printf("-------------LIBFUNCS---------------\n");
    for(i=0;i<totalNamedLibFuncs;i++)
    {
        printf("Name: %s Count:%d \n",namedLibfuncs[i],i);
    }
    printf("-------------USERFUNCS---------------\n");
    for(i=0;i<totalUserFuncs;i++)
    {
        printf("Name: %s Count:%d \n",userFuncs[i].id,i);
    }
}
FILE *binary;
static void print_vmarg(vmarg* v)
{
    int i;
    if(v==NULL){
        printf("        |");
        //fprintf(binary,"");
        return;
    }
    else if(v->type==label_a)
    {
        printf(" 00:(label),%d |",v->val);
        fprintf(binary,"00%d",v->val );
    }
    else if(v->type==global_a)
    {
        printf(" 01:(global),%u:%s |",v->val,v->name);
        fprintf(binary,"01%d",v->val );
    }
    else if(v->type==local_a)
    {
       printf(" 02:(local),%u:%s |",v->val,v->name);
       fprintf(binary,"02%d",v->val );
    }
    else if(v->type==formal_a)
    {
       printf(" 03:(formal),%u:%s |",v->val,v->name);
       fprintf(binary,"03%d",v->val );
    }
    else if(v->type==number_a)
    {
       printf(" 04:(number_a),%u:%f |",v->val,numConsts[v->val]);
       fprintf(binary,"04%d",v->val);
    }
    else if(v->type==string_a)
    {
       printf(" 05:(string_a),%u:%s |",v->val,stringConsts[v->val]);
       fprintf(binary,"05%d",v->val);
    }
    else if(v->type==bool_a)
    {
       printf(" 06:(bool_a),%u |",v->val);
       fprintf(binary,"06%d",v->val);
    }
    else if(v->type==nil_a)
    {
       printf(" 07:(nil_a),%u |",v->val);
       fprintf(binary,"07%d",v->val);
    }
     else if(v->type==userfunc_a)
    {
       printf(" 08:(userfunc_a),%u:%s |",v->val,userFuncs[v->val].id);
       fprintf(binary,"08%d",v->val);
    }
    else if(v->type==libfunc_a)
    {
       printf(" 09:(libfunc_a),%u:%s |",v->val,namedLibfuncs[v->val]);
       fprintf(binary,"09%d",v->val);
    }
    else if(v->type==retval_a)
    {
       printf(" 10:(retval_a) |");
       fprintf(binary,"10%d",v->val);
    }
}
void printInstructions(void)
{
    binary=fopen("binary.alpha","w+");
    if(binary==NULL)
    {
        perror("Cannot open file to write binary\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < currInstruction; i++) {
        printf("Instruction %d: |", i);
        
        printf("  Opcode: %s |", opcodeToString(instructions[i].opcode));
        fprintf(binary,"%d", instructions[i].opcode);
        
        print_vmarg(&instructions[i].result);
        print_vmarg(&instructions[i].arg1);
        print_vmarg(&instructions[i].arg2);
        printf("  Source Line: %u\n", instructions[i].srcLine);
        fprintf(binary,"\n");
    }
    fclose(binary);
}