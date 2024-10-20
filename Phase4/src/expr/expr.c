#include "expr.h"
#include "../symtable/symtable.h"

unsigned programmVarOffset = 0;
unsigned functionLocalOffset = 0;
unsigned formalArgOffet = 0;
unsigned scopeSpaceCounter = 0;
int temp_counter = 0;
int useless_quad = 0;
quad *quads = NULL;
quad *total_quads = NULL;
unsigned total = 0;
unsigned total1 = 0;
unsigned currQuad = 0;
int global_position = -1;
extern FILE *file;
static char *inttostring(int num)
{
    int i, rem, length = 0, n = num;
    if (n == 0)
        length = 1;
    while (n != 0)
    {
        length++;
        n /= 10;
    }
    char *str = (char *)malloc(sizeof(char) * (length + 1));
    for (i = 0; i < length; i++)
    {
        rem = num % 10;
        num /= 10;
        str[length - (i + 1)] = rem + '0';
    }
    str[length] = '\0';
    return str;
}
char *newtempname(void)
{
    char *src = inttostring(temp_counter++);
    char *dest = (char *)malloc((strlen(src) + 3) * sizeof(char)); 
    strcpy(dest, "_t");
    strcat(dest, src);
    free(src);
    return dest;
}

void resettemp(void)
{
    temp_counter = 0;
}

struct symbol *newtemp(int line, symbol_t symb)
{
    char *name = newtempname();
    enum SymbolType temp;
    temp = (scope == 0) ? GLOBAL : LOCAL;
    if (SymTable_contains(symtable, name, temp, 0, 0) == false)
    {
        SymTable_insert(symtable, name, scope, line, LOCAL);
    }
    fprintf(file, "%s: offset:%d\n", name, currscopeoffset());
    struct symbol *sym = lookup(name, scope);
    if (sym == NULL)
    {
        return initialize_symbol(name, line, symb, scope);
    }
    else
    {
        return sym;
    }
}
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
void print_quad(quad q)
{
    const char *opcode_name = NULL;
    switch (q.op)
    {
    case assign:
        opcode_name = "assign";
        break;
    case add:
        opcode_name = "add";
        break;
    case mul:
        opcode_name = "mul";
        break;
    case div_op:
        opcode_name = "div_op";
        break;
    case sub:
        opcode_name = "sub";
        break;
    case mod:
        opcode_name = "mod";
        break;
    case uminus:
        opcode_name = "uminus";
        break;
    case if_eq:
        opcode_name = "if_eq";
        break;
    case if_less:
        opcode_name = "if_less";
        break;
    case if_lesseq:
        opcode_name = "if_lesseq";
        break;
    case if_greater:
        opcode_name = "if_greater";
        break;
    case if_geatereq:
        opcode_name = "if_geatereq";
        break;
    case if_noteq:
        opcode_name = "if_noteq";
        break;
    case call:
        opcode_name = "call";
        break;
    case ret:
        opcode_name = "ret";
        break;
    case funcstart:
        opcode_name = "funcstart";
        break;
    case funcend:
        opcode_name = "funcend";
        break;
    case tablegetelem:
        opcode_name = "tablegetelem";
        break;
    case tablesetelem:
        opcode_name = "tablesetelem";
        break;
    case getretval:
        opcode_name = "getretval";
        break;
    case tablecreate:
        opcode_name = "tablecreate";
        break;
    case param:
        opcode_name = "param";
        break;
    case jump:
        opcode_name = "jump";
        break;
    default:
        opcode_name = "unknown";
        return;
    }

    blue();
    printf("%-20s", opcode_name);
    reset();
    green();
    printf("%-20s", (q.result != NULL && q.result->sym != NULL) ? q.result->sym->name : " ");
    reset();
    yellow();
    if (q.arg1 != NULL)
    {
        if (q.arg1->type == constnum_e)
        {
            printf("%-20.2f", q.arg1->numConst);
        }
        else if (q.arg1->type == constbool_e)
        {
            printf("%-20s", (q.arg1->boolConst == 0) ? "FALSE" : "TRUE");
        }
        else if (q.arg1->type == conststring_e)
        {
            (q.arg1->strConst[0] != '\"') ? printf("\"%s\"%-*s", q.arg1->strConst, (int)(20 - strlen(q.arg1->strConst) - 2), "") : printf("%-20s", q.arg1->strConst);
            ;
        }
        else
        {
            if (q.arg1->sym != NULL)
            {
                printf("%-20s", q.arg1->sym->name);
            }
            else
            {
                printf("%-20s", " ");
            }
        }
    }
    else
    {
        printf("%-20s", " ");
    }
    reset();
    orange();
    if (q.arg2 != NULL)
    {
        if (q.arg2->type == constnum_e)
        {
            printf("%-20.2f", q.arg2->numConst);
        }
        else if (q.arg2->type == constbool_e)
        {
            printf("%-20s", (q.arg2->boolConst == 0) ? "FALSE" : "TRUE");
        }
        else if (q.arg2->type == conststring_e)
        {
            (q.arg2->strConst[0] != '\"') ? printf("\"%s\"%-*s", q.arg2->strConst, (int)(20 - strlen(q.arg2->strConst) - 2), "") : printf("%-20s", q.arg2->strConst);
            ;
        }
        else
        {
            if (q.arg2->sym != NULL)
            {

                printf("%-20s", q.arg2->sym->name);
            }
            else
            {
                printf("%-20s", " ");
            }
        }
    }
    else
    {
        printf("%-20s", " ");
    }
    reset();
    grey();

    printf("%-5d", q.label);
    reset();
    purple();

    printf("%-4d\n", q.line);
    reset();
}

