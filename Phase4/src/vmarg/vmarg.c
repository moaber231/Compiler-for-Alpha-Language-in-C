#include "vmarg.h"
#include <string.h>
#include <stdlib.h>
extern unsigned int total;
instruction *instructions = NULL;
int currInstruction = 0;
int total_instructions = 0;
static double *numConsts;
static unsigned totalNumConsts;
static char **stringConsts;
static unsigned totalStringConsts;
static char **namedLibfuncs;
static unsigned totalNamedLibFuncs;
static userfunc *userFuncs;
static unsigned totalUserFuncs;
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
    numConsts = temp;
    numConsts[totalNumConsts - 1] = n;
    return totalNumConsts - 1;
}
unsigned consts_newstring(char *s)
{
    char **temp = NULL;
    totalStringConsts++;
    temp = realloc(stringConsts, totalStringConsts * sizeof(char *));
    stringConsts = temp;
    stringConsts[totalStringConsts - 1] = strdup(s);
    return totalStringConsts - 1;
}
unsigned libfucns_newused(char *s)
{
    char **temp = NULL;
    totalNamedLibFuncs++;
    temp = realloc(namedLibfuncs, totalNamedLibFuncs * sizeof(char *));
    namedLibfuncs = temp;
    namedLibfuncs[totalNamedLibFuncs - 1] = strdup(s);
    return totalNamedLibFuncs - 1;
}
unsigned userfuncs_newfunc(symbol *sym)
{
    userfunc *temp = NULL;
    totalUserFuncs++;
    temp = realloc(userFuncs, totalUserFuncs * sizeof(char *));
    userFuncs = temp;
    userFuncs[totalUserFuncs - 1].address = sym->iaddress;
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
        arg->val = e->sym->iaddress;
    }
    case libraryfunc_e:
    {
        arg->type = libfunc_a;
        arg->val = libfucns_newused(e->sym->name);
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
    instructions[total_instructions] = t;
    total_instructions++;
}
unsigned nextinstructionlabel()
{
    return currInstruction;
}
void generate(vmoopcode op, quad *q)
{
    instruction *t = (instruction *)malloc(sizeof(instruction));
    t->opcode = op;
    make_operand(q->arg1, &t->arg1);
    make_operand(q->arg2, &t->arg2);
    make_operand(q->result, &t->result);
    q->taddress = nextinstructionlabel();
    emit_instruction(*t);
}
void Generate_Ins(void)
{
    for (unsigned i = 0; i < actual_nextquad(); ++i)
        generators[quads[i].op](quads + i);
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
    make_operand(q->arg1, &t->arg1);
    make_operand(q->arg2, &t->arg2);
    t->result.type = label_a;
    if (q->label < currprocessedquad(q))
    {
        t->result.val = q[q->label].taddress;
    }
    else
    {
        add_incomplete_jump(nextinstructionlabel(), q->label);
    }
    q->taddress = nextinstructionlabel();
}
void generate_ADD(quad *q) { generate(add, q); }
void generate_SUB(quad *q) { generate(sub, q); }
void generate_MUL(quad *q) { generate(mul, q); }
void generate_UMINUS(quad *q) { generate(uminus, q); }
void generate_DIV(quad *q) { generate(div_op, q); }
void generate_MOD(quad *q) { generate(mod, q); }
void generate_NEWTABLE(quad *q) { generate(newtable_e, q); }
void generate_TABLEGETELM(quad *q) { generate(tablegetelem, q); }
void generate_TABLESETELEM(quad *q) { generate(tablesetelem, q); }
void generate_ASSIGN(quad *q) { generate(assign, q); }
void generate_NOP(quad *q)
{
    instruction *t = malloc(sizeof(instruction));
    t->opcode = nop_v;
    emit_instruction(*t);
}
void generate_JUMP(quad *q) { /*generate(jump, q);*/ }
void generate_IF_EQ(quad *q) { generate_relational(if_eq, q); }
void generate_IF_NOTEQ(quad *q) { generate_relational(if_noteq, q); }
void generate_IF_GREATER(quad *q) { generate_relational(if_greater, q); }
void generate_IF_GREATEREQ(quad *q) { generate_relational(if_geatereq, q); }
void generate_IF_LESS(quad *q) { generate_relational(if_less, q); }
void generate_IF_LESSEQ(quad *q) { generate_relational(if_lesseq, q); }
void generate_PARAM(quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction *t = malloc(sizeof(instruction));
    t->opcode = pusharg_v;
    make_operand(q->arg1, &t->arg1);
    emit_instruction(*t);
}
void generate_CALL(quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction *t = malloc(sizeof(instruction));
    t->opcode = call_v;
    make_operand(q->arg1, &t->arg1);
    emit_instruction(*t);
}
void generate_GETRETVAL(quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction *t = malloc(sizeof(instruction));
    t->opcode = assign;
    make_operand(q->result, &t->result);
    make_retvaloperand(&t->arg1);
    emit_instruction(*t);
}
void generate_FUNCSTART(quad *q)
{
    symbol *f = q->result->sym;
    f->taddress = nextinstructionlabel();
    q->taddress = nextinstructionlabel();
    push(&funcstack, NULL, -1, NULL, f);
    instruction *t = (instruction *)malloc(sizeof(instruction));
    t->opcode = funcenter_v;
    make_operand(q->result, &(t->result));
    emit_instruction(*t);
}
void generate_RETURN(quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction *t = (instruction *)malloc(sizeof(instruction));
    t->opcode = assign_v;
    make_retvaloperand(&(t->result));
    make_operand(q->arg1, &(t->arg1));
    emit_instruction(*t);
}
void generate_FUNCEND(quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction *t = (instruction *)malloc(sizeof(instruction));
    t->opcode = funcexit_v;
    make_operand(q->result, &(t->result));
    emit_instruction(*t);
}
void generate_NOT(quad *q)
{
    return;
}
void generate_OR(quad *q)
{
    return;
}