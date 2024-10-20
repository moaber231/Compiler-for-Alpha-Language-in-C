Phase 5: Virtual Machine (VM) Execution

```bash
cd Virtual_Machine
```
Run make to build the Alpha Virtual Machine (AVM):

```bash
make
```
Execute the virtual machine with the generated target code:

```bash
./scanner input_file.txt
```
Expected Output: The virtual machine will execute the target code and produce output based on the input Alpha program. You should see the results of your program, such as function outputs, arithmetic operations, etc.

Example:Input
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

```bash
	3.000 6.000 9.000 54.000
	18.000 9.000
	54.000 54.000 54.000 54.000
	0.000 0.000
	-9.000
	20.000
	16.500
	0.000
	3.000 6.000 6.000 9.000 0.000
```