void print_all_quads(void)
{
    red();
    printf("quads#\t");
    reset();
    blue();
    printf("opcode\t\t");
    reset();
    green();
    printf(" result\t\t\t");
    reset();
    yellow();
    printf("arg1\t\t");
    reset();
    orange();
    printf("  arg2\t\t  ");
    reset();
    grey();
    printf("    label");
    reset();
    purple();
    printf(" line\n");
    reset();
    printf("---------------------------------------------------\n");
    int index = 0;
   fix_jumps();
    for (int i = 0; i < total1; ++i)
    {
        if (quads[i].op == -1)
            continue;
        red();
        printf("%d:\t", index);
        index++;
        reset();
        print_quad(quads[i]);
    }
    printf("---------------------------------------------------\n");
}
void expand(void)
{
    int newSize = total + 1024;
    quads = (quad *)realloc(quads, newSize * sizeof(quad));
    assert(quads != NULL);
    total = newSize;
}

void enterscopespace(void)
{
    ++scopeSpaceCounter;
}

void exitscopespace(void)
{
    assert(scopeSpaceCounter > 0); 
    --scopeSpaceCounter;
}

scopespace_t currscopespace(void)
{
    if (scopeSpaceCounter == 1)
        return programvar;
    else if (scopeSpaceCounter % 2 == 0)
        return formalarg;
    else
        return functionlocal;
}

void inccurrscopeoffset(void)
{
    switch (currscopespace())
    {
    case programvar:
        ++programmVarOffset;
        break;
    case functionlocal:
        ++functionLocalOffset;
        break;
    case formalarg:
        ++formalArgOffet;
        break;
    default:
        assert(0); 
    }
}
void resetformalargsoffset()
{
    formalArgOffet = 0;
}
void resetfunctionlocalsoffset()
{
    functionLocalOffset = 0;
}
void restorecurrscopeoffset(unsigned offset)
{
    switch (currscopespace())
    {
    case programvar:
        programmVarOffset = offset;
        break;
    case functionlocal:
        functionLocalOffset = offset;
        break;
    case formalarg:
        formalArgOffet = offset;
        break;
    default:
        assert(0);
    }
}
unsigned currscopeoffset(void)
{
    switch (currscopespace())
    {
    case programvar:
        return programmVarOffset;
    case functionlocal:
        return functionLocalOffset;
    case formalarg:
        return formalArgOffet;
    default:
        assert(0);
    }
}

