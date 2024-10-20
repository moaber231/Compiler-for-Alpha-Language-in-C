#include "../expr/expr.h"
struct StackNode* newNode(char* data, int scope,struct expr* exp);

void push(struct StackNode** root, char* data, int scope,struct expr *e);

char* pop(struct StackNode** root);

struct expr* pop_exp(struct StackNode** root);

int peek(struct StackNode* root);  
