%{
#define SEARCH_ALL -1
#include "symtable.h"
#include "stack.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int yyerror(char *yaccProviderMessage);
#define yylex alpha_yylex
extern int alpha_yylex(void);
extern int alpha_yylineno;
extern char *alpha_yytext;
extern FILE *alpha_yyin;
char *var_fun=NULL;
int var_fun_type=0;
bool print_error=false;
bool declare_var=false;
int isIteration=0;
int anonymous=1;
int dot_var=0;
int keyword_bc=0;
int keyword_ret=0;
char dollar[100001]="$";
bool lvalue_callsuffix=true;
int scope=0;
int inside_function=0;
SymTable_T symtable;
struct StackNode* stack_function = NULL;
bool check_libfunc(char*); 
void inttostring(char *str, int num);
void printError(char *error)
{
    if(print_error==true)
    {
        printf("\n %s %s line: %d\n",error,var_fun,alpha_yylineno);
        free(var_fun);
        var_fun=NULL;
        print_error=false;
    }
        return; 
}
#define YYDEBUG 1 
%}

%union {
    char *stringValue;
    int intValue;
    float realValue;

}

%start programm

%token <stringValue> variable 
%token <intValue> integer 
%token <realValue> real 
%token <stringValue> String

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
%right Not incr decr UMINUS  /*Den yparxei proseteristikothta sto + kai to - yparxei dio fores sthn proteraiothta*/
%left dot ddot
%left lbracket rbracket
%left lparenthesis rparenthesis
%type <intValue> expr term 
%type <stringValue> lvalue
%%

programm:
    stmts { printf("programm\n"); }
    ;

stmt:
     expr_semicolon{ printf("stmt\n"); }
    | ifstmt { printf("ifstmt\n"); }
    | whilestmt{ printf("whilestmt\n"); }
    | forstmt { printf("forstmt\n"); }
    | returnstmt{ printf("returnstmt\n"); }
    | Break semicolon {printf("Break\n");if(keyword_bc<=0)printf("Cannot use break outside of a loop\n");}
    | Continue semicolon {printf("Continue\n");if(keyword_bc<=0)printf("Cannot use continue outside of a loop\n");}
    | funcdef{ printf("funcdef\n"); }
    | block{ printf("block\n"); }
    ;

stmts:
    /* empty */
    | stmts stmt { printf("stmts\n"); }
    ;
expr_semicolon:
    expr semicolon
    | semicolon;

expr:
    assignexpr {printf("assignexpr\n");}
    | expr Plus expr {$$=$1+$3; printf("expr Plus expr\n");}  
    | expr Minus expr { $$=$1-$3; printf("expr Minus expr\n");}
    | expr Mult expr { $$=$1*$3; printf("expr Mult expr\n");}
    | expr Div expr { $$=$1/$3;printf("expr Div expr\n");}
    | expr Mod expr { $$=$1%$3; printf("expr Mod expr\n");}
    | expr smaller expr {$$=($1<$3);printf("expr smaller expr\n");}
    | expr greater expr {$$=($1<$3);printf("expr greater expr\n");}
    | expr geq expr { $$=($1>=$3); printf("expr geq expr\n");}
    | expr seq expr { $$=($1<=$3); printf("expr seq expr\n");}
    | expr beq expr { $$=($1==$3); printf("expr beq expr\n");}
    | expr bne expr { $$=($1!=$3); printf("expr bne expr\n");}
    | expr And expr { $$=$1&&$3; printf("expr And expr\n");}
    | expr Or expr {  $$=$1||$3; printf("expr Or expr\n");}
    | term  {printf("term\n");}
    ;


assignexpr:
    lvalue{printf("LINE: %d\n",alpha_yylineno); if(check_libfunc(var_fun)==true)print_error=true; printError("Cannot use lvalue variable: ");
        if(declare_var==true)
            {
                SymTable_insert(symtable,var_fun,scope,alpha_yylineno,LOCAL);
                declare_var=false;
            }} 
    Assign expr {}
    ;

term:
    lparenthesis expr rparenthesis {  printf("lparenthesis expr rparenthesis \n");}
    | Minus expr %prec UMINUS  { printf("lMinus expr \n");}
    | Not expr { printf("ot expr \n");}
    | incr lvalue{if(check_libfunc(var_fun)==true)print_error=true; printError("Cannot pre-increase this variable: "); printf("incr lvalue\n");}
    | lvalue{if(check_libfunc(var_fun)==true)print_error=true;printError("Cannot post-increase this variable: ");} incr{ printf("lvalue incr\n");}
    | decr lvalue{if(check_libfunc(var_fun)==true)print_error=true;printError("Cannot pre-decrease this variable:"); printf("decr lvalue \n");}
    | lvalue{if(check_libfunc(var_fun)==true)print_error=true;printError("Cannot post-decrease this variable:");} decr{ printf("llvalue decr\n");}
    | primary{ printf("primary\n");}
    ;