void emit(iopcode op, expr *arg1, expr *arg2, expr *result, unsigned label, unsigned line)
{
    if (currQuad == total)
    {
        expand();
    }
    quad *q = &quads[currQuad++];
    global_position = currQuad;
    if (op == -1)
        useless_quad++;
    q->op = op;
    q->arg1 = arg1;
    q->arg2 = arg2;
    q->result = result;
    q->label = label;
    q->line = line;
    total1++;
}
expr *make_lists(expr *e)
{
    e->list = (FT_list *)malloc(sizeof(FT_list));
    e->list->FalseList = (expr *)malloc(sizeof(expr));
    e->list->TrueList = (expr *)malloc(sizeof(expr));
    return e;
}
expr *new_expr(expr_t type)
{
    expr *e = (expr *)malloc(sizeof(expr));
    memset(e, 0, sizeof(expr));
    e->type = type;
    e = make_lists(e);
    return e;
}

expr *new_expr_constnum(double num)
{
    expr *e = new_expr(constnum_e);
    e->numConst = num;
    return e;
}

expr *new_expr_conststring(const char *str)
{
    expr *e = new_expr(conststring_e);
    e->strConst = strdup(str);
    return e;
}

expr *new_expr_constbool(unsigned int b)
{
    expr *e = new_expr(constbool_e);
    e->boolConst = !!b; 
    return e;
}

expr *lvalue_expr(symbol *sym)
{
    assert(sym);
    expr *e = (expr *)malloc(sizeof(expr));
    memset(e, 0, sizeof(expr));

    e->next = NULL;
    e->sym = sym;
    e = make_lists(e);
    switch (sym->type)
    {
    case var_s:
        e->type = var_e;
        break;
    case programmfunc_s:
        e->type = programfunc_e;
        break;
    case library_s:
        e->type = libraryfunc_e;
        break;
    default:
        assert(0);
    }

    return e;
}
expr *emit_iftableitem(expr *e, int line)
{
    if (e->type != tableitem_e)
        return e;
    else
    {
        expr *result = new_expr(var_e);
        result->sym = newtemp(e->sym->line, e->sym->type);
        emit(tablegetelem, e, e->index, result, 0, line);
        return result;
    }
}
expr *member_item(expr *lvalue, char *name, int line)
{
    lvalue = emit_iftableitem(lvalue, line);
    expr *item = new_expr(tableitem_e);
    item->sym = lvalue->sym;
    item->index = new_expr_conststring(name);
    return item;
}
expr *member_numConst(expr *lvalue, double d, int line)
{
    lvalue = emit_iftableitem(lvalue, line);
    expr *item = new_expr(tableitem_e);
    item->sym = lvalue->sym;
    item->index = new_expr_constnum(d);
    return item;
}
expr *member_string(expr *lvalue, char *name, int line)
{
    lvalue = emit_iftableitem(lvalue, line);
    expr *item = new_expr(tableitem_e);
    item->sym = lvalue->sym;
    item->index = new_expr_conststring(name);
    return item;
}
expr *member_boolean(expr *lvalue, unsigned b, int line)
{
    lvalue = emit_iftableitem(lvalue, line);
    expr *item = new_expr(tableitem_e);
    item->sym = lvalue->sym;
    item->index = new_expr_constbool(b);
    return item;
}

unsigned nextquad(void)
{
    return currQuad - useless_quad;
}
unsigned actual_nextquad(void)
{
    return currQuad;
}
symbol *initialize_symbol(char *id_name, int line, symbol_t symb, int scope)
{
    struct symbol *sym = malloc(sizeof(struct symbol));
    if (sym == NULL)
        exit(EXIT_FAILURE);
    sym->line = line;
    sym->type = symb;
    sym->name = strdup(id_name);
    sym->space = currscopespace();
    sym->scope = scope;
    sym->offset = currscopeoffset();
    inccurrscopeoffset();
    return sym;
}
extern expr *Head;
void insert(symbol *sym)
{
    expr *temp = lvalue_expr(sym);
    temp->next = Head;
    Head = temp;
    return;
}
struct symbol *lookup(char *name, int scope)
{
    expr *temp = Head;
    while (temp != NULL)
    {
        if (temp->sym->scope == scope && strcmp(temp->sym->name, name) == 0)
        {
            char space[100];
            if (temp->sym->space == programvar)
                strcpy(space, "programvar");
            else if (temp->sym->space == functionlocal)
                strcpy(space, "functionlocal");
            else if (temp->sym->space == formalarg)
                strcpy(space, "formalarg");
            char symbol[100];
            if (temp->sym->type == var_s)
                strcpy(symbol, "var_s");
            else if (temp->sym->type == programmfunc_s)
                strcpy(symbol, "programmfunc_s");
            else if (temp->sym->type == library_s)
                strcpy(symbol, "library_s");
            return temp->sym;
        }
        else
            temp = temp->next;
    }
    return NULL;
}

