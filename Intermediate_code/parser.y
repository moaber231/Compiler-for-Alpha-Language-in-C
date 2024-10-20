%{
#define SEARCH_ALL -1
#include "src/symtable/symtable.h"
#include "src/stack/stack.h"
#include "src/expr/expr.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int yyerror(char *yaccProviderMessage);
#define yylex alpha_yylex
extern int alpha_yylex(void);
extern int alpha_yylineno;
extern char *alpha_yytext;
extern FILE *alpha_yyin;
FILE *file;
char *var_fun=NULL;
int temp_off=-1;
int var_fun_type=0;
int last_func_global=-1;
bool print_error=false;
bool declare_var=false;
int isIteration=0;
int anonymous=1;
int dot_var=0;
int argument_counter=0;
extern unsigned int programmVarOffset;
extern unsigned int functionLocalOffset;
stmt_t* tmp_stmt;
expr *tmp_Expr;
stmt_t* tmp_stmt1;
struct forpref * tmp_for;
int keyword_bc=0;
int keyword_ret=0;
char dollar[100001]="$";
bool lvalue_callsuffix=true;
int scope=0;
int inside_function=0;
bool ddotParam=false;
SymTable_T symtable;
expr * Flist=NULL;
expr * Tlist=NULL;
int jump_function=-1;
int flag_and=-1;
int flag_or=-1;
struct StackNode* stack_function = NULL;
struct StackNode* offset_save = NULL;
struct StackNode* call_name = NULL;
struct StackNode* func_jump = NULL;
struct StackNode* loop_counter = NULL;
bool check_libfunc(char*); 
void inttostring(char *str, int num);

expr* Elist=NULL;
expr* Head=NULL;
symbol* sym=NULL;
int *temp_int=NULL;
char *temp_string=NULL;
float *temp_float=NULL;
 expr *em;

static void red ()
{
  printf("\033[1;31m");
}
static void reset ()
{
  printf("\033[0m");
}
void printError(char *error)
{
    if(print_error==true)
    {
        red();
        printf("%s %s line: %d\n",error,var_fun,alpha_yylineno);
        reset();        
        print_error=false;
    }
    return; 
}
#define YYDEBUG 1 
%}
%union {
    struct expr *sval;
    struct symbol *tval;
    struct Call * mval;
    struct map *ind;
    struct stmt_t* stm;
    struct expr_map *sval_ind;
    struct af_quad * af_Quad;
    char *stringValue;
    int intValue;
    unsigned int uintValue;
    float realValue;
    struct forpref * for_str;
}

%start programm



%token geq ">="
%token bne "!="
%token seq "<="
%token beq "=="
%token incr "++"
%token decr "--"
%token ddot ".."
%token greater '>'
%token smaller '<'
%token dcolon "::"
%token semicolon ';'
%token comma ','
%token lcurly '{'
%token lparenthesis '(' 
%token lbracket '['
%token rcurly '}'
%token rparenthesis ')' 
%token rbracket ']'

%token If "if"
%token Else "else"
%token For "for"
%token While "while"
%token Return "return"
%token Break "break"
%token Continue "continue"
%token Function "function"
%token And "and"
%token Or "or"
%token Not "not"
%token Nil "nil"
%token True "true"
%token False "false"
%token Plus '+'
%token Minus '-'
%token Mult '*'
%token Div '/'
%token Mod '%'
%token Assign '='
%token Dot '.'
%token Colon ':'
%token Local "local"


%right Assign
%left Or
%left And 
%nonassoc bne beq
%nonassoc geq seq greater smaller 
%left Minus Plus
%left Mult Div Mod
%right Not incr decr UMINUS  
%left Dot ddot
%left lbracket rbracket
%left lparenthesis rparenthesis
%token <stringValue> variable 
%token <intValue> integer 
%token <realValue> real 
%token <stringValue> String

%type <stringValue> funcname
%type <intValue> blockFunction;  
%type <tval>  funcprefix
%type <mval>  methodcall
%type <mval>  callsuffix
%type <sval>  call
%type <mval>  normcall//front 27/3,28/38
%type <tval> funcdef_par
%type <tval> funcdef
%type <sval> assignexpr
%type <sval> lvalue
%type <sval> const
%type <sval> expr
%type <sval> expr_semicolon
%type <sval> objectdef
%type <sval> term
%type <sval> primary
%type <sval> elist_comma
%type <sval> elist
%type <sval> member
%type <ind> indexed
%type <ind> indexelem
%type <ind> indexelem_comma
%type <sval_ind> elist_indexed
%type <uintValue> ifprefix
%type <af_Quad> elseprefix
%type <uintValue> whilestart
%type <uintValue> whilecond
%type <uintValue> N
%type <uintValue> Nb
%type <uintValue> Na
%type <uintValue> Nn
%type <uintValue> Nc
%type <uintValue> M
%type <stm> block
%type <stm> stmt
%type <stm> stmts
%type <stm> cont
%type <stm> brk
%type <stm> ifstmt
%type <for_str> forprefix
%%

programm:
    stmts {  }
    ;

