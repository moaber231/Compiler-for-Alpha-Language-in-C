%{
#include <stdio.h>
#include "list.h"
#include <string.h>
#define YY_DECL int alpha_yylex(void * lvalp)
char type[100];
int token=0;
lex_list list=NULL;
int count_comment=0;
int count_string=0;
char buffer[100000];
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

curly [{]|[}]
bracket [[]|[]]
parenthesis [(]|[)]
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
{for} {list=insert_node(list,yylineno,++token,"for","for"); }
{if} {list=insert_node(list,yylineno,++token,"if","if"); }
{else} {list=insert_node(list,yylineno,++token,"else","else"); }
{while} {list=insert_node(list,yylineno,++token,"while","while");}
{function} {list=insert_node(list,yylineno,++token,"function","function");}
{return} {list=insert_node(list,yylineno,++token,"return","return");}
{break} {list=insert_node(list,yylineno,++token,"break","break");}
{continue} {list=insert_node(list,yylineno,++token,"continue","continue");}
{and} {list=insert_node(list,yylineno,++token,"and","and");}
{not} {list=insert_node(list,yylineno,++token,"not","not");}
{or} {list=insert_node(list,yylineno,++token,"or","or");}
{local} {list=insert_node(list,yylineno,++token,"local","local");}
{true} {list=insert_node(list,yylineno,++token,"true","true");}
{false} {list=insert_node(list,yylineno,++token,"false","false");}
{nil} {list=insert_node(list,yylineno,++token,"nil","nil");}

{variable} { list=insert_node(list,yylineno,++token,yytext,"variable"); }
{integer} { list=insert_node(list,yylineno,++token,yytext,"integer"); }
{real}    { list=insert_node(list,yylineno,++token,yytext,"real number"); }

{space}   { list=insert_node(list,yylineno,++token," ","space"); }
{comment} { list=insert_node(list,yylineno,++token,yytext,"comment"); }


[/][*]                    { BEGIN(COMMENT); count_comment++; }
<COMMENT>[*][/]           {  count_comment--;
							if(count_comment==0){BEGIN(INITIAL);list=insert_node(list,yylineno,++token,yytext,"nested comment");}}
<COMMENT>[/][*]			{count_comment++;}
<COMMENT>"*"+           {  }
<COMMENT>.              {  }
<COMMENT><<EOF>>            {   
							if(count_comment>0)
							{
								print_lex(list);
    							destroy(list);
								printf("Exit failure comment error\n");  
								exit(EXIT_FAILURE);
							}}
["]					{buffer[0]='\0';BEGIN(STRING);count_string++;strcat(buffer,yytext);}
<STRING>["]			{BEGIN(INITIAL);count_string--;strcat(buffer,yytext);list=insert_node(list,yylineno,++token,buffer,"string");}
<STRING>\\[n]		{ strcat(buffer,"\n");}
<STRING>\\[t]		{ strcat(buffer,"\t");}
<STRING>\\[a]		{ strcat(buffer,"\a");}
<STRING>\\\\		{ strcat(buffer,"\\");strcat(buffer,"\\");}
<STRING>\\["]		{ strcat(buffer,"\\");char *quote="\"";strcat(buffer,quote);}
<STRING>\\[']		{ strcat(buffer,"\\");char *quot="'";strcat(buffer,quot);}
<STRING>\\[b] 		{ strcat(buffer,"\\");strcat(buffer,"b");}
<STRING>\\[e] 		{ strcat(buffer,"\\");strcat(buffer,"e");}
<STRING>\\[f] 		{ strcat(buffer,"\\");strcat(buffer,"f");}
<STRING>\\[v] 		{ strcat(buffer,"\\");strcat(buffer,"v");}
<STRING>\\[r] 		{ strcat(buffer,"\\");strcat(buffer,"r");}	
<STRING>\\[?] 		{ strcat(buffer,"\\");strcat(buffer,"?");}
<STRING>\\		    { strcat(buffer,yytext); printf("Error wrong string:%s\n",buffer);exit(EXIT_FAILURE); }
<STRING>.				{strcat(buffer,yytext); }
<STRING><<EOF>>			{if(count_string==1)
							{
								print_lex(list);
    							destroy(list);
								printf("Exit failure string error\n");  
								exit(EXIT_FAILURE);
							}}


{geq} { list=insert_node(list,yylineno,++token,">=","greater equal"); }
{seq} { list=insert_node(list,yylineno,++token,"<=","less equal"); }
{beq} { list=insert_node(list,yylineno,++token,"==","equals"); }
{incr} { list=insert_node(list,yylineno,++token,"++","increment"); }
{decr} { list=insert_node(list,yylineno,++token,"--","decrement"); }
{assign} { list=insert_node(list,yylineno,++token,"=","assign"); }
{plus} { list=insert_node(list,yylineno,++token,"+","plus"); }
{minus} { list=insert_node(list,yylineno,++token,"-","minus"); }
{mult} { list=insert_node(list,yylineno,++token,"*","multiply"); }
{div} { list=insert_node(list,yylineno,++token,"/","div"); }
{mod} { list=insert_node(list,yylineno,++token,"%","modulo"); }
{bne} { list=insert_node(list,yylineno,++token,"!=","not equal"); }
{greater} { list=insert_node(list,yylineno,++token,">","greater than"); }
{smaller} { list=insert_node(list,yylineno,++token,"<","less than"); }

{curly} { list=insert_node(list,yylineno,++token,yytext,"curly"); }
{bracket} { list=insert_node(list,yylineno,++token,yytext,"bracket"); }
{parenthesis} { list=insert_node(list,yylineno,++token,yytext,"parenthesis"); }
{semicolon} { list=insert_node(list,yylineno,++token,";","semicolon"); }
{coma} { list=insert_node(list,yylineno,++token,",","coma"); }
{dcolon} { list=insert_node(list,yylineno,++token,"::","dcolon"); }
{colon} { list=insert_node(list,yylineno,++token,":","colon"); }
{ddot} { list=insert_node(list,yylineno,++token,"..","DDOT"); }
{dot} { list=insert_node(list,yylineno,++token,".","DOT"); }
.|\n {printf("undefined %s\n",yytext);} 
%%


int main(int argc, char const *argv[])
{
	
    if (argc > 1)
    {
        if (!(alpha_yyin = fopen(argv[1], "r")))
        {
            printf("Error: Cannot read file %s\n", argv[1]);
            return 1;
        }
    }
    else
    {
        alpha_yyin = stdin;
    }
    alpha_yylex(NULL);
    print_lex(list);
    destroy(list);
    return 0;
}
