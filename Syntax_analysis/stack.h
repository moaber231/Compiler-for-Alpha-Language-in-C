
struct StackNode* newNode(char* data, int scope);

void push(struct StackNode** root, char* data, int scope);

char* pop(struct StackNode** root);

int peek(struct StackNode* root);  