brk:Break semicolon
{
            fprintf(file,"Break semicolon\n");
            if(keyword_bc<=0)
            {
                red();
                printf("Cannot use break outside of a loop\n");
                reset();
            }
            $$=NULL;
            $$=make_stmt($$);
            $$->breakList = newlist(actual_nextquad()); 
           emit(jump,NULL,NULL,NULL,0,alpha_yylineno);
}
cont:Continue semicolon
{
    fprintf(file,"Continue semicolon\n");
      if(keyword_bc<=0)
        {
            red();
            printf("Cannot use continue outside of a loop\n");
            reset();
        }
        $$=NULL;
        $$=make_stmt($$);
        $$->contList = newlist(actual_nextquad()); 
       emit(jump,NULL,NULL,NULL,0,alpha_yylineno);
}
stmt:
     expr_semicolon{$$=NULL; resettemp(); }
    | ifstmt { $$=$1;resettemp();}
    | whilestmt{$$=NULL;resettemp(); }
    | forstmt { $$=NULL;resettemp();}
    | returnstmt{$$=NULL;resettemp();}
    | brk{$$=$1;resettemp();}
    | cont{$$=$1;resettemp();}
    | funcdef{$$=NULL;resettemp(); }
    | block{$$=$1;resettemp(); }
    ;

stmts:
    stmt 
    {
        if($1!=NULL)
        {
            $$=make_stmt($$);
            $$=$1;

        }
       resettemp();
    }
    |
     stmts stmt 
     { 
        if($2!=NULL)
        {
            $$=make_stmt($$);
            $1=make_stmt($1);
            $$->breakList = mergelist($1->breakList, $2->breakList);
            $$->contList = mergelist($1->contList, $2->contList);
        }
       resettemp();

     }
    ;
N:{ $$=actual_nextquad();emit(jump,NULL,NULL,NULL,0,alpha_yylineno); };
M:{ $$=nextquad(); };
expr_semicolon:
    expr semicolon{$$=$1; true_jump_false(alpha_yylineno,$1);fprintf(file,"expr semicolon\n");}
    | semicolon{$$=NULL;};

expr:
    assignexpr {$$=$1;fprintf(file,"assignexpr\n");}
    | expr Plus expr { $$=expr_num_type(add, $1, $3, alpha_yylineno);fprintf(file,"expr Plus expr\n");}  /*istempexpr($expr) ? $expr->sym : PROERAITIKO*/    
    | expr Minus expr { $$=expr_num_type(sub, $1, $3, alpha_yylineno); fprintf(file,"expr Minus expr\n");}
    | expr Mult expr {$$=expr_num_type(mul, $1, $3, alpha_yylineno);fprintf(file,"expr Minus expr\n");}
    | expr Div expr {$$=expr_num_type(div_op, $1, $3, alpha_yylineno); fprintf(file,"expr Div expr\n");}
    | expr Mod expr { $$=expr_num_type(mod, $1, $3, alpha_yylineno);fprintf(file,"expr Mod expr\n");}
    | expr smaller{ true_jump_false(alpha_yylineno,$1);} expr {  true_jump_false(alpha_yylineno,$4);$$ = new_expr(boolexpr_e);/*$$->sym = newtemp(alpha_yylineno,var_s);*/create_relop_quad(if_less, $1, $4, $$,0,alpha_yylineno);fprintf(file,"expr smaller expr\n");}
    | expr greater{ true_jump_false(alpha_yylineno,$1);} expr {  true_jump_false(alpha_yylineno,$4);$$ = new_expr(boolexpr_e);/*$$->sym = newtemp(alpha_yylineno,var_s);*/create_relop_quad(if_greater, $1, $4, $$,0,alpha_yylineno); fprintf(file,"expr greater expr\n");}
    | expr geq { true_jump_false(alpha_yylineno,$1);} expr { true_jump_false(alpha_yylineno,$4);$$ = new_expr(boolexpr_e);/*$$->sym = newtemp(alpha_yylineno,var_s);*/create_relop_quad(if_geatereq, $1, $4, $$,0,alpha_yylineno);fprintf(file,"expr geq expr\n");}
    | expr seq { true_jump_false(alpha_yylineno,$1);}expr { true_jump_false(alpha_yylineno,$4);$$ = new_expr(boolexpr_e);/*$$->sym = newtemp(alpha_yylineno,var_s);*/create_relop_quad(if_lesseq, $1, $4, $$,0,alpha_yylineno);fprintf(file,"expr seq expr\n");}
    | expr beq { true_jump_false(alpha_yylineno,$1);}expr { 
        true_jump_false(alpha_yylineno,$4);
        $$ = new_expr(boolexpr_e);/*$$->sym = newtemp(alpha_yylineno,var_s);*/create_relop_quad(if_eq, $1, $4, $$,0,alpha_yylineno);  fprintf(file,"expr beq expr\n"); }
    | expr bne { true_jump_false(alpha_yylineno,$1);}expr { true_jump_false(alpha_yylineno,$4);$$ = new_expr(boolexpr_e);/*$$->sym = newtemp(alpha_yylineno,var_s);*/create_relop_quad(if_noteq, $1, $4, $$,0,alpha_yylineno);fprintf(file,"expr bne expr\n");}
    |  expr And { 
        fprintf(file,"expr and expr\n");
        ($1->type!=boolexpr_e)?create_relop_quad(if_eq,$1,new_expr_constbool(1),NULL,0,alpha_yylineno):(void)0;
        }
     M expr{
        $$ = new_expr(boolexpr_e);
        $$->sym = newtemp(alpha_yylineno,var_s);
        unsigned int  next_quad=$4;
        expr *expr1=$1;
        if($5->type!=boolexpr_e)
        {
            create_relop_quad(if_eq,$5,new_expr_constbool(1),NULL,0,alpha_yylineno);
        }
        expr1 =$1;
        expr *expr2=$5;
        BackPatch($1->list->TrueList,next_quad);
        $$->list->TrueList=$5->list->TrueList;
        $$->list->FalseList=merge($1->list->FalseList,$5->list->FalseList);
         expr *ee=$$;
         expr1=NULL;
    }
    | expr Or {  
        fprintf(file,"expr Or expr\n");
        ($1->type!=boolexpr_e ) ? create_relop_quad(if_eq,$1,new_expr_constbool(1),NULL,0,alpha_yylineno):(void)0;
        }
        M expr{
        $$ = new_expr(boolexpr_e);
        int flag=-1;
        expr *e1=$1;
        expr *e2=$5;
        $$->sym = newtemp(alpha_yylineno,var_s);
        unsigned int  next_quad= $4;
        if($5->type!=boolexpr_e )
        {
            create_relop_quad(if_eq, $5,new_expr_constbool(1),NULL,0,alpha_yylineno);
        }
        BackPatch($1->list->FalseList ,next_quad);
        $$->list->TrueList=merge($1->list->TrueList,$5->list->TrueList);
        $$->list->FalseList=$5->list->FalseList;
        expr *eol=$$;
        next_quad=0;
    }
    | 
    term  
    {
        fprintf(file,"term\n");
        $$=$1;
        if($1->type!=boolexpr_e)
        {
            emit(-1,$1,new_expr_constbool(1),NULL,0,alpha_yylineno);
        }
    }
    ;


