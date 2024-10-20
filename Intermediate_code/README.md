Phase 3: Intermediate Code Generation
Navigate to the directory:

```bash
cd Intermediate_code
```
Run make to build the intermediate code generator:

```bash
make
```
Execute the parser to generate intermediate code:

```bash
./scanner input_file.txt
```
Expected Output: After successful syntactical and semantic analysis, the intermediate code will be written to a file quads.txt. The file will contain a series of quad instructions that represent the intermediate form of the input program.

Example content of quads.txt:

```makefile
1: ASSIGN x, 10
2: IF_EQ x, 10, L1
3: JUMP L2
L1: PRINT x
```