expr *insert_elist(expr *h, expr *e)
{
    expr *start = h;
    if (e == NULL)
    {
        h->next = NULL;
        return h;
    }
    if (h == NULL)
    {
        e->next = NULL;
        return e;
    }
    e->next = NULL;
    while (start->next != NULL)
    {
        start = start->next;
    }
    start->next = e;
    return h;
}
map *new_map(expr *index, expr *value)
{
    map *e = (map *)malloc(sizeof(map));
    memset(e, 0, sizeof(map));
    e->index = index;
    e->value = value;
    e->next = NULL;
    return e;
}
map *insert_indexed(map *h, map *e)
{
    map *start = h;
    while (start->next != NULL)
    {
        start = start->next;
    }
    start->next = e;
    return h;
}
expr_map *new_expression(int b, expr *exp)
{
    expr_map *e = (expr_map *)malloc(sizeof(int) + sizeof(expr));
    memset(e, 0, sizeof(int) + sizeof(expr));
    e->expr_or_map = b;
    e->expression = exp;
    if (exp != NULL)
        e->indexed->next = NULL;
    return e;
}
expr_map *new_mapping(int b, map *m)
{
    expr_map *e = (expr_map *)malloc(sizeof(int) + sizeof(map));
    memset(e, 0, sizeof(int) + sizeof(map));
    e->expr_or_map = b;
    e->indexed = m;
    if (m != NULL)
        return e;
}
void check_arith(expr *e, const char *context)
{
    if (e->type == constbool_e ||
        e->type == conststring_e ||
        e->type == nil_e ||
        e->type == newtable_e ||
        e->type == programfunc_e ||
        e->type == libraryfunc_e ||
        e->type == boolexpr_e)
        printf("Illegal expr used in %s!", context);
}
Call *new_Call(expr *elist, unsigned char method, char *name)
{
    Call *c = (Call *)malloc(sizeof(Call));
    memset(c, 0, sizeof(Call));
    c->elist = elist;
    c->method = method;
    if (name != NULL)
    {
        c->name = strdup(name);
    }
    else
    {
        c->name = NULL;
    }
    return c;
}
expr *get_last(expr *e)
{
    expr *temp = e;
    if (e == NULL)
        return NULL;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    return temp;
}
extern bool ddotParam;
expr *make_call(expr *lv, expr *reversed_elist, int line)
{
    int index = 0;
    int size = 10;
    expr *func = emit_iftableitem(lv, line);
    expr **reverse = (expr **)malloc(size * sizeof(expr *));
    while (reversed_elist)
    {
        if (index >= size)
        {
            size *= 2;
            expr **temp = (expr **)realloc(reverse, size * sizeof(expr *));
            reverse = temp;
        }
        reverse[index++] = reversed_elist;
        reversed_elist = reversed_elist->next;
    }
    if (ddotParam == true)
        index--;
    for (int i = index - 1; i >= 0; i--)
    {
        emit(param, reverse[i], NULL, NULL, 0, line);
    }
    if (ddotParam == true)
    {
        emit(param, reverse[index], NULL, NULL, 0, line);
        ddotParam = false;
    }
    emit(call, func, NULL, NULL, 0, line);
    expr *result = new_expr(var_e);
    result->sym = newtemp(line, var_s);
    emit(getretval, NULL, NULL, result, 0, line);
    free(reverse);
    return result;
}
unsigned int istempname(char *s)
{
    return *s == '_';
}
unsigned int istempexpr(expr *e)
{
    return e->sym && istempname(e->sym->name);
}
static void destory_quad(int index)
{
    quads[index].op = -1;
    quads[index].arg1 = NULL;
    quads[index].arg2 = NULL;
    quads[index].result = NULL;
    quads[index].label = -1;
    quads[index].line = -1;
}
static int check_expr_quad(expr *e)
{
    for (int i = 0; i < total1; i++)
    {
        if (quads[i].arg1 != NULL && quads[i].arg1 == e && quads[i].op == -1)
        {
            return i;
        }
        if (quads[i].arg2 != NULL && quads[i].arg2 == e && quads[i].op == -1)
        {
            return i;
        }
    }
    return -1;
}

