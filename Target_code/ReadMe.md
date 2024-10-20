Phase 4: Target Code Generation
Navigate to the directory:

```bash
cd Target_code
Run make to build the target code generator:
```
```bash
make
```
Execute the parser to generate target (VM) code:

```bash
./scanner input_file.txt
```
Expected Output: The program will produce both binary and textual forms of the target code. The binary output will be stored in a file, while the human-readable version will appear in the terminal.
Example code input in Alpha language
```bash
/*
    Test file for the final phase of HY-340: Languages & Compilers
    Computer science dpt, University of Crete, Greece

    Expected Output:
	3.000 6.000 9.000 54.000
	18.000 9.000
	54.000 54.000 54.000 54.000
	0.000 0.000
	-9.000
	20.000
	16.500
	0.000
	3.000 6.000 6.000 9.000 0.000
*/

nl = "\n";
sp = " ";

a = 3;
b=6;
c = 9;
d  =  54;

print(a, sp, b, sp, c, sp, d, nl);

a = 4;
a = a+5;
b = a;
a = a+b;

print(a, sp, b, nl);

a = b = c = d;

print(a, sp, b, sp, c, sp, d, nl);

a = b = -(-(c+d)*(a-b))%-b;

print(a, sp, b, nl);

a = 4;
b = 5;
c = 7;
d = 8;

e = a+b+c+d;
e = a+(b+(c+d));
e = -a+b;
e = -(a+b);

print(e, nl);

e = a*b-c/d%e;

print(e, nl);

e = a*(b-c/(d%e));

print(e, nl);

e = -(-(a+b)/-(c+d))%e;

print(e, nl);

a---++b+--c-d++;

print(a, sp, b, sp, c, sp, d, sp, e, nl);

```
Example of human-readable target code:

```makefile
0: ASSIGN x, 10
1: IF_EQ x, 10, 3
2: JUMP 4
3: CALL print
```
