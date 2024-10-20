#include <stdbool.h>
typedef struct Variable {
	char *name;
	unsigned int scope;
	unsigned int line;
} Variable;

typedef struct function {
	char *name;
	//List of arguments
	unsigned int scope;
	unsigned int line;
} function;

enum SymbolType {
	GLOBAL, LOCAL, FORMAL,
	USERFUNC, LIBFUNC
};

typedef struct SymbolTableEntry {
	bool isActive;
	union {
		Variable *varVal;
		function *funcVal;
	} value;
	enum SymbolType type;
	struct SymbolTableEntry* next;
} SymbolTableEntry;

struct SymTable
{
	SymbolTableEntry **head;
	int max_scope;
	unsigned int length;
};
typedef struct SymTable* SymTable_T;

SymTable_T SymTable_new(void);

void SymTable_free(SymTable_T oSymTable);

unsigned int SymTable_getLength(SymTable_T oSymTable);

int SymTable_insert(SymTable_T oSymTable,  char *name, unsigned int scope, unsigned int line,  enum SymbolType type);

int SymTable_remove(SymTable_T oSymTable,  char* name ,  enum SymbolType type);

bool SymTable_contains(SymTable_T oSymTable,   char* name ,  enum SymbolType type, int scope, int insideFunction);

void *SymTable_get(SymTable_T oSymTable,   char* name ,  enum SymbolType type, int scope);

void SymTable_hide(SymTable_T oSymTable,  unsigned int scope);

void show_hidden(SymTable_T oSymTable);

void print_all(SymTable_T oSymTable);
