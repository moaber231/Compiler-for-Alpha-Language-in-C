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
In this phase, we create a lexical analyzer (or lexer) for the Alpha language using the flex tool. The lexical analyzer processes the source code of Alpha programs, breaking it down into tokens. These tokens represent individual elements of the language, such as keywords (if, else, etc.), operators (+, -, *, /, etc.), identifiers (variable names, function names), and constants (numbers, strings). This is the first step of the compilation process, where the input program is scanned and broken down into a sequence of meaningful symbols for the next phase.

The scanner1.l file contains the rules and patterns that define how different tokens are identified.
The tokens are printed to the console with details like line numbers and types.
What Happens

The lexer scans the input source code file and identifies tokens, such as keywords, operators, literals, and identifiers.
Each recognized token is output, and its type is categorized (e.g., KEYWORD, IDENTIFIER, INTCONST).
For example, if the input contains if (x > 5), it will output tokens for if, (, x, >, and 5.

Phase 2: Syntax Analysis
Description
In the syntax analysis phase, a parser is created using bison. The parser checks whether the token sequence produced by the lexer follows the syntactical rules (grammar) of the Alpha language. It performs a structured analysis of the program, verifying that tokens are arranged correctly to form valid statements, expressions, and program constructs.

This phase also introduces a symbol table to store variables, functions, and their scopes. The parser works with a defined grammar, identifying structures such as loops, conditionals, and function definitions.

What Happens

The parser analyzes the sequence of tokens and verifies if they conform to the Alpha language's grammar rules.
If an invalid token sequence (like an unclosed parenthesis) is found, an error is raised.
For valid code, the parser outputs the applied grammar rules, indicating how the program is structured.
A symbol table is generated, which keeps track of the variables, functions, and their properties (like their scope, type, and location).
Phase 3: Intermediate Code Generation
Description
In this phase, we extend the parser to generate intermediate code. Intermediate code is a low-level representation of the source program, abstracted from the specifics of the target machine. This code is easier to optimize and can later be translated into machine code for different platforms.

We use semantic rules to produce this intermediate code for expressions, statements, and control flows such as loops and conditionals. The output is written to a file called quads.txt in the form of quads (4-tuples), representing operations, operands, and results.

What Happens

After successful parsing, the intermediate code generation process begins.
For each statement and expression in the source program, the parser generates corresponding quad instructions (which consist of an operation, two operands, and a result).
This phase handles expression evaluations, variable assignments, and flow control (if statements, loops).
The generated intermediate code is saved in the quads.txt file, allowing for future optimizations or machine code generation.
Example quad:

```yaml
1: ASSIGN x, 10  # Assigns the value 10 to x
```
Phase 4: Target Code Generation
Description
In the target code generation phase, we convert the intermediate code into target code for the Alpha Virtual Machine (VM). This phase outputs instructions that a virtual machine can execute. The target code is generated both in a binary format (for execution) and a human-readable format (for debugging or review).

The goal is to translate each intermediate code quad into corresponding VM instructions, handling tasks like arithmetic operations, function calls, and memory management.

What Happens

Each intermediate code quad is translated into machine-like instructions that the Alpha Virtual Machine (AVM) can execute.
The generated target code is produced in two formats: a binary version for execution and a textual version for easier debugging.
The VM instructions handle operations such as addition, subtraction, jumps, and function calls.
Example target code (human-readable form):

```yaml
0: ASSIGN x, 10
1: IF_EQ x, 10, L1  # Jump to label L1 if x equals 10
```

Sure! Here's a detailed description of what happens in each phase of the compiler construction for the Alpha language, from Lexical Analysis to the Virtual Machine implementation.

Phase 1: Lexical Analysis
Description
In this phase, we create a lexical analyzer (or lexer) for the Alpha language using the flex tool. The lexical analyzer processes the source code of Alpha programs, breaking it down into tokens. These tokens represent individual elements of the language, such as keywords (if, else, etc.), operators (+, -, *, /, etc.), identifiers (variable names, function names), and constants (numbers, strings). This is the first step of the compilation process, where the input program is scanned and broken down into a sequence of meaningful symbols for the next phase.

The scanner1.l file contains the rules and patterns that define how different tokens are identified.
The tokens are printed to the console with details like line numbers and types.
What Happens

The lexer scans the input source code file and identifies tokens, such as keywords, operators, literals, and identifiers.
Each recognized token is output, and its type is categorized (e.g., KEYWORD, IDENTIFIER, INTCONST).
For example, if the input contains if (x > 5), it will output tokens for if, (, x, >, and 5.
Phase 2: Syntax Analysis
Description
In the syntax analysis phase, a parser is created using bison. The parser checks whether the token sequence produced by the lexer follows the syntactical rules (grammar) of the Alpha language. It performs a structured analysis of the program, verifying that tokens are arranged correctly to form valid statements, expressions, and program constructs.

This phase also introduces a symbol table to store variables, functions, and their scopes. The parser works with a defined grammar, identifying structures such as loops, conditionals, and function definitions.

What Happens

The parser analyzes the sequence of tokens and verifies if they conform to the Alpha language's grammar rules.
If an invalid token sequence (like an unclosed parenthesis) is found, an error is raised.
For valid code, the parser outputs the applied grammar rules, indicating how the program is structured.
A symbol table is generated, which keeps track of the variables, functions, and their properties (like their scope, type, and location).
Phase 3: Intermediate Code Generation
Description
In this phase, we extend the parser to generate intermediate code. Intermediate code is a low-level representation of the source program, abstracted from the specifics of the target machine. This code is easier to optimize and can later be translated into machine code for different platforms.

We use semantic rules to produce this intermediate code for expressions, statements, and control flows such as loops and conditionals. The output is written to a file called quads.txt in the form of quads (4-tuples), representing operations, operands, and results.

What Happens

After successful parsing, the intermediate code generation process begins.
For each statement and expression in the source program, the parser generates corresponding quad instructions (which consist of an operation, two operands, and a result).
This phase handles expression evaluations, variable assignments, and flow control (if statements, loops).
The generated intermediate code is saved in the quads.txt file, allowing for future optimizations or machine code generation.
Example quad:

yaml

1: ASSIGN x, 10  # Assigns the value 10 to x
Phase 4: Target Code Generation
Description
In the target code generation phase, we convert the intermediate code into target code for the Alpha Virtual Machine (VM). This phase outputs instructions that a virtual machine can execute. The target code is generated both in a binary format (for execution) and a human-readable format (for debugging or review).

The goal is to translate each intermediate code quad into corresponding VM instructions, handling tasks like arithmetic operations, function calls, and memory management.

What Happens

Each intermediate code quad is translated into machine-like instructions that the Alpha Virtual Machine (AVM) can execute.
The generated target code is produced in two formats: a binary version for execution and a textual version for easier debugging.
The VM instructions handle operations such as addition, subtraction, jumps, and function calls.
Example target code (human-readable form):

```yaml
0: ASSIGN x, 10
1: IF_EQ x, 10, L1  # Jump to label L1 if x equals 10
```
Phase 5: Virtual Machine
Description
In the final phase, we implement the Alpha Virtual Machine (AVM), which can execute the target code generated in the previous phase. The AVM is responsible for loading the binary target code, interpreting it, and executing the instructions step by step. This phase also includes the implementation of core library functions required for the language, such as print, input, and typeof.

The AVM manages the program's memory, handles function calls, and executes the virtual instructions.

What Happens

The AVM loads the binary target code and begins executing the virtual instructions.
It simulates the behavior of a physical machine, handling operations like arithmetic, control flow (jumps, conditionals), function calls, and memory management.
You can see the actual execution of your Alpha program, including outputs from functions like print.
This phase brings the compiler to completion, allowing the compiled Alpha programs to run in a controlled virtual environment.
Example of VM execution:

A program containing print(x) would output the value of x to the terminal after being interpreted by the AVM.
Each phase builds progressively on the previous one, starting with token recognition and culminating in full execution on the virtual machine. Each stage plays a vital role in the compiler pipeline, transforming high-level Alpha code into executable virtual machine code.
