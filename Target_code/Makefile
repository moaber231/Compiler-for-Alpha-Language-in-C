CC=gcc 
CFLAGS=-g
LDFLAGS=-lfl 
BISONFLAGS=-d -v 

OBJS=scanner.o parser.o src/list/list.o src/symtable/symtablehash.o src/stack/stack.o src/expr/expr.o src/vmarg/vmarg.o

EXEC=scanner

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

parser.tab.c parser.tab.h: parser.y
	bison $(BISONFLAGS) parser.y

scanner.c: scanner.l parser.tab.h
	flex -o scanner.c scanner.l

scanner.o: scanner.c parser.tab.c
	$(CC) $(CFLAGS) -c scanner.c -o scanner.o

parser.o: parser.tab.c parser.tab.h
	$(CC) $(CFLAGS) -c parser.tab.c -o parser.o

src/list/list.o: src/list/list.c src/list/list.h
	$(CC) $(CFLAGS) -c src/list/list.c -o src/list/list.o

src/symtable/symtablehash.o: src/symtable/symtablehash.c src/symtable/symtable.h
	$(CC) $(CFLAGS) -c src/symtable/symtablehash.c -o src/symtable/symtablehash.o

src/stack/stack.o: src/stack/stack.c src/stack/stack.h
	$(CC) $(CFLAGS) -c src/stack/stack.c -o src/stack/stack.o

src/expr/expr.o: src/expr/expr.c src/expr/expr.h src/symtable/symtable.h
	$(CC) $(CFLAGS) -c src/expr/expr.c -o src/expr/expr.o
	
src/vmarg/vmarg.o: src/vmarg/vmarg.c src/vmarg/vmarg.h src/stack/stack.h
	$(CC) $(CFLAGS) -c src/vmarg/vmarg.c -o src/vmarg/vmarg.o

clean:
	rm -f $(EXEC) $(OBJS) scanner.c parser.tab.c parser.tab.h parser.output
