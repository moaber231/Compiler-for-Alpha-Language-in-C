Phase 2: Syntax Analysis
Navigate to the directory:

```bash
cd phase2_syntax_analysis
```
Run make to build the parser:

```bash
make
```
Execute the parser with an Alpha source file:

```bash
./scanner input_file.txt
```
Expected Output: The program will print the grammatical rules applied during the parsing of the source code. Additionally, at the end of the parsing, it will print the symbol table, which includes:

Symbol name
Type (e.g., variable, function)
Definition line number
Scope (local or global)
Example:

```yaml
Applied rule: expr → term
Applied rule: term → IDENT
Symbol table:
- x: variable, line 2, scope 0
- print: function (library), line 0, scope 0
```