void TranstionOfQuads(unsigned index, quad lastQuad)
{
    for (int i = total1 - 1; i > index; i--)
    {
        quads[i] = quads[i - 1];
        if (quads[i].label != 0)
        {
            quads[i].label++;
        }
    }
    quads[index] = lastQuad;
}
void update_false_true_lists(FT_list *e)
{
    expr *temp = e->FalseList;
    while (temp != NULL)
    {
        temp->lbl++;
        temp = temp->next;
    }
    temp = e->TrueList;
    while (temp != NULL)
    {
        temp->lbl++;
        temp = temp->next;
    }
}
expr *expr_num_type(unsigned arithop, expr *e1, expr *e2, int line)
{
    expr *e = NULL;
    if (e1 == NULL || e2 == NULL)
        return NULL;
    if ((e1->type == programfunc_e || e1->type == libraryfunc_e || e1->type == boolexpr_e || e1->type == newtable_e || e1->type == constbool_e || e1->type == conststring_e || e1->type == nil_e) || (e2->type == programfunc_e || e2->type == libraryfunc_e || e2->type == boolexpr_e || e2->type == newtable_e || e2->type == constbool_e || e2->type == conststring_e || e2->type == nil_e))
    {
        red();
        printf("This type of expr cannot be an arithmetic expr:%d\n", line);
        reset();
    }
    else
    {
        if (e1->type == constnum_e && e2->type == constnum_e)
        {
            switch ((iopcode)arithop)
            {
            case (add):
                e = new_expr_constnum(e1->numConst + e2->numConst);
                break;
            case (sub):
                e = new_expr_constnum(e1->numConst - e2->numConst);
                break;
            case (div_op):
                e = new_expr_constnum(e1->numConst / e2->numConst);
                break;
            case (mul):
                e = new_expr_constnum(e1->numConst * e2->numConst);
                break;
            default:
                e = new_expr_constnum((double)((int)e1->numConst % (int)e2->numConst));
                break;
            }
            e->sym = newtemp(line, var_s);
            int index_e1 = check_expr_quad(e1);
            int index_e2 = check_expr_quad(e2);
            if (index_e1 != -1)
            {
                destory_quad(index_e1);
            }
            if (index_e2 != -1)
            {
                destory_quad(index_e2);
            }
            emit(arithop, e1, e2, e, 0, line);
            return e;
        }
        else
        {
            e = new_expr(arithexpr_e);
            e->sym = newtemp(line, var_s);
            emit(arithop, e1, e2, e, 0, line);
            return e;
        }
    }
}
expr *assign_expr(expr *assign_e, expr *lval, expr *e, int line)
{
    if (lval == NULL || e == NULL)
    {
        assign_e = NULL;
        return assign_e;
    }
    if (lval->type == tableitem_e)
    {
        emit(tablesetelem, lval->index, e, lval, 0, line);
        assign_e = emit_iftableitem(lval, line);
        assign_e->type = assign;
    }
    else
    {
        emit(assign, e, NULL, lval, 0, line);
        assign_e = new_expr(assign);
        assign_e->sym = newtemp(line, var_s);
        emit(assign, lval, NULL, assign_e, 0, line);
    }
    return assign_e;
}
void patchlabel(unsigned quadNo, unsigned label)
{
    assert(quadNo < currQuad);
    if (quads[quadNo].label == 0)
        quads[quadNo].label = label;
}
stmt_t *make_stmt(stmt_t *s)
{
    if (s != NULL)
    {
        // s->breakList = s->contList = 0;
    }
    else
    {
        s = malloc(sizeof(stmt_t));
        s->breakList = s->contList = 0;
    }
    return s;
}
int newlist(int i)
{
    quads[i].label = 0;
    return i;
}
int mergelist(int l1, int l2)
{
    if (!l1)
        return l2;
    else if (!l2)
        return l1;
    else
    {
        int i = l1;
        while (quads[i].label)
            i = quads[i].label;
        quads[i].label = l2;
        return l1;
    }
}
void patchlist(int list, int label)
{
    while (list)
    {
        int next = quads[list].label;
        quads[list].label = label;
        list = next;
    }
}
forpref *make_for_pref(unsigned int test, unsigned int enter)
{
    forpref *f = (forpref *)malloc(sizeof(forpref));
    f->test = test;
    f->enter = enter;
    return f;
}