assignexpr:
    lvalue Assign expr {
        fprintf(file," lvalue Assign expr \n");
        printError("Cannot use lvalue variable: ");
        if(declare_var==true)
        {
            SymTable_insert(symtable,var_fun,scope,alpha_yylineno,LOCAL);
            declare_var=false;
            sym=initialize_symbol(var_fun,alpha_yylineno,var_s,scope);
            insert(sym);
        }
        true_jump_false(alpha_yylineno,$3);
        $$=assign_expr($$, $1, $3,alpha_yylineno);
    }
    ;
term:
    lparenthesis expr rparenthesis 
    { 
        fprintf(file,"lparenthesis expr rparenthesis\n");
         $$=$2;
    }
    | Minus expr %prec UMINUS  {
        fprintf(file," Minus expr prec UMINUS\n");
        check_arith($$, "unary minus");
        $$ = new_expr(arithexpr_e);
        $$->sym = istempexpr($2) ? $2->sym : newtemp(alpha_yylineno,var_s);
        emit(uminus,$2, NULL, $$,0,alpha_yylineno);
        }
    | 
    Not expr
    {
            fprintf(file," Not expr\n");
            $$ = new_expr(boolexpr_e);
            $$->sym =  istempexpr($2) ? $2->sym : newtemp(alpha_yylineno,var_s);
            if($2->type!=boolexpr_e )
            {
                create_relop_quad(if_eq,$2,new_expr_constbool(1),NULL,0,alpha_yylineno);
            }
            
            $$->list->FalseList=$2->list->TrueList;
           $$->list->TrueList=$2->list->FalseList; 
    }
    | incr lvalue{
         fprintf(file,"  incr lvalue\n");
        if(check_libfunc(var_fun)==true)print_error=true;
         printError("Cannot pre-increase this variable: ");
          check_arith($2, "lvalue++");
         if ($2->type == tableitem_e) {
                $$ = emit_iftableitem($2,alpha_yylineno);
                emit(add, $$, new_expr_constnum(1), $$,0,alpha_yylineno);
                emit(tablesetelem,$2->index,$$,$2,0,alpha_yylineno);
            }
            else {
                emit(add, $2, new_expr_constnum(1), $2,0,alpha_yylineno);
                $$ = new_expr(arithexpr_e);
                $$->sym = newtemp(alpha_yylineno,var_s);
                emit(assign, $2, NULL, $$,0,alpha_yylineno);
            }
          
          }
    | lvalue  incr{
         fprintf(file," lvalue incr \n");
        if(check_libfunc(var_fun)==true)print_error=true;printError("Cannot post-increase this variable: ");
            check_arith($1, "++lvalue");
            $$ = new_expr(arithexpr_e);
            $$->sym = newtemp(alpha_yylineno,var_s);
            if ($1->type == tableitem_e) {
                    expr* e = emit_iftableitem($1,alpha_yylineno);
                    emit(assign, e, NULL, $$,0,alpha_yylineno);
                    emit(add, e ,new_expr_constnum(1),e,0,alpha_yylineno);
                    emit(tablesetelem,$1->index,e,$1,0,alpha_yylineno);
                }
                else {
                    emit(assign, $1, NULL, $$,0,alpha_yylineno);
                    emit(add, $1, new_expr_constnum(1), $1,0,alpha_yylineno);
            }
    }
    | decr lvalue{
         fprintf(file,"  decr lvalue\n");
        if(check_libfunc(var_fun)==true)print_error=true;printError("Cannot pre-decrease this variable:");
        check_arith($2, "lvalue--");
            assert($2->sym);
            if ($2->type == tableitem_e) {
                $$ = emit_iftableitem($2,alpha_yylineno);
                emit(sub, $$, new_expr_constnum(1), $$,0,alpha_yylineno);
                emit(tablesetelem,$2->index, $$,$2,0,alpha_yylineno);
            }
            else {
                emit(sub, $2, new_expr_constnum(1), $2,0,alpha_yylineno);
                $$ = new_expr(arithexpr_e);
                $$->sym = newtemp(alpha_yylineno,var_s);
                emit(assign, $2, NULL, $$,0,alpha_yylineno);
            }
    }
    | lvalue decr{
         fprintf(file,"lvalue decr\n");
        if(check_libfunc(var_fun)==true)print_error=true;printError("Cannot post-decrease this variable:");
      check_arith($1, "--lvalue");
            $$ = new_expr(arithexpr_e);
            $$->sym = newtemp(alpha_yylineno,var_s);
            if ($1->type == tableitem_e) {
                    expr *e = emit_iftableitem($1,alpha_yylineno);
                    emit(assign,e, NULL, $$,0,alpha_yylineno);
                    emit(sub, e, new_expr_constnum(1), e,0,alpha_yylineno);
                    emit(tablesetelem,$1->index,e,$1,0,alpha_yylineno);
                }
                else {
                    emit(assign, $1, NULL, $$,0,alpha_yylineno);
                    emit(sub, $1, new_expr_constnum(1), $1,0,alpha_yylineno);
            }
        } 
    | primary{ fprintf(file,"primary\n");$$=$1; }
    ;
