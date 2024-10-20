#ifndef VMARG_H
#define VMARG_H
#include "../stack/stack.h"

typedef enum vmoopcode
{
    assign_v,
    add_v,
    sub_v,
    mul_v,
    div_v,
    mod_v,
    uminus_v,
    and_v,
    or_v,
    not_v,
    jeq_v,
    jne_v,
    jle_v,
    jge_v,
    jlt_v,
    jgt_v,
    call_v,
    pusharg_v,
    funcenter_v,
    funcexit_v,
    newtable_v,
    tablegetelem_v,
    nop_v
} vmoopcode;
typedef enum vmarg_t
{
    label_a = 0,
    global_a = 1,
    formal_a = 2,
    local_a = 3,
    number_a = 4,
    string_a = 5,
    bool_a = 6,
    nil_a = 7,
    userfunc_a = 8,
    libfunc_a = 9,
    retval_a = 10
} vmarg_t;
typedef struct vmarg
{
    vmarg_t type;
    unsigned val;
} vmarg;
typedef struct instruction
{
    vmoopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned srcLine;
} instruction;
typedef struct userfunc
{
    unsigned address;
    unsigned localSize;
    char *id;
} userfunc;
typedef struct quad quad;
typedef struct expr expr;
typedef struct symbol symbol;
extern quad *quads;
extern void add_incomplete_jump(unsigned instrNo, unsigned iaddress);
extern void generate_ADD(quad *q);
extern void generate_SUB(quad *q);
extern void generate_MUL(quad *q);
extern void generate_DIV(quad *q);
extern void generate_MOD(quad *q);
void generate_UMINUS(quad *q);
extern void generate_NEWTABLE(quad *q);
extern void generate_TABLEGETELM(quad *q);
extern void generate_TABLESETELEM(quad *q);
extern void generate_ASSIGN(quad *q);
extern void generate_NOP(quad *q);
extern void generate_JUMP(quad *q);
extern void generate_IF_EQ(quad *q);
extern void generate_IF_NOTEQ(quad *q);
extern void generate_IF_GREATER(quad *q);
extern void generate_IF_GREATEREQ(quad *q);
extern void generate_IF_LESS(quad *q);
extern void generate_IF_LESSEQ(quad *q);
extern void generate_NOT(quad *q);
extern void generate_OR(quad *q);
extern void generate_PARAM(quad *q);
extern void generate_CALL(quad *q);
extern void generate_GETRETVAL(quad *q);
extern void generate_FUNCSTART(quad *q);
extern void generate_RETURN(quad *q);
extern void generate_FUNCEND(quad *q);
typedef void (*generator_func_t)(quad *q);

extern generator_func_t generators[];
unsigned consts_newstring(char *s);
unsigned consts_newnumber(double n);
unsigned libfucns_newused(char *s);
unsigned userfuncs_newfunc(symbol *sym);
void make_operand(expr *e, vmarg *arg);
void make_numberoperand(vmarg *arg, double val);
void make_booloperand(vmarg *arg, unsigned val);
void make_retvaloperand(vmarg *arg);
void Generate_Ins(void);
void generate(vmoopcode op, quad *q);
void emit_instruction(instruction t);
#endif