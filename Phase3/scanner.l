%{
#include "parser.tab.h"
#include <stdio.h>
#include "src/list/list.h"
#include <string.h>
#define YY_DECL int alpha_yylex(void * lvalp)
char type[100];
int token=0;
lex_list list=NULL;
int count_comment=0;
int count_string=0;
char buffer[100000];
int start_comment=0;
int end_comment=0;
void inttostring(char *str, int num) {
    int i,rem,length=0,n;
    n=num;
    while(n!=0) {
        length++;
        n/=10;
    }
    for(i=0;i<length;i++) {
        rem=num%10;
        num=num/10;
        str[length-(i+1)]=rem+'0';
    }
    str[length]='\0';
}
%}

%option prefix = "alpha_yy"
%option noyywrap
%option yylineno 

newline [\n]
for [f][o][r]
if [i][f]
else [e][l][s][e]
while [w][h][i][l][e]
function [f][u][n][c][t][i][o][n]
return [r][e][t][u][r][n]
break [b][r][e][a][k]
continue [c][o][n][t][i][n][u][e]
and [a][n][d]
not [n][o][t]
or [o][r]
local [l][o][c][a][l]
true [t][r][u][e]
false [f][a][l][s][e]
nil [n][i][l]

variable [a-zA-Z][a-zA-Z_0-9]*
integer (0[xX][0-9A-Fa-f]+)|([0-9]+)
real [0-9]*\.[0-9]+([eE][-+]?[0-9]+)?

space [\r \t\v]+
comment "//".*

geq [>][=]
seq [<][=]
beq [=][=]
incr [+][+]
decr [-][-]
assign [=]
plus [+]
minus[-]
mult [*]
div [/]
mod [%]
bne [!][=]
greater [>]
smaller [<]

lcurly [{]
rcurly [}]
lbracket [[]
rbracket []]
lparenthesis [(]
rparenthesis [)]
semicolon [;]
coma [,]
dcolon [:][:]
colon [:]
ddot [.][.]
dot[.]

%x COMMENT INCLUDE
%x STRING 

%% 
{newline} { }
{for} { list=insert_node(list, yylineno, ++token, "for", "for"); return For;}
{if} { list=insert_node(list, yylineno, ++token, "if", "if"); return If;}
{else} { list=insert_node(list, yylineno, ++token, "else", "else"); return Else;}
{while} { list=insert_node(list, yylineno, ++token, "while", "while"); return While;}
{function} { list=insert_node(list, yylineno, ++token, "function", "function"); return Function;}
{return} { list=insert_node(list, yylineno, ++token, "return", "return"); return Return;}
{break} { list=insert_node(list, yylineno, ++token, "break", "break"); return Break;}
{continue} { list=insert_node(list, yylineno, ++token, "continue", "continue"); return Continue;}
{and} { list=insert_node(list, yylineno, ++token, "and", "and"); return And;}
{not} { list=insert_node(list, yylineno, ++token, "not", "not"); return Not;}
{or} { list=insert_node(list, yylineno, ++token, "or", "or"); return Or;}
{local} { list=insert_node(list, yylineno, ++token, "local", "local"); return Local;}
{true} { list=insert_node(list, yylineno, ++token, "true", "true"); return True;}
{false} { list=insert_node(list, yylineno, ++token, "false", "false"); return False;}
{nil} { list=insert_node(list, yylineno, ++token, "nil", "nil"); return Nil;}

{variable} { list=insert_node(list,yylineno,++token,yytext,"variable");yylval.stringValue = strdup(yytext); return variable; }
{integer} { list=insert_node(list,yylineno,++token,yytext,"integer");yylval.intValue = atoi(yytext); return integer;}
{real} { list=insert_node(list,yylineno,++token,yytext,"real number"); yylval.realValue = atof(yytext); return real; }

{space} { }
{comment} { list=insert_node(list,yylineno,++token,yytext,"line comment"); }