void BackPatch(expr *e, unsigned int label)
{
    expr *temp = e;
    while (temp != NULL)
    {
        patchlabel(temp->lbl, label);
        temp = temp->next;
    }
}
expr *merge(expr *e1, expr *e2)
{
    if (e1 == NULL)
        return e2;
    if (e2 == NULL)
        return e1;
    expr *temp = e1;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = e2;
    return e1;
}
int create_relop_quad(iopcode op, expr *arg1, expr *arg2, expr *result, unsigned label, unsigned line)
{
    emit(op, arg1, arg2, result, label, line);
    emit(jump, NULL, NULL, NULL, 0, line);
    expr *flist = NULL;
    expr *tlist = NULL;
    int index_e1 = check_expr_quad(arg1);
    int index_e2 = check_expr_quad(arg2);
    if (index_e1 != -1)
    {
        quads[index_e1] = quads[total1 - 2]; //-2 giati exei ginei kai to jump
        if (result == NULL)
        {
            arg1->list->TrueList->lbl = index_e1;
            arg1->list->FalseList->lbl = index_e1 + 1;
        }
        else
        {
            result->list->TrueList->lbl = index_e1;
            result->list->FalseList->lbl = index_e1 + 1;
        }
        quads[total1 - 2].op = -1; // to axristeuo gia to print
        TranstionOfQuads(index_e1 + 1, quads[total1 - 1]);
    }
    else
    {
        if (result != NULL)
        {
            result->list->TrueList->lbl = global_position - 2;
            result->list->FalseList->lbl = global_position - 1;
        }
    }
    return index_e1;
}
void true_jump_false(int line, expr *e)
{

    if (e->type == boolexpr_e /*|| e->type == constbool_e*/)
    {
        expr *t0 = new_expr(boolexpr_e);
        t0->sym = newtemp(line, var_s);
        unsigned int n1 = nextquad();
        unsigned int n2 = nextquad() + 2;
        emit(assign, new_expr_constbool(1), t0, NULL, 0, line);
        emit(jump, NULL, NULL, NULL, nextquad() + 2, line);
        emit(assign, new_expr_constbool(0), t0, NULL, 0, line);
        BackPatch(e->list->TrueList, n1);
        BackPatch(e->list->FalseList, n2);
    }
}
struct af_quad *new_Af_quad(int nextquad, int actualquad)
{
    af_quad *a = malloc(sizeof(af_quad));
    a->actualquad = actualquad;
    a->nextquad = nextquad;
    return a;
}
void fix_jumps()
{
    quad *q = NULL;
    int valid_quads = 0;
    for (int i = 0; i < global_position; i++)
    {
        if (quads[i].op >= 0 && quads[i].op <= 25)
        {
            quad *temp_quad = realloc(q, sizeof(quad) * (valid_quads + 1));
            q = temp_quad;
            q[valid_quads] = quads[i];
            valid_quads++;
        }
    }
    for (int i = 0; i < valid_quads; i++)
    {
        if (q[i].op == jump)
        {
            if (i != 0)
            {
                if (q[i - 1].op == ret)
                {
                    if (q[q[i].label].op != funcend)
                    {
                        int counter = 0;
                        for (int j = i+1; j <=valid_quads; j++)
                        {
                            if (q[j].op == funcend)
                            {
                                break;
                            }
                            counter++;
                        }
                        q[i].label = q[i].label + counter;
                    }
                }
            }
        }
    }
    int counter = 0;
    for (int i = 0; i < global_position; i++)
    {
        if (quads[i].op >= 0 && quads[i].op <= 25)
        {
            if (quads[i].op == jump)
            {
                quads[i] = q[counter];
            }
            counter++;
        }
    }
}