funcdef_par:lparenthesis funcdef rparenthesis{ fprintf(file,"lparenthesis funcdef rparenthesis\n");$$=$2;}
;

primary:
    lvalue {fprintf(file,"lvalue\n");printError("Cannot use primary variable: ");$$ = emit_iftableitem($1,alpha_yylineno); }
    | call{$$=$1; fprintf(file,"call\n");}
    | objectdef{$$=$1;  fprintf(file,"objectdef\n");}
    | funcdef_par{fprintf(file,"funcdef_par\n");$$ = new_expr(programfunc_e);$$->sym = $1;}
    | const{$$=$1;fprintf(file,"const\n");}
    ;

lvalue:
    variable {
    fprintf(file,"variable\n");
    fprintf(file,"%s: offset_var:%d",var_fun,currscopeoffset());
    if(var_fun!=NULL)free(var_fun);
    var_fun=strdup($1);
    if(inside_function==0)/*BLOCK*/
    {//elenxops yparkeis olon ton metavliton
        if(SymTable_contains(symtable,$1,GLOBAL,SEARCH_ALL,inside_function)==false )
        {
            print_error=false;
            SymTable_insert(symtable,$1,scope,alpha_yylineno,LOCAL); 
            if(scope!=0)var_fun_type=LOCAL;
            else{var_fun_type=GLOBAL;}
            //new code
            sym=initialize_symbol($1,alpha_yylineno,var_s,scope);
            insert(sym);//na to valw kai sta alla
             $$ = lvalue_expr(sym);
            push(&call_name,var_fun,-1,$$);
        }
        else if(check_libfunc($1)==false && SymTable_contains(symtable,$1,USERFUNC,SEARCH_ALL,inside_function)==false)
        {
            red();    
            printf("asxeto error LINE: %d \n",alpha_yylineno);
            reset();
        }
        else if (check_libfunc($1)==false && SymTable_contains(symtable,$1,LOCAL,scope,inside_function)==false && 
            SymTable_contains(symtable,$1,GLOBAL,scope,inside_function)==false &&
            SymTable_contains(symtable,$1,USERFUNC,scope,inside_function)==true)
        {
            print_error=true;
        }
        else 
        {//searching for $1 in struct symbol
            int i;
            for(i=scope;i>-1;i--)
            {
                sym=lookup($1,i);
                if(sym!=NULL)
                {
                    $$ = lvalue_expr(sym);
                    push(&call_name,var_fun,-1,$$);

                }
            }
            if(check_libfunc($1)==true && sym==NULL)
            {
                    sym=initialize_symbol($1,alpha_yylineno,library_s,0);
                    insert(sym);
                    $$ = lvalue_expr(sym);
                    push(&call_name,var_fun,-1,$$);
             }
            print_error=false;
        }
    } 
    else/*Into function*/
    {
        if(isIteration!=0)/*if while for*/
        {
            int temp=0;
            while(temp!=(isIteration+1))/*from scope to scope-iteration kai scope=0 check for variables */
            {
                if(SymTable_contains(symtable,$1,FORMAL,scope-temp,false)==true || 
                    SymTable_contains(symtable,$1,USERFUNC,scope-temp,false)==true || 
                    SymTable_contains(symtable,$1,LOCAL,scope-temp,false)==true )
                {
                    int i=0;
                       for(i=scope;i>-1;i--)
                        {
                        sym=lookup($1,i);
                        if(sym!=NULL)
                        {
                            $$ = lvalue_expr(sym);
                            push(&call_name,var_fun,-1,$$);
                        }   
                    }
                    break;
                }
                temp++;
            }
            if(SymTable_contains(symtable,$1,USERFUNC,0,false)==false && 
                SymTable_contains(symtable,$1,GLOBAL,0,false)==false && 
                temp == isIteration+1 && check_libfunc($1)==false)
            {//Se periptosi pou eimaste mesa se sinartisi dilonontai {id} kai otan den anagnwrizontai diladi return x*y otan to y den exei vrethei pio prin
                SymTable_insert(symtable,$1,scope,alpha_yylineno,LOCAL);
                //new code 
                sym=initialize_symbol($1,alpha_yylineno,var_s,scope);
                insert(sym);
                  $$ = lvalue_expr(sym);
                push(&call_name,var_fun,-1,$$);


            }
            else if(SymTable_contains(symtable,$1,USERFUNC,0,false)==false && 
                SymTable_contains(symtable,$1,GLOBAL,0,false)==false )
            {//searching for $1 in struct symbol
                int temp=0;
                while(temp!=(isIteration+1))
                {
                    sym=lookup($1,scope-temp);
                    if(sym!=NULL)$$=lvalue_expr(sym);
                    temp++;
                }
                if(check_libfunc($1)==true && sym==NULL)
                {
                    sym=lookup($1,0);
                    if(sym==NULL)
                    {
                        sym=initialize_symbol($1,alpha_yylineno,library_s,0);
                        insert(sym);
                        $$ = lvalue_expr(sym);
                        push(&call_name,var_fun,-1,$$);
                    }
                }

            }      
        }
        else 
        {
            int function_scope=peek(stack_function);
            int temp_scope=scope;
            bool found=false;
            while(temp_scope>=function_scope)
            {
                if(SymTable_contains(symtable,$1,LOCAL,temp_scope,false)==true || SymTable_contains(symtable,$1,FORMAL,temp_scope,false)==true)break;
                else temp_scope--;
            }
            while(function_scope>=1)
            {
                if(SymTable_contains(symtable,$1,LOCAL,function_scope,false)==true || SymTable_contains(symtable,$1,FORMAL,function_scope,false)==true)
                {
                    found=true;
                    break;
                }
                function_scope--;
            }
            function_scope=peek(stack_function);
            if(found==true && check_libfunc($1)==false)
            {
                red();
                printf("Function/variable1 does not exist: %s line: %d\n",$1,alpha_yylineno);
                reset();
            }
            else if((temp_scope+1)==function_scope && SymTable_contains(symtable,$1,USERFUNC,0,false)==false && SymTable_contains(symtable,$1,GLOBAL,0,false)==false)
            {
                SymTable_insert(symtable,$1,scope,alpha_yylineno,LOCAL);
                //new code
                sym=initialize_symbol($1,alpha_yylineno,var_s,scope);
                insert(sym);   
                $$ = lvalue_expr(sym);
                push(&call_name,var_fun,-1,$$);

            }
            else if(SymTable_contains(symtable,$1,USERFUNC,0,false)==false )
            {
                temp_scope=scope;
                function_scope=peek(stack_function);
                while(temp_scope>=function_scope)
                {
                    sym=lookup($1,temp_scope);
                    if(sym!=NULL)
                    {
                        $$ = lvalue_expr(sym);
                        push(&call_name,var_fun,-1,$$);
                    }
                   temp_scope--;
                }
                if(check_libfunc($1)==true && sym==NULL)
                {
                    sym=initialize_symbol($1,alpha_yylineno,library_s,0);
                    insert(sym);
                    $$ = lvalue_expr(sym);
                    push(&call_name,var_fun,-1,$$);

                }
            }
        }

    }
    }
    | Local variable  {
        fprintf(file,"Local variable\n");
        if(var_fun!=NULL)free(var_fun);
        var_fun=strdup($2);
       
        if(check_libfunc($2)==true && scope!=0)
        {
            red();
            printf("Using LIBFUNC name: %s LINE %d\n",$2,alpha_yylineno);
            reset();
        }
        else if(check_libfunc($2)==true && scope==0){}
        else if(scope ==0)
        {
            if(SymTable_contains(symtable,$2,GLOBAL,scope,false)==false && SymTable_contains(symtable,$2,USERFUNC,scope,false)==false){//an local var elexneis an yparxeis gia na se diloseis
                SymTable_insert(symtable,$2,scope,alpha_yylineno,GLOBAL);
                //new code
                sym=initialize_symbol($2,alpha_yylineno,var_s,scope);
                insert(sym);
             $$ = lvalue_expr(sym);   

            }
            else
            {
                sym=lookup($2,scope);
            }
        }
        else
        {
            if(SymTable_contains(symtable,$2,FORMAL,scope,false)==false && 
                SymTable_contains(symtable,$2,USERFUNC,scope,false)==false && 
                SymTable_contains(symtable,$2,LOCAL,scope,false)==false){    //an eisai se scope!=0 koitas kai formal kai local sto scope pou eisai
                        
                    SymTable_insert(symtable,$2,scope,alpha_yylineno,LOCAL);
                    sym=initialize_symbol($2,alpha_yylineno,var_s,scope);
                    insert(sym);
                     $$ = lvalue_expr(sym);   

            }
            else
            {
                sym=lookup($2,scope);
                 $$ = lvalue_expr(sym);   

            }
        }

    }
    | dcolon variable {
        fprintf(file,"dcolon variable\n");
        if(var_fun!=NULL)free(var_fun);
        var_fun=strdup($2);
      if(SymTable_contains(symtable,$2,GLOBAL,0,false)==false && 
            (SymTable_contains(symtable,$2,USERFUNC,0,inside_function)==false && 
                SymTable_get(symtable,$2,USERFUNC,0)==NULL))
        {
            $$ = NULL;
            red();
            printf("no global variable %s found line: %d",$2,alpha_yylineno);
            reset();
        }
        else
        {
             sym=lookup($2,0);
             $$ = lvalue_expr(sym);   
        }
    }
    | member{
        fprintf(file,"member\n");
        expr*e=$1;
        $$=$1;
    }
    ;

