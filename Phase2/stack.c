#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 
#define WRONG NULL
struct StackNode { 
    char* function;
    int scope; 
    struct StackNode* next; 
}; 

static int isEmpty(struct StackNode* root) 
{ 
    return !root; 
} 
struct StackNode* newNode(char* data, int scope) 
{ 
    struct StackNode* stackNode =  (struct StackNode*) malloc(sizeof(struct StackNode ) ); 
    stackNode->function =strdup(data);
    stackNode->scope=scope; 
    stackNode->next = NULL; 
    return stackNode; 
} 


void push(struct StackNode** root, char* data, int scope) 
{ 
    struct StackNode* stackNode = newNode(data,scope); 
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

int peek(struct StackNode* root) 
{ 
    if (isEmpty(root)) 
        return -1; 
    return root->scope; 
} 
