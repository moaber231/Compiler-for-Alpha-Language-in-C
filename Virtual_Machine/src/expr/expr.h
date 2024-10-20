#ifndef EXPR_H
#define EXPR_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

struct SymTable;
typedef struct stmt_t
{
    int breakList, contList;
} stmt_t;
typedef struct forpref
{
    unsigned int test;
    unsigned int enter;
} forpref;
typedef struct false_true_lists
{
    struct expr *FalseList;
    struct expr *TrueList;
} FT_list;
typedef enum
{
    assign,
    add,
    sub,
    mul,
    div_op,
    mod,
    uminus,
    if_eq,
    if_less,
    if_lesseq,
    if_greater,
    if_geatereq,
    if_noteq,
    call,
    ret,
    funcstart,
    funcend,
    tablegetelem,
    tablesetelem,
    getretval,
    tablecreate,
    jump,
    param
} iopcode;

typedef enum
{
    var_e,
    tableitem_e,
    programfunc_e,
    libraryfunc_e,
    arithexpr_e,
    boolexpr_e,
    newtable_e,
    constnum_e,
    constbool_e,
    conststring_e,
    nil_e,
    error_t
} expr_t;

typedef enum
{
    programvar,
    functionlocal,
    formalarg
} scopespace_t;

typedef enum
{
    var_s,
    programmfunc_s,
    library_s
} symbol_t;

typedef struct symbol
{
    symbol_t type;
    char *name;
    scopespace_t space;
    unsigned offset;
    unsigned scope;
    unsigned line;
    unsigned taddress;
    unsigned iaddress;
    unsigned totalLocals;
} symbol;
typedef struct map
{
    struct expr *index;
    struct expr *value;
    struct map *next;
} map;
typedef struct af_quad
{
    int nextquad;
    int actualquad;
} af_quad;
typedef struct expr
{
    expr_t type;
    struct symbol *sym;
    struct expr *index;
    double numConst;
    char *strConst;
    unsigned int lbl;
    unsigned char boolConst;
    struct expr *next;
    FT_list *list;
} expr;

typedef struct expr_map
{
    int expr_or_map;
    union
    {
        expr *expression;
        map *indexed;
    };
} expr_map;

typedef struct quad
{
    iopcode op;
    expr *result;
    expr *arg1;
    expr *arg2;
    unsigned label;
    unsigned line;
    unsigned taddress;
} quad;

typedef struct Call
{
    expr *elist;
    unsigned char method;
    char *name;
} Call;

extern unsigned programmVarOffset;
extern unsigned functionLocalOffset;
extern unsigned formalArgOffet;
extern unsigned scopeSpaceCounter;
extern int temp_counter;
extern int scope;
extern expr *Head;
extern struct SymTable *symtable;
typedef struct SymTable *SymTable_T;
enum SymbolType;
void emit(iopcode op, expr *arg1, expr *arg2, expr *result, unsigned label, unsigned line);
expr *emit_iftableitem(expr *e, int line);
void patchlabel(unsigned quadNo, unsigned label);
void resettemp(void);
void enterscopespace(void);
void exitscopespace(void);
void expand(void);
void insert(symbol *sym);
struct symbol *newtemp(int line, symbol_t symb);
struct symbol *lookup(char *name, int scope);
char *newtempname(void);
expr *new_expr(expr_t type);
expr *new_expr_constnum(double num);
expr *new_expr_conststring(const char *str);
expr *new_expr_constbool(unsigned int b);
expr *lvalue_expr(symbol *sym);
expr *member_item(expr *lvalue, char *name, int line);
expr *member_numConst(expr *lvalue, double d, int line);
expr *member_string(expr *lvalue, char *name, int line);
expr *member_boolean(expr *lvalue, unsigned b, int line);
expr *make_call(expr *lv, expr *reversed_elist, int line);
expr *get_last(expr *e);
unsigned nextquad(void);
symbol *initialize_symbol(char *id_name, int line, symbol_t symb, int scope);
expr *insert_elist(expr *h, expr *e);
map *insert_indexed(map *h, map *e);
map *new_map(expr *index, expr *value);
expr_map *new_expression(int b, expr *exp);
expr_map *new_mapping(int b, map *m);
void check_arith(expr *e, const char *context);
void resetformalargsoffset();
unsigned currscopeoffset(void);
void resetfunctionlocalsoffset();
void restorecurrscopeoffset(unsigned offset);
Call *new_Call(expr *elist, unsigned char method, char *name);
unsigned int istempname(char *s);
unsigned int istempexpr(expr *e);
void print_quad(quad q);
void print_all_quads(void);
expr *expr_num_type(unsigned arithop, expr *e1, expr *e2, int line);
expr *assign_expr(expr *assign_e, expr *lval, expr *e, int line);
scopespace_t currscopespace(void);
void patchlabel(unsigned quadNo, unsigned label);
stmt_t *make_stmt(stmt_t *s);
int newlist(int i);
int mergelist(int l1, int l2);
void patchlist(int list, int label);
forpref *make_for_pref(unsigned int test, unsigned int enter);
expr *make_lists(expr *e);
void BackPatch(expr *e, unsigned int label);
expr *merge(expr *e1, expr *e2);
int create_relop_quad(iopcode op, expr *arg1, expr *arg2, expr *result, unsigned label, unsigned line);
void update_false_true_lists(FT_list *e);
unsigned actual_nextquad(void);
void true_jump_false(int line, expr *e);
struct af_quad *new_Af_quad(int nextquad, int actualquad);
int SymTable_insert(SymTable_T oSymTable, char *name, unsigned int scope, unsigned int line, enum SymbolType type);
bool SymTable_contains(SymTable_T oSymTable, char *name, enum SymbolType type, int scope, int insideFunction);
void fix_jumps();
#endif