[/][*] { BEGIN(COMMENT); count_comment++; end_comment=yylineno; start_comment=end_comment; }
<COMMENT>[*][/] { 
    count_comment--;
    if(count_comment==0) {
        BEGIN(INITIAL);
        char temp[100];
        char string[300]="";
        inttostring(temp,start_comment);
        strcat(string,temp);
        strcat(string,"-");
        inttostring(temp,yylineno);
        strcat(string,temp);
        list=insert_node(list,yylineno,++token,string,"comment");
    } else {
        list=insert_node(list,yylineno,++token,"\"\"","inner nested comment");
    }
}
<COMMENT>[\n] { end_comment++; }
<COMMENT>[/][*] { count_comment++; }
<COMMENT>. { }
<COMMENT><<EOF>> {
    if(count_comment>0) {
        print_lex(list);
        destroy(list);
        printf("Exit failure comment error\n");
        exit(EXIT_FAILURE);
    }
}
["] { 
    buffer[0]='\0'; 
    BEGIN(STRING); 
    count_string++; 
    strcat(buffer,yytext); 
}
<STRING>["] { 
BEGIN(INITIAL); 
count_string--; 
strcat(buffer,yytext);
 list=insert_node(list,yylineno,++token,buffer,"string");
 yylval.stringValue = strdup(buffer);
 return String;
 }
<STRING>\\[n] { strcat(buffer,"\n"); }
<STRING>\\[t] { strcat(buffer,"\t"); }
<STRING>\\[a] { strcat(buffer,"\a"); }
<STRING>\\\\ { strcat(buffer,"\\"); }
<STRING>\\["] { strcat(buffer,"\""); }
<STRING>\\['] { strcat(buffer,"'"); }
<STRING>\\[b] { strcat(buffer,"\b"); }
<STRING>\\[e] { strcat(buffer,"\e"); }
<STRING>\\[f] { strcat(buffer,"\f"); }
<STRING>\\[v] { strcat(buffer,"\v"); }
<STRING>\\[r] { strcat(buffer,"\r"); }
<STRING>\\[?] { strcat(buffer,"\?"); }
<STRING>\\ { strcat(buffer,yytext); printf("Error wrong string:%s\n",buffer); exit(EXIT_FAILURE); }
<STRING>. { strcat(buffer,yytext); }
<STRING><<EOF>> {
    if(count_string==1) {
        print_lex(list);
        destroy(list);
        printf("Exit failure string error\n");
        exit(EXIT_FAILURE);
    }
}

{geq} { list=insert_node(list,yylineno,++token,">=","greater equal"); return geq; }
{seq} { list=insert_node(list,yylineno,++token,"<=","less equal"); return seq; }
{beq} { list=insert_node(list,yylineno,++token,"==","equals"); return beq; }
{incr} { list=insert_node(list,yylineno,++token,"++","increment"); return incr; }
{decr} { list=insert_node(list,yylineno,++token,"--","decrement"); return decr; }
{assign} { list=insert_node(list,yylineno,++token,"=","assign"); return Assign; }
{plus} { list=insert_node(list,yylineno,++token,"+","plus"); return Plus; }
{minus} { list=insert_node(list,yylineno,++token,"-","minus"); return Minus; }
{mult} { list=insert_node(list,yylineno,++token,"*","multiply"); return Mult; }
{div} { list=insert_node(list,yylineno,++token,"/","div"); return Div; }
{mod} { list=insert_node(list,yylineno,++token,"%","modulo"); return Mod; }
{bne} { list=insert_node(list,yylineno,++token,"!=","not equal"); return bne; }
{greater} { list=insert_node(list,yylineno,++token,">","greater than"); return greater; }
{smaller} { list=insert_node(list,yylineno,++token,"<","less than"); return smaller; }
{lcurly} { list=insert_node(list,yylineno,++token,yytext,"lcurly"); return lcurly; }
{rcurly} { list=insert_node(list,yylineno,++token,yytext,"rcurly"); return rcurly; }
{lbracket} { list=insert_node(list,yylineno,++token,yytext,"lbracket"); return lbracket; }
{rbracket} { list=insert_node(list,yylineno,++token,yytext,"rbracket"); return rbracket; }
{lparenthesis} { list=insert_node(list,yylineno,++token,yytext,"lparenthesis"); return lparenthesis; }
{rparenthesis} { list=insert_node(list,yylineno,++token,yytext,"rparenthesis"); return rparenthesis; }
{semicolon} { list=insert_node(list,yylineno,++token,";","semicolon"); return semicolon; }
{coma} { list=insert_node(list,yylineno,++token,",","coma"); return comma; }
{dcolon} { list=insert_node(list,yylineno,++token,"::","dcolon"); return dcolon; }
{colon} { list=insert_node(list,yylineno,++token,":","colon"); return Colon; }
{ddot} { list=insert_node(list,yylineno,++token,"..","DDOT"); return ddot; }
{dot} { list=insert_node(list,yylineno,++token,".","DOT"); return Dot; }
.|\n { /* handle undefined characters */ }


