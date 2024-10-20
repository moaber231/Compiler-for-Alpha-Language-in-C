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

scanner1.o: scanner1.c parser.tab.h
	$(CC) $(CFLAGS) -c scanner1.c -o scanner1.o

parser.o: parser.tab.c parser.tab.h
	$(CC) $(CFLAGS) -c parser.tab.c -o parser.o

list.o: list.c
	$(CC) $(CFLAGS) -c list.c -o list.o

symtablehash.o: symtablehash.c symtable.h
	$(CC) $(CFLAGS) -c symtablehash.c -o symtablehash.o

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c -o stack.o

clean:
	rm -f $(EXEC) $(OBJS) scanner1.c parser.tab.c parser.tab.h parser.output stack.o

