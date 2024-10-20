#include "symtable.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#define HASH_MULTIPLIER 65599
#define SEARCH_ALL -1
int SIZE=509;
extern FILE *file;
char *libfunctions[12]={
	"print",
	"input",
	"objectmemberkeys",
	"objecttotalmembers",
	"objectcopy",
	"totalarguments",
	"argument",
	"typeof",
	"strtonum",
	"sqrt",
	"cos",
	"sin"
};
/* Return a hash code for pcKey. */
unsigned int SymTable_hash( char *pcKey)
{
	size_t ui;
	unsigned int uiHash = 0U;
	for (ui = 0U; pcKey[ui] != '\0'; ui++)uiHash = uiHash * HASH_MULTIPLIER + pcKey[ui];
	return uiHash;

}
/*Makes a new hash array with pointers to every position of the array */
SymTable_T SymTable_new(void)
{
	int i;
	SymTable_T new=(SymTable_T)malloc(sizeof(struct SymTable));
	if(new==NULL)
	{
		printf("Error Out of Memory\n");
		exit(EXIT_FAILURE);
	}
	new->head=(struct SymbolTableEntry**)malloc(sizeof(struct SymbolTableEntry*)*SIZE);
	if(new->head==NULL)
	{
		printf("Error Out of Memory\n");
		exit(EXIT_FAILURE);
	}
	for(i=0;i<SIZE;i++)new->head[i]=NULL;
	for(i=0;i<12;i++)SymTable_insert(new,libfunctions[i],0,0,LIBFUNC);
	new->length=0;
	new->max_scope=0;
	return new;

}
/*it frees everything that got with dynamically allocated memory */
void SymTable_free(SymTable_T oSymTable)
{
	int i;
	if(oSymTable==NULL)return;
	for(i=0;i<SIZE;i++)
	{
		while(oSymTable->head[i]!=NULL){
			if(oSymTable->head[i]->type==USERFUNC || oSymTable->head[i]->type==LIBFUNC)
			{
				if(SymTable_remove(oSymTable,oSymTable->head[i]->value.funcVal->name,oSymTable->head[i]->type)==0)
				{
					printf("Error in free");
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				if(SymTable_remove(oSymTable,oSymTable->head[i]->value.varVal->name,oSymTable->head[i]->type)==0)
				{
					printf("Error in free");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	free(oSymTable->head);
	free(oSymTable);
	return;
}
/*Returns the total bindings of hash table */
unsigned int SymTable_getLength(SymTable_T oSymTable)
{
	assert(oSymTable!=NULL);
	return oSymTable->length;
}
static SymbolTableEntry* initialize( char *name, unsigned int scope, unsigned int line,  enum SymbolType type)
{
	struct SymbolTableEntry *new=(struct SymbolTableEntry*)malloc(sizeof(struct SymbolTableEntry));
	if(type==USERFUNC || type==LIBFUNC)
	{
		new->value.funcVal=(struct function*)malloc(sizeof(struct function));
		if(new->value.funcVal==NULL)
		{
			printf("Error in malloc\n");
			exit(EXIT_FAILURE);
		}
		new->value.funcVal->name=strdup(name);
		new->value.funcVal->scope=scope;
		new->value.funcVal->line=line;
	}
	else
	{
		new->value.varVal=(struct Variable*)malloc(sizeof(struct Variable));
		if(new->value.varVal==NULL)
		{
			printf("Error in malloc\n");
			exit(EXIT_FAILURE);
		}
		new->value.varVal->name=strdup(name);
		new->value.varVal->scope=scope;
		new->value.varVal->line=line;
	}
	new->isActive=true;
	new->type=type;
	return new;
} 
/*Puts the element to hash table */
int SymTable_insert(SymTable_T oSymTable, char *name, unsigned int scope, unsigned int line,  enum SymbolType type)
{
	assert(oSymTable);
	assert(name);
	int hash=SymTable_hash(name)%SIZE;
	if(scope==0 && type!=USERFUNC && type != LIBFUNC)type=GLOBAL;	
	struct SymbolTableEntry *head=oSymTable->head[hash];
	struct SymbolTableEntry *new=initialize(name,scope,line,type);
	new->next=head;
	oSymTable->length++;
	oSymTable->head[hash]=new;
	if(scope>oSymTable->max_scope)oSymTable->max_scope=scope;
	return 1;
}
/*Removes the element from the hash table  */
int SymTable_remove(SymTable_T oSymTable,  char* name ,  enum SymbolType type)
{
	bool function=false;
	int hash;
	struct SymbolTableEntry* find;
	struct SymbolTableEntry* previous=NULL;
	assert(oSymTable!=NULL);
	hash=SymTable_hash(name)%SIZE;
	find=oSymTable->head[hash];
	if(type==USERFUNC || type==LIBFUNC)function=true;
	while(find!=NULL)
	{
		if(find->type==type)
		{
			if(function==true)
			{
				if(strcmp(find->value.funcVal->name,name)==0)break;
			}
			else
			{
				if(strcmp(find->value.varVal->name,name)==0)break;
			}
		}
		previous=find;
		find=find->next;
	}
	if(find==NULL)return 0;
	else if(previous==NULL)oSymTable->head[hash]=oSymTable->head[hash]->next;
	else previous->next=find->next;
	if(function==true)
	{
		free(find->value.funcVal->name);
		free(find->value.funcVal);
	}
	else
	{
		free(find->value.varVal->name);
		free(find->value.varVal);
	}
	free(find);
	oSymTable->length--;
	return 1;

}
bool check_libfunc(char * name)
{
	assert(name);
	for(int i=0;i<12;i++)
	{
		if(strcmp(libfunctions[i],name)==0)return true;
	}
	return false;
}
/*Returns 1 if the element with key == name exists in the hash table else returns 0 */
bool SymTable_contains(SymTable_T oSymTable,  char* name ,  enum SymbolType type, int scope, int insideFunction)
{
	assert(oSymTable);
	assert(name);
	int i;
	if(check_libfunc(name)==true && scope!=0 && type!=LIBFUNC)return true;
	if(insideFunction!=0)
	{
		if(SymTable_get(oSymTable,name,FORMAL,scope)!=NULL || SymTable_get(oSymTable,name,LOCAL,scope)!=NULL || SymTable_get(oSymTable,name,USERFUNC,scope)!=NULL 
			|| SymTable_get(oSymTable,name,GLOBAL,0)!=NULL /*|| SymTable_get(oSymTable,name,USERFUNC,0)!=NULL*/)return true;
		else return false;
	}
	else if(scope==SEARCH_ALL)
	{
		for(i=oSymTable->max_scope;i>-1;i--)
		{
			if(SymTable_get(oSymTable,name,LIBFUNC,i)!=NULL)return true;
			if(SymTable_get(oSymTable,name,USERFUNC,i)!=NULL)return true;
			if(SymTable_get(oSymTable,name,FORMAL,i)!=NULL)return true;
			if(SymTable_get(oSymTable,name,GLOBAL,i)!=NULL)return true;
			if(SymTable_get(oSymTable,name,LOCAL,i)!=NULL)return true;
		}
		return false;		
	}
	else
	{
		if(SymTable_get(oSymTable,name,type,scope)!=NULL)return true;
		return false;
	}
	

}
/*Returns the address of Variable/Function if the name exists to the hash table
else returns null */
void *SymTable_get(SymTable_T oSymTable,  char* name ,  enum SymbolType type, int scope)
{
	bool function=false;
	int hash;
	struct SymbolTableEntry* find;
	assert(oSymTable!=NULL);
	if(type==USERFUNC || type==LIBFUNC)function=true;
	hash=SymTable_hash(name)%SIZE;
	find=oSymTable->head[hash];
	while(find!=NULL )
	{
		if(find->isActive==true && find->type==type)
		{
			if(function==true)
			{
				if(find->value.funcVal->scope==scope && strcmp(find->value.funcVal->name,name)==0)break;
			}
			else
			{
				if(find->value.varVal->scope==scope && strcmp(find->value.varVal->name,name)==0)break;
			}
		}
		find=find->next;
	}
	if(find==NULL)return NULL;
	if(function==true)/*return (void *)find->value.funcVal*/return (void *)find;
	else return /*(void *)find->value.varVal*/(void *)find;
}
/*Hides the element in a scope */
void SymTable_hide(SymTable_T oSymTable,  unsigned int scope)
{
	int i;
	for(i=0;i<SIZE;i++)
	{
		SymbolTableEntry* find=oSymTable->head[i];
		while(find!=NULL)
		{
			if(find->type==USERFUNC || find->type==LIBFUNC)
			{
				if(find->value.funcVal->scope==scope)find->isActive=false;
			}
			else
			{
				if(find->value.varVal->scope==scope)find->isActive=false;
			}
			find=find->next; 
		}
	}
	return;
}
void show_hidden(SymTable_T oSymTable)
{
	int i;
	for(i=0;i<SIZE;i++)
	{
		SymbolTableEntry* find=oSymTable->head[i];
		while(find!=NULL)
		{
			if(find->type==USERFUNC || find->type==LIBFUNC)
			{
				if(find->isActive==false)fprintf(file,"Hidden Function: %s\n",find->value.funcVal->name);
			}
			else
			{
				if(find->isActive==false)fprintf(file,"Hidden Variable: %s\n",find->value.varVal->name);
			} 
			find=find->next;
		}
	}
	return;
}

void print_all(SymTable_T oSymTable)
{
	int i;
	for(i=0;i<=oSymTable->max_scope;i++)
	{
		fprintf(file,"-----------		Scope #%d 	-----------\n",i);
		for(int j=0;j<SIZE;j++)
		{
			struct SymbolTableEntry* find=oSymTable->head[j];
			while(find!=NULL)
			{
				if(find->type==LIBFUNC && find->value.funcVal->scope==i)fprintf(file,"\"%s\" [LIBFUNC] (line=%d) (scope=%d)\n",find->value.funcVal->name,find->value.funcVal->line,find->value.funcVal->scope);
				find=find->next;
			}
		}
		for(int j=0;j<SIZE;j++)
		{
			struct SymbolTableEntry* find=oSymTable->head[j];
			while(find!=NULL)
			{
				if(find->type==USERFUNC && find->value.funcVal->scope==i)fprintf(file,"\"%s\" [USERFUNC] (line=%d) (scope=%d)\n",find->value.funcVal->name,find->value.funcVal->line,find->value.funcVal->scope);
				find=find->next;
			}
		}
		for(int j=0;j<SIZE;j++)
		{
			struct SymbolTableEntry* find=oSymTable->head[j];
			while(find!=NULL)
			{
				if(find->type==GLOBAL && find->value.varVal->scope==i)fprintf(file,"\"%s\" [GLOBAL] (line=%d) (scope=%d)\n",find->value.varVal->name,find->value.varVal->line,find->value.varVal->scope);
				find=find->next;
			}
		}
		for(int j=0;j<SIZE;j++)
		{
			struct SymbolTableEntry* find=oSymTable->head[j];
			while(find!=NULL)
			{
				if(find->type==LOCAL && find->value.varVal->scope==i)fprintf(file,"\"%s\" [LOCAL] (line=%d) (scope=%d)\n",find->value.varVal->name,find->value.varVal->line,find->value.varVal->scope);
				find=find->next;
			}
		}
		for(int j=0;j<SIZE;j++)
		{
			struct SymbolTableEntry* find=oSymTable->head[j];
			while(find!=NULL)
			{
				if(find->type==FORMAL && find->value.varVal->scope==i)fprintf(file,"\"%s\" [FORMAL] (line=%d) (scope=%d)\n",find->value.varVal->name,find->value.varVal->line,find->value.varVal->scope);
				find=find->next;
			}
		}
		
	}
}