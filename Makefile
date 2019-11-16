CC := clang
CFLAGS := 
DC := ldc2
DFLAGS := --unittest -I=./source
LINKFLAGS :=
LEX := flex
LEXFLAGS := --yylineno
YACC := bison
YACCFLAGS :=

OBJECTS := build/lex.o build/grammar.o build/app.o build/parsing.o

all: $(OBJECTS)
	$(DC) $(LINKFLAGS) -of=clok $(OBJECTS)

build/app.o: source/app.d
	$(DC) $(DFLAGS) -c -of=build/app.o source/app.d

build/parsing.o: source/parsing.d
	$(DC) $(DFLAGS) -c -of=build/parsing.o source/parsing.d

build/lex.o: source/lexer.l build/grammar.h
	$(LEX) $(LEXFLAGS) -o build/lex.yy.c source/lexer.l
	$(CC) -c -o build/lex.o build/lex.yy.c

build/grammar.c: build/grammar.h
build/grammar.h: source/grammar.y
	$(YACC) $(YACCFLAGS) -d -o build/grammar.c source/grammar.y

build/grammar.o: build/grammar.c
	$(CC) -c -o build/grammar.o build/grammar.c

clean:
	rm build/*