member:
    lvalue Dot variable{
            fprintf(file,"lvalue Dot variable\n");
           if(check_libfunc(var_fun)==true)print_error=true;printError("lvalue.variable cannot be used: ");
            dot_var=1;
            $$=member_item($1,$3,alpha_yylineno);
        }
    | 
    lvalue lbracket expr rbracket{
            fprintf(file,"lvalue lbracket expr rbracket\n");
            if(check_libfunc(var_fun)==true)print_error=true;printError("lvalue[expr] cannot be used here: ");
           if($3->type==arithexpr_e || $3->type==constnum_e )
           {
             $$=member_numConst($1,$3->numConst,alpha_yylineno);
           }
           else if($3->type==conststring_e)
           {
             $$=member_string($1,$3->strConst,alpha_yylineno);
                
           }
           else if($3->type==constbool_e || $3->type==boolexpr_e)
           {
             $$=member_boolean($1,$3->boolConst,alpha_yylineno);
           }
           else
           {
                $1=emit_iftableitem($1,alpha_yylineno);
                $$=new_expr(tableitem_e);
                $$->sym=$1->sym;
                $$->index=$3;
           }
           true_jump_false(alpha_yylineno,$3);
        }
    | 
    call lbracket expr rbracket
    {   
            fprintf(file,"call lbracket expr rbracket\n");
            $1=emit_iftableitem($1,alpha_yylineno);
            $$=new_expr(tableitem_e);
            $$->sym=$1->sym;
            $$->index=$3;
            true_jump_false(alpha_yylineno,$3);
    }
    | call Dot variable{
        fprintf(file," call Dot variable\n");
        if(var_fun!=NULL)free(var_fun);    
        var_fun=strdup($3);
        $$=member_item($1,$3,alpha_yylineno);
    }
    ;

