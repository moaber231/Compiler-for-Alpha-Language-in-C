#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 
#define WRONG NULL
struct StackNode { 
    char* function;
    int scope; 
    struct expr* e;
    struct StackNode* next; 
}; 

static int isEmpty(struct StackNode* root) 
{ 
    return !root; 
} 
struct StackNode* newNode(char* data, int scope,struct expr* exp) 
{ 
    struct StackNode* stackNode =  (struct StackNode*) malloc(sizeof(struct StackNode ) ); 
    if(data!=NULL)
    {
        stackNode->function =strdup(data);
    }
    else
    {
        stackNode->function =NULL;
    }
    stackNode->scope=scope; 
    stackNode->e=exp;
    stackNode->next = NULL; 
    return stackNode; 
} 


void push(struct StackNode** root, char* data, int scope,struct expr* exp) 
{ 
    struct StackNode* stackNode = newNode(data,scope,exp); 
    stackNode->next = *root; 
    *root = stackNode; 
} 

char* pop(struct StackNode** root) 
{ 
    if (isEmpty(*root)) 
        return WRONG; 
    struct StackNode* temp = *root; 
    *root = (*root)->next; 
    char* popped = temp->function;
    free(temp->function); 
    free(temp); 

    return popped; 
} 
struct expr* pop_exp(struct StackNode** root) 
{ 
    if (isEmpty(*root)) 
        return WRONG; 
    struct StackNode* temp = *root; 
    *root = (*root)->next; 
    struct expr* popped = temp->e;
    free(temp);
    return popped; 
} 
int peek(struct StackNode* root) 
{ 
    if (isEmpty(root)) 
        return -1; 
    return root->scope; 
} 
