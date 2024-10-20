Alpha Language Compiler - README
Project Overview
This project involves the creation of a compiler for the Alpha programming language, divided into five phases. Each phase builds on the previous, beginning with lexical analysis and culminating in the generation of target and virtual machine code. The compiler is implemented in C/C++ using tools like flex for lexical analysis and bison for syntax analysis.

Project Structure
```scss

├── Lexical_Analysisi
│   ├── Makefile
│   ├── scanner1.l (lexical analyzer file)
│   └── list.c (helper list functions)
├── Syntax_analysis
│   ├── Makefile
│   ├── parser.y (grammar specification)
│   ├── scanner1.l (updated lexical analyzer)
│   ├── symtablehash.c (symbol table implementation)
│   ├── stack.c (stack structure implementation)
├── Intermediate_code
│   ├── Makefile
│   ├── parser.y (extended with semantic rules)
│   ├── scanner.l (lexical analyzer)
│   ├── symtablehash.c
│   ├── expr.c (intermediate code generation for expressions)
├── Target_code
│   ├── Makefile
│   ├── parser.y
│   ├── vmarg.c (generation of VM target code)
│   ├── symtablehash.c
│   └── other supporting files
├── Virtual_Machine
│   ├── Makefile
│   ├── avm.c (alpha virtual machine implementation)
│   ├── vmarg.c
│   └── other supporting files
└── README.md
```
Phase 1: Lexical Analysis
Description
In this phase, the lexical analyzer for the Alpha language is created using flex. The analyzer identifies tokens from the source code, such as keywords, operators, and identifiers. The output is the sequence of tokens printed to standard output.

Files
scanner1.l: The lex file containing token definitions.
list.c: A helper C file for managing token lists.
Makefile
makefile

CC=gcc
CFLAGS=-g
LDFLAGS=-lfl
OBJS=lex.alpha_yy.o list.o

obj: lex
	gcc -g list.o lex.alpha_yy.c -o lexer -lfl

lex: scanner1.l
	flex scanner1.l

list: list.o
	$(CC) $(CFLAGS) list.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
Phase 2: Syntax Analysis
Description
The syntax analysis phase introduces a parser for Alpha's grammar using bison. This parser checks for syntactic correctness and prints the grammar rules as they are applied. A symbol table is also created in this phase.

Files
parser.y: Contains the grammar for Alpha and its rules.
scanner1.l: The updated lexical analyzer for compatibility with the parser.
symtablehash.c: Symbol table implementation.
stack.c: Stack implementation for managing scopes.
Makefile
makefile

CC=gcc
CFLAGS=-g
LDFLAGS=-lfl
BISONFLAGS=-d -v
OBJS=scanner1.o parser.o list.o symtablehash.o stack.o
EXEC=scanner

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

parser.tab.c parser.tab.h: parser.y
	bison $(BISONFLAGS) parser.y

scanner1.c: scanner1.l
	flex -o scanner1.c scanner1.l

clean:
	rm -f $(EXEC) $(OBJS) scanner1.c parser.tab.c parser.tab.h
Phase 3: Intermediate Code Generation
Description
In this phase, we add semantic rules to the parser and generate intermediate code for Alpha programs. The output is stored in the quads.txt file.

Files
parser.y: Contains the grammar and semantic rules.
expr.c: Handles the generation of intermediate code for expressions.
Makefile
makefile

CC=gcc
CFLAGS=-g
LDFLAGS=-lfl
BISONFLAGS=-d -v
OBJS=scanner.o parser.o src/list/list.o src/symtable/symtablehash.o src/stack/stack.o src/expr/expr.o
EXEC=scanner

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(EXEC) $(OBJS) scanner.c parser.tab.c parser.tab.h parser.output
Phase 4: Target Code Generation
Description
In this phase, we generate the target code for the Alpha virtual machine. The code is output in both binary and human-readable forms.

Files
vmarg.c: Responsible for generating target code for the virtual machine.
Makefile
makefile

CC=gcc
CFLAGS=-g
LDFLAGS=-lfl
BISONFLAGS=-d -v
OBJS=scanner.o parser.o src/list/list.o src/symtable/symtablehash.o src/stack/stack.o src/expr/expr.o src/vmarg/vmarg.o
EXEC=scanner

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(EXEC) $(OBJS) scanner.c parser.tab.c parser.tab.h parser.output
Phase 5: Virtual Machine
Description
This phase implements the Alpha Virtual Machine (AVM) to execute the target code generated in phase 4. It also includes the implementation of required library functions.

Files
avm.c: Implements the virtual machine.
Makefile
makefile

CC=gcc
CFLAGS=-g
LDFLAGS=-lfl
BISONFLAGS=-d -v
OBJS=scanner.o parser.o src/list/list.o src/symtable/symtablehash.o src/stack/stack.o src/expr/expr.o src/vmarg/vmarg.o src/avm/avm.o
EXEC=scanner

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(EXEC) $(OBJS) scanner.c parser.tab.c parser.tab.h parser.output