call:
    call lparenthesis elist rparenthesis { 
        fprintf(file," call lparenthesis elist rparenthesise\n");
        $$=make_call($1,$3,alpha_yylineno);
     }
    | lvalue callsuffix{
        fprintf(file,"lvalue callsuffix\n");
        expr* temp;
        expr *e_c=$1;
        Call *e_c1=$2;
        int check_iftable=-1;
        if($1!=NULL)
        {
            if($1->type==tableitem_e)check_iftable=0;
        }
        if(var_fun!=NULL)
        {
            int temp_scope=scope;
            while(temp_scope>=0)
            {
                sym=lookup(var_fun,temp_scope);
                if(sym!=NULL && check_iftable==-1)$1=lvalue_expr(sym);
                temp_scope--;
            }
            print_error=false;
            free(var_fun);
            var_fun=NULL;
        } 
        expr * arg=NULL;
        for(int i=0;i<argument_counter; i++)
        {
            arg=pop_exp(&call_name);
        }
        argument_counter=0;
        if(arg!=NULL)
        {
            $1=arg;
        }
        e_c=$1;
        $1 = emit_iftableitem($1,alpha_yylineno);
        if ($2->method){
            if($2->elist!=NULL)
            {
                expr *e=$1;
                expr *e1=get_last($2->elist);
                e1->next= $1;
            }
            arg=member_item($1, $2->name,alpha_yylineno);
            $1 = emit_iftableitem(arg,alpha_yylineno);
        
        }
        $$ = make_call($1, $2->elist,alpha_yylineno);
    }    
    | funcdef_par lparenthesis elist rparenthesis{
        fprintf(file," funcdef_par lparenthesis elist rparenthesis\n");
        expr* func = new_expr(programfunc_e);
        func->sym = $1;
        $$ = make_call(func, $3,alpha_yylineno);
    }
    ;

callsuffix:
    normcall {$$=$1;fprintf(file,"normcall\n");}
    | 
    methodcall{$$=$1;fprintf(file,"methodcall\n");}
    ;

normcall:
    lparenthesis elist rparenthesis { fprintf(file," lparenthesis elist rparenthesis\n");
    $$=new_Call($2,0,NULL); }
    ;

methodcall:
    ddot variable lparenthesis elist rparenthesis {$$= new_Call($4,1,$2);argument_counter++;ddotParam=true;fprintf(file,"  ddot variable lparenthesis elist rparenthesis\n");}
    ;
indexed:
    /*empty*/
    {
        $$=NULL;
    }
    |indexelem_comma
    ;   
indexelem_comma:
    indexelem { }
    | indexelem_comma comma indexelem{$$=insert_indexed($1,$3); fprintf(file,"indexelem_comma comma indexelem\n");}
    ;
indexelem: lcurly expr Colon expr rcurly{$$=new_map($2,$4);  fprintf(file," lcurly expr Colon expr rcurly\n");};
elist :/*empty*/
    {
        $$=NULL;
        argument_counter=0;
    }
    | elist_comma{$$=$1;}
    ;
elist_comma:
    expr{fprintf(file,"expr\n");$$=$1;$$->next=NULL;argument_counter++;true_jump_false(alpha_yylineno,$1);}
    | 
    elist_comma comma expr{argument_counter++;$$=insert_elist($1,$3);true_jump_false(alpha_yylineno,$3); fprintf(file," elist_comma comma expr\n");}
    ;
elist_indexed:
    elist_comma {$$=new_expression(0,$1);fprintf(file," elist_comma\n");}
    | 
    indexed{$$=new_mapping(1,$1); fprintf(file," indexed\n");}
    ;

objectdef:
    lbracket elist_indexed rbracket
     {
        fprintf(file," lbracket elist_indexed rbracket\n");
        expr* t = new_expr(newtable_e);
        t->sym = newtemp(alpha_yylineno,0);
        emit(tablecreate, NULL, NULL,t,0,alpha_yylineno);
        expr_map *ma=$2; 
        if($2->expr_or_map==0)
        {
            for (int i = 0; $2->expression; $2->expression = $2->expression->next)
            emit(tablesetelem, new_expr_constnum(i++), $2->expression,t,0,alpha_yylineno);
        }
        else
        {
            for (int i = 0; $2->indexed; $2->indexed = $2->indexed->next)
            emit(tablesetelem, $2->indexed->index, $2->indexed->value,t,0,alpha_yylineno);
        }
        $$ = t;
     }
    ;
block:
    lcurly{   fprintf(file,"block\n"); scope++;temp_off=currscopeoffset();} stmts rcurly{ SymTable_hide(symtable,scope--); $$=$3;} 
    |  lcurly{scope++;}rcurly{ SymTable_hide(symtable,scope--);$$ = NULL;}
    ;