funcdef_par:lparenthesis funcdef rparenthesis
;

primary:
    lvalue {printError("Cannot use primary variable: ");printf("lvalue\n"); }
    | call{ printf("call\n");}
    | objectdef{ printf("objectdef\n");}
    | funcdef_par{ printf("lparenthesis funcdef rparenthesis\n");}
    | const{ printf("const\n");}
    ;

lvalue:
    variable {printf("variable\n");
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
        }
        else if(check_libfunc($1)==false && SymTable_contains(symtable,$1,USERFUNC,SEARCH_ALL,inside_function)==false)
        {    
            printf("asxeto error LINE: %d \n",alpha_yylineno);
        }
        else if (check_libfunc($1)==false && SymTable_contains(symtable,$1,LOCAL,scope,inside_function)==false && 
            SymTable_contains(symtable,$1,GLOBAL,scope,inside_function)==false &&
            SymTable_contains(symtable,$1,USERFUNC,scope,inside_function)==true)
        {
            print_error=true;
        }
        else
        {
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
                    break;
                }
                temp++;
            }
            if(SymTable_contains(symtable,$1,USERFUNC,0,false)==false && 
                SymTable_contains(symtable,$1,GLOBAL,0,false)==false && 
                temp == isIteration+1 && check_libfunc($1)==false)
            {//Se periptosi pou eimaste mesa se sinartisi dilonontai {id} kai otan den anagnwrizontai diladi return x*y otan to y den exei vrethei pio prin
                SymTable_insert(symtable,$1,scope,alpha_yylineno,LOCAL);
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
                printf("\nFunction/variable1 does not exist: %s line: %d\n",$1,alpha_yylineno);
            } 
            if(temp_scope==function_scope && SymTable_contains(symtable,$1,USERFUNC,0,false)==false && SymTable_contains(symtable,$1,GLOBAL,0,false)==false)
            {
                SymTable_insert(symtable,$1,scope,alpha_yylineno,LOCAL);                 
            }
        }
    }
    }
    | Local variable  {
        if(var_fun!=NULL)free(var_fun);
        var_fun=strdup($2);
        printf("local variable\n");
        if(check_libfunc($2)==true && scope!=0)printf("Using LIBFUNC name: %s LINE %d\n",$2,alpha_yylineno);
        else if(check_libfunc($2)==true && scope==0){}
        else if(scope ==0)
        {
            if(SymTable_contains(symtable,$2,GLOBAL,scope,false)==false && SymTable_contains(symtable,$2,USERFUNC,scope,false)==false)//an local var elexneis an yparxeis gia na se diloseis
                        SymTable_insert(symtable,$2,scope,alpha_yylineno,GLOBAL);
        }
        else
        {
            if(SymTable_contains(symtable,$2,FORMAL,scope,false)==false && 
                SymTable_contains(symtable,$2,USERFUNC,scope,false)==false && 
                SymTable_contains(symtable,$2,LOCAL,scope,false)==false)
                //an eisai se scope!=0 koitas kai formal kai local sto scope pou eisai
                        SymTable_insert(symtable,$2,scope,alpha_yylineno,LOCAL);
        }
    }
    | dcolon variable {
        if(var_fun!=NULL)free(var_fun);
        var_fun=strdup($2);
        printf("..variable\n");if(SymTable_contains(symtable,$2,GLOBAL,0,false)==false && 
            (SymTable_contains(symtable,$2,USERFUNC,0,inside_function)==false && 
                SymTable_get(symtable,$2,USERFUNC,0)==NULL))printf("no global variable %s found line: %d",$2,alpha_yylineno);
    }
    | member{
        printf("member\n");}
    ;

member:
    lvalue{if(check_libfunc(var_fun)==true)print_error=true;printError("lvalue.variable cannot be used: ");} Dot variable{dot_var=1;printf("lvalue . variable\n");}
    | lvalue{if(check_libfunc(var_fun)==true)print_error=true;printError("lvalue[expr] cannot be used here: ");} lbracket expr rbracket{printf("lvalue [expr] \n");}
    | call lbracket expr rbracket{printf("call [ expr ]\n");}
    | call Dot variable{
        if(var_fun!=NULL)free(var_fun);    
        var_fun=strdup($3);
        printf("call . variable\n");
    }
    ;

call:
    call lparenthesis elist rparenthesis { printf("call(elist)\n"); }
    | lvalue callsuffix{
        if(var_fun!=NULL)
        {
            print_error=false;
            free(var_fun);
            var_fun=NULL;
        } 
        printf("lvalue callsuffix\n");
    }    
    | funcdef_par lparenthesis elist rparenthesis{printf("(funcdef)(elist)\n");}
    ;

