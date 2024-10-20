#include "../expr/expr.h"
struct StackNode* newNode(char* data, int scope,struct expr* exp,struct symbol *s);

void push(struct StackNode **root, char *data, int scope, struct expr *exp,struct symbol *s);

char* pop(struct StackNode** root);

struct expr* pop_exp(struct StackNode** root);

int peek(struct StackNode* root);  