blockFunction:
    lcurly{  fprintf(file,"blockFunction\n"); scope++;temp_off=currscopeoffset();push(&loop_counter,NULL,keyword_bc,NULL);keyword_bc=-1;} stmts rcurly{ SymTable_hide(symtable,scope--); $$=temp_off; keyword_bc=peek(loop_counter); } 
    |  lcurly{scope++; push(&loop_counter,NULL,keyword_bc,NULL);keyword_bc=-1;}  rcurly{keyword_bc=peek(loop_counter);}
    ;
funcname:
    /*empty*/
    {       
        fprintf(file,"funcname\n");
        char dollar[]="$";
        char *anonymous_str=malloc(100000);
        inttostring(anonymous_str,anonymous++);
        char *temp_func=strdup(strcat(dollar,anonymous_str));
        push(&stack_function,temp_func,scope,NULL);
        SymTable_insert(symtable,temp_func,scope,alpha_yylineno,USERFUNC);
        //new code
        sym=initialize_symbol(temp_func,alpha_yylineno,programmfunc_s,scope);
        insert(sym);
        $$=strdup(temp_func);
        free(anonymous_str);
    }
    |variable{
        fprintf(file,"variable\n");
        $$=strdup($1);
        fprintf(file,"%s : offset:%d\n",$$,currscopeoffset());
        push(&stack_function,$1,scope,NULL);
        if(check_libfunc($1)==true)
        {
            red();
            printf("LIBFUNC name cannot be used: %s LINE %d\n",$1,alpha_yylineno);
            reset();
        }
        else if(SymTable_contains(symtable,$1,USERFUNC,scope,false)==false && 
        SymTable_contains(symtable,$1,GLOBAL,scope,false)==false && 
        SymTable_contains(symtable,$1,FORMAL,scope,false)==false &&
        SymTable_contains(symtable,$1,LOCAL,scope,false)==false)
        {
            SymTable_insert(symtable,$1,scope,alpha_yylineno,USERFUNC);
            //new code
            sym=initialize_symbol($1,alpha_yylineno,programmfunc_s,scope);
          
            insert(sym);
        }
        else
        {
            red();
            printf("Error Name already exists: %s line : %d\n",$1,alpha_yylineno);
            reset();
        }
    };
funcprefix:Function funcname
{
    fprintf(file,"funcprefix:Function funcname\n");
     if(currscopespace()==functionlocal)//poustia
            {
                functionLocalOffset--;
            }
            else
            {
                programmVarOffset--;
            }   
    $$ = initialize_symbol($2,alpha_yylineno ,programmfunc_s,scope);
    $$->iaddress = nextquad();
    jump_function=1;
    emit(jump,NULL,NULL,NULL,0,alpha_yylineno);
    push(&func_jump,NULL,actual_nextquad()-1,NULL);
    emit(funcstart, lvalue_expr($$), NULL, NULL,0,alpha_yylineno);
    push(&offset_save,NULL,currscopeoffset(),NULL); 
    inside_function++; 
    resetformalargsoffset(); 
    resetfunctionlocalsoffset();
};
funargs:  
    lparenthesis{fprintf(file,"funargs\n");enterscopespace();inside_function--;scope++;} 
    idlist rparenthesis{ fprintf(file," idlist rparenthesis\n");scope--;inside_function++;{keyword_ret++;}enterscopespace();};
funcdef:
    funcprefix funargs blockFunction {
        fprintf(file,"funcprefix funargs block \n");
        pop(&stack_function); 
        inside_function--;
        {keyword_ret--;}
        exitscopespace();
        exitscopespace();
        $1->totalLocals = $3;
        int offset =peek(offset_save);
        restorecurrscopeoffset(offset);
        $$ = $1;
        emit(funcend, lvalue_expr($1), NULL, NULL,0,alpha_yylineno);
        patchlabel(peek(func_jump),nextquad());
        pop(&func_jump);
        }
    ;

const:
    integer {fprintf(file,"integer \n"); $$=new_expr_constnum($1);  }
    | String {fprintf(file,"String \n"); $$=new_expr_conststring($1);  }
    | Nil { fprintf(file,"Nil \n");$$=new_expr(nil_e); }
    | True { fprintf(file,"True \n");$$=new_expr_constbool(1);}
    | False{fprintf(file,"False \n"); $$=new_expr_constbool(0);  } 
    | real { fprintf(file,"real \n");$$=new_expr_constnum($1); }
    ;

idlist:/*empty*/| idlist_comma{fprintf(file,"idlist_comma \n");}
    ;

idlist_comma:
    variable {
        fprintf(file,"variable \n");
        resetfunctionlocalsoffset();
        if(check_libfunc($1)==false && (SymTable_contains(symtable,$1,FORMAL,scope,inside_function)==false)  
                && SymTable_get(symtable,$1,USERFUNC,0)==NULL){
            SymTable_insert(symtable,$1,scope,alpha_yylineno,FORMAL);
            sym=initialize_symbol($1,alpha_yylineno,var_s,scope);
            insert(sym);
        }
        else{
            red();
            printf("Error There is already variable: %s int this scope LINE : %d\n",$1,alpha_yylineno);
            reset();
        }
    }
    | idlist_comma comma variable{
        fprintf(file,"idlist_comma comma variable \n");
         fprintf(file,"offset%d\n",currscopeoffset());
        if(check_libfunc($3)==false 
        && (SymTable_contains(symtable,$3,FORMAL,scope,inside_function)==false)  
        && SymTable_get(symtable,$3,USERFUNC,0)==NULL)
        {            
            SymTable_insert(symtable,$3,scope,alpha_yylineno,FORMAL);
            //new code
            sym=initialize_symbol($3,alpha_yylineno,var_s,scope);
            insert(sym);
        }
        else
        {
            red();
            printf("Error There is already variable: %s int this scope LINE:%d\n",$3,alpha_yylineno);
            reset();
        }
    }
    ;
    elseprefix: Else {
         fprintf(file," elseprefix: Else \n");
        $$=new_Af_quad(nextquad(),actual_nextquad());
        emit(jump, NULL, NULL,NULL,0,alpha_yylineno);
        isIteration++;};