callsuffix:
    normcall {printf("normcall\n");}
    | methodcall{printf("methodcall\n");}
    ;

normcall:
    lparenthesis elist rparenthesis { printf("normcall\n"); }
    ;

methodcall:
    ddot variable lparenthesis elist rparenthesis { printf(" .. variable ( elist )\n"); }
    ;
indexed:
    /*empty*/
    |indexelem_comma
    ;   
indexelem_comma:
    indexelem { printf("indexed\n"); }
    | indexelem_comma comma indexelem
    ;
indexelem: lcurly expr Colon expr rcurly{ printf("{expr:expr}\n"); };
elist :
    /*empty*/
    | elist_comma
    ;
elist_comma:
    expr { printf("elist\n"); }
    | elist_comma comma expr
    ;
elist_indexed:
    elist_comma
    | indexed
    ;

objectdef:
    lbracket elist_indexed rbracket { printf("[elist]\n"); }
    ;

block:
    lcurly{scope++;} stmts rcurly{SymTable_hide(symtable,scope--);} { printf("{stmts}\n"); }
    ;
VarOptional:
    /*empty*/
    {       
        char dollar[]="$";char *anonymous_str=malloc(100000);inttostring(anonymous_str,anonymous++);
        char *temp_func=strdup(strcat(dollar,anonymous_str));
        push(&stack_function,temp_func,scope);
        SymTable_insert(symtable,temp_func,scope,alpha_yylineno,USERFUNC);
        free(anonymous_str);
    }
    |variable{
        push(&stack_function,$1,scope);
        if(check_libfunc($1)==true)printf("LIBFUNC name cannot be used: %s LINE %d\n",$1,alpha_yylineno);
        else if(SymTable_contains(symtable,$1,USERFUNC,scope,false)==false && 
        SymTable_contains(symtable,$1,GLOBAL,scope,false)==false && 
        SymTable_contains(symtable,$1,FORMAL,scope,false)==false &&
        SymTable_contains(symtable,$1,LOCAL,scope,false)==false)
        {
            SymTable_insert(symtable,$1,scope,alpha_yylineno,USERFUNC);

        }
        else{
            printf("Name already exists: %s line : %d\n",$1,alpha_yylineno);
        }
    };
funcdef:
    Function{inside_function++;} VarOptional lparenthesis{inside_function--;scope++;} idlist rparenthesis{scope--;inside_function++;{keyword_ret++;}} block {pop(&stack_function); inside_function--;{keyword_ret--;} }
    ;

const:
    integer { printf("const:integer\n"); }
    | String { printf("const:string\n"); }
    | Nil { printf("const:nil\n"); }
    | True { printf("const:true\n"); }
    | False{ printf("const:false\n"); } 
    | real { printf("const:real\n"); }
    ;

idlist:
    /*empty*/
    | idlist_comma
    ;

idlist_comma:
    variable {if(check_libfunc($1)==false && (SymTable_contains(symtable,$1,FORMAL,scope,inside_function)==false)  
        && SymTable_get(symtable,$1,USERFUNC,0)==NULL){
            SymTable_insert(symtable,$1,scope,alpha_yylineno,FORMAL);
        }
        else{
            printf("There is already variable: %s int this scope LINE : %d\n",$1,alpha_yylineno);
        }
    }
    | idlist_comma comma variable{
        if(check_libfunc($3)==false 
        && (SymTable_contains(symtable,$3,FORMAL,scope,inside_function)==false)  
        && SymTable_get(symtable,$3,USERFUNC,0)==NULL)
        {            
            SymTable_insert(symtable,$3,scope,alpha_yylineno,FORMAL);
        }
        else{
            printf("There is already variable: %s int this scope LINE:%d\n",$3,alpha_yylineno);
        }
    }
    ;
 else_stmt:
    /*empty*/
    | Else {isIteration++;scope++;}  expr_semicolon {scope--;isIteration--;}
    | Else {isIteration++;scope++;} ifstmt {scope--;isIteration--;}
    | Else {isIteration++;scope++;} whilestmt {scope--;isIteration--;}
    | Else {isIteration++;scope++;} forstmt {scope--;isIteration--;}
    | Else {isIteration++;scope++;}  returnstmt {scope--;isIteration--;}
    | Else {isIteration++;scope++;} Break semicolon {scope--;isIteration--;}
    | Else {isIteration++;scope++;} Continue semicolon {scope--;isIteration--;} 
    | Else {isIteration++;scope++;} funcdef {scope--;isIteration--;}
    | Else {isIteration++;} block {isIteration--;};
    ;

