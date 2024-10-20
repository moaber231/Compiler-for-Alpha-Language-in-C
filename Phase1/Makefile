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
%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<