ifprefix: If lparenthesis expr rparenthesis
{
     fprintf(file,"ifprefix: If lparenthesis expr rparenthesis\n");
    isIteration++;
    true_jump_false(alpha_yylineno,$3);
    emit(if_eq,new_expr_constbool(1),$3,NULL,nextquad()+2,alpha_yylineno);
    $$=actual_nextquad();
    emit(jump,NULL,NULL,NULL,0,alpha_yylineno);
};
ifstmt: ifprefix stmt { fprintf(file,"ifprefix stmt\n");patchlabel($1, nextquad()); $$=$2;isIteration--;}
 |
 ifprefix stmt elseprefix  stmt {
        fprintf(file,"ifprefix stmt elseprefix  stmt\n");
        patchlabel($1, $3->nextquad+1);
        patchlabel($3->actualquad, nextquad());
        $$=NULL;
        $$=make_stmt($$);
        ($2)?$$=$2:(void)0;
        $2=make_stmt($2);
        if($4)
        {
            $$->breakList = mergelist($2->breakList, $4->breakList);
            $$->contList = mergelist($2->contList, $4->contList);
        }
        isIteration--;
        }
 ;
whilestart: While{$$=nextquad();}
whilecond : lparenthesis  expr rparenthesis
{
    fprintf(file,"whilecond : lparenthesis  expr rparenthesis\n");
    keyword_bc++;
    isIteration++;
    true_jump_false(alpha_yylineno,$2);                                     
    emit(if_eq,new_expr_constbool(1),$2,NULL,nextquad()+2,alpha_yylineno);
    $$=actual_nextquad();
    emit(jump,NULL,NULL,NULL,0,alpha_yylineno);
}
whilestmt : whilestart whilecond stmt 
{
    fprintf(file,"whilestmt : whilestart whilecond stmt \n");
    stmt_t *s=$3;
    keyword_bc--;
    isIteration--;
    emit(jump, NULL, NULL,NULL,$1,alpha_yylineno);
    patchlabel($2, nextquad());
    if($3!=NULL)
    {
        patchlist($3->breakList, nextquad());
        patchlist($3->contList, $1);
    }
}
Nb:expr
{
    fprintf(file,"expr \n");
     $$=nextquad();
     true_jump_false(alpha_yylineno,$1);
     emit(if_eq, new_expr_constbool(1),$1,NULL, 0,alpha_yylineno);
}
Na:{$$=nextquad(); emit(jump,NULL,NULL,NULL,0,alpha_yylineno);fprintf(file,"Na \n");}
Nn:{$$=actual_nextquad()-1;fprintf(file,"Nn \n");}
Nc:{$$=actual_nextquad(); emit(jump,NULL,NULL,NULL,0,alpha_yylineno);fprintf(file,"Nc \n");}
forprefix :  For lparenthesis{(void)0; fprintf(file,"forprefix :  For lparenthesis \n");}
M elist semicolon Nb semicolon{$$=make_for_pref($7,actual_nextquad()-1);/*giati to kano hdh sthn 1179*/tmp_for=$$; keyword_bc++;isIteration++; fprintf(file," M elist semicolon expr semicolon\n");}

forstmt : 
         forprefix Na{(void)0; fprintf(file,"For Na\n");}
         Nn{(void)0;fprintf(file," Nn\n");}
         elist rparenthesis Nc {fprintf(file,"elist rparenthesis Nc \n");(void)0;}
         Nn{patchlabel($1->enter, nextquad()); fprintf(file," Nn \n");}
        stmt N
         { 
            fprintf(file,"stmt N \n");
           keyword_bc--;
           isIteration--;
           patchlabel($4,nextquad());//false jump
           patchlabel($8, $1->test); //loop jump
           patchlabel($13, $2 + 1); //closure jump
           if ($12 != NULL)
           {
               patchlist($12->breakList, nextquad()); 
               patchlist($12->contList, $2 + 1); 
           }
         }
         ;

returnstmt : Return expr_semicolon {
        fprintf(file,"returnstmt : Return expr_semicolon\n");
        if(keyword_ret<=0)
        {
            red();
            printf("Cannot use return outside of a function\n");
            reset();
        }     
        if($2==NULL)emit(ret, NULL,NULL,NULL,0,alpha_yylineno);  
        else emit(ret, $2,NULL,NULL,0,alpha_yylineno); 
        emit(jump, NULL,NULL,NULL,nextquad()+1,alpha_yylineno);
    }

%%
int yyerror(char *yaccProviderMessage)
{
    red();
    fprintf(stderr, "%s: at line %d, before token: %s\n", yaccProviderMessage, alpha_yylineno, alpha_yytext);
    fprintf(stderr, "INPUT NOT VALID\n");
    reset();
    return 0; 
}

int main(int argc, char **argv)
{
    file=fopen("output_parser.txt","w+");
    if(argc > 1)
    {
        if(!(alpha_yyin = fopen(argv[1], "r")))
        {
            red();
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            reset();
            return 1;
        }
    }
    else
    {
        alpha_yyin = stdin;
    }
    symtable=SymTable_new();
    enterscopespace();
    yyparse();
    print_all(symtable);
    print_all_quads();
    return 0;
}