new_stmt:
    {scope++;} expr_semicolon{scope--; printf("stmt\n"); }
    |{scope++;} ifstmt {scope--; printf("ifstmt\n"); }
    |{scope++;} whilestmt{scope--; printf("whilestmt\n"); }
    |{scope++;} forstmt {scope--; printf("forstmt\n"); }
    |{scope++;} returnstmt{scope--; printf("returnstmt\n"); }
    | Break semicolon {printf("Break\n");if(keyword_bc<=0)printf("Cannot use break outside of a loop\n");}
    | Continue semicolon {printf("Continue\n");if(keyword_bc<=0)printf("Cannot use continue outside of a loop\n");}
    |{scope++;} funcdef{ scope--;printf("funcdef\n"); }
    | block
    ;
ifstmt: If lparenthesis expr rparenthesis {isIteration++;}new_stmt{ isIteration--;} else_stmt

whilestmt : While lparenthesis  expr rparenthesis{keyword_bc++;isIteration++;scope++;}  expr_semicolon {scope--;keyword_bc--;isIteration--;}
    | While lparenthesis  expr rparenthesis{keyword_bc++;isIteration++;scope++;} ifstmt {scope--;keyword_bc--;isIteration--;}
    | While lparenthesis  expr rparenthesis{keyword_bc++;isIteration++;scope++;} whilestmt {scope--;keyword_bc--;isIteration--;}
    | While lparenthesis  expr rparenthesis{keyword_bc++;isIteration++;scope++;} forstmt {scope--;keyword_bc--;isIteration--;}
    | While lparenthesis  expr rparenthesis{keyword_bc++;isIteration++;scope++;}  returnstmt {scope--;keyword_bc--;isIteration--;}
    | While lparenthesis  expr rparenthesis{keyword_bc++;isIteration++;scope++;} Break semicolon {scope--;if(keyword_bc<=0)printf("Cannot use break outside of a loop\n");keyword_bc--;isIteration--;}
    | While lparenthesis  expr rparenthesis{keyword_bc++;isIteration++;scope++;} Continue semicolon {scope--;if(keyword_bc<=0)printf("Cannot use continue outside of a loop\n");keyword_bc--;isIteration--;} 
    | While lparenthesis  expr rparenthesis{keyword_bc++;isIteration++;scope++;} funcdef {scope--;keyword_bc--;isIteration--;}
    | While lparenthesis  expr rparenthesis{keyword_bc++;isIteration++;} block {keyword_bc--;isIteration--;};

forstmt : For lparenthesis  elist semicolon expr semicolon elist  rparenthesis{keyword_bc++;isIteration++;scope++;}  expr_semicolon {scope--;keyword_bc--;isIteration--;}
    | For lparenthesis  elist semicolon expr semicolon elist rparenthesis{keyword_bc++;isIteration++;scope++;} ifstmt {scope--;keyword_bc--;isIteration--;}
    | For lparenthesis elist semicolon expr semicolon elist  rparenthesis{keyword_bc++;isIteration++;scope++;} whilestmt {scope--;keyword_bc--;isIteration--;}
    | For lparenthesis  elist semicolon expr semicolon elist  rparenthesis{keyword_bc++;isIteration++;scope++;} forstmt {scope--;keyword_bc--;isIteration--;}
    | For lparenthesis  elist semicolon expr semicolon elist  rparenthesis{keyword_bc++;isIteration++;scope++;}  returnstmt {scope--;keyword_bc--;isIteration--;}
    | For lparenthesis  elist semicolon expr semicolon elist  rparenthesis{keyword_bc++;isIteration++;scope++;} Break semicolon {scope--;if(keyword_bc<=0)printf("Cannot use break outside of a loop\n");keyword_bc--;isIteration--;}
    | For lparenthesis  elist semicolon expr semicolon elist  rparenthesis{keyword_bc++;isIteration++;scope++;} Continue semicolon {scope--;if(keyword_bc<=0)printf("Cannot use continue outside of a loop\n");keyword_bc--;isIteration--;} 
    | For lparenthesis  elist semicolon expr semicolon elist  rparenthesis{keyword_bc++;isIteration++;scope++;} funcdef {scope--;keyword_bc--;isIteration--;}
    | For lparenthesis  elist semicolon expr semicolon elist rparenthesis{keyword_bc++;isIteration++;} block {keyword_bc--;isIteration--;};

returnstmt : Return [expr_semicolon] {if(keyword_ret<=0)printf("Cannot use return outside of a function\n");}

%%
int yyerror(char *yaccProviderMessage)
{
    fprintf(stderr, "%s: at line %d, before token: %s\n", yaccProviderMessage, alpha_yylineno, alpha_yytext);
    fprintf(stderr, "INPUT NOT VALID\n");
    return 0; 
}

int main(int argc, char **argv)
{
    if(argc > 1)
    {
        if(!(alpha_yyin = fopen(argv[1], "r")))
        {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else
    {
        alpha_yyin = stdin;
    }
    symtable=SymTable_new();
    yyparse();
    print_all(symtable);

    return 0;
}
