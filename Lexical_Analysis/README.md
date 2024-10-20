How to Run
Phase 1: Lexical Analysis
Navigate to the directory:
```bash
cd Lexical_analysis
```
Run make to build the lexical analyzer:
```bash
make
```
Execute the lexical analyzer with an input file containing Alpha language code:
```bash
./lexer input_file.txt
```
Expected Output: The program will output each token found in the input file. The format will be:
```php
<line_number>: #<token_number> "<token_value>" <TOKEN_TYPE>
```
Example:
```yaml
1: #1 "if" KEYWORD IF
2: #2 "x" IDENTIFIER
3: #3 "10" INTCONST
```
