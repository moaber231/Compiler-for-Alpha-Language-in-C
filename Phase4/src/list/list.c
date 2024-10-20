#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

lex_list insert_node(lex_list list,unsigned int numline,unsigned int numToken,char *content, char *type)
{
	//assert(list);
	assert(content);
	assert(type);
	lex_list new=(lex_list)malloc(sizeof(struct alpha_token_t));
	if(new==NULL)
	{
		printf("Error Out of Memory\n");
		exit(EXIT_FAILURE);
	}
	//initialize struct lex
	new->numline=numline;
	new->numToken=numToken;
	
	new->content=(char *)malloc(sizeof(char)*(strlen(content)+1));
	strcpy(new->content,content);
	
	new->type=(char*)malloc(sizeof(char)*(strlen(type)+1));
	strcpy(new->type,type);

	new->alpha_yylex=NULL;
	if(list==NULL)
	{
		return new;
	}
	//put the new node in the end of list
	lex_list temp=list;
	while(temp->alpha_yylex!=NULL)temp=temp->alpha_yylex;
	temp->alpha_yylex=new;
	return list;
}

void print_lex(lex_list list)
{
	lex_list temp=list;
	while(temp!=NULL)
	{
		printf("%d: ",temp->numline);
		printf("#%d		",temp->numToken);
		int i=0;
		printf("%s 		",temp->content);
		printf("%s",temp->type);
		printf("\n");
		temp=temp->alpha_yylex;
	}
	return;
}

void destroy(lex_list list)
{
	while(list!=NULL)
	{
		lex_list temp=list;
		list=list->alpha_yylex;
		free(temp->content);
		free(temp->type);
		free(temp);
	}
	return;
}
