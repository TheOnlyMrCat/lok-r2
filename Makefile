CC := clang
CFLAGS := 
DC := ldc2
DFLAGS := --unittest
LINKFLAGS :=
LEX := flex
LEXFLAGS := --yylineno
YACC := bison
YACCFLAGS :=

OBJECTS := build/lex.o build/grammar.o build/app.o build/parse.o

all: $(OBJECTS)
	$(DC) $(LINKFLAGS) -of=clok $(OBJECTS)

build/app.o: source/app.d
	$(DC) $(DFLAGS) -c -of=build/app.o source/app.d

build/parse.o: source/parse.d
	$(DC) $(DFLAGS) -c -of=build/parse.o source/parse.d

build/lex.o: source/lexer.l build/grammar
	$(LEX) $(LEXFLAGS) -o build/lex.yy.c source/lexer.l
	$(CC) -c -o build/lex.o build/lex.yy.c

build/grammar: source/grammar.y
	$(YACC) $(YACCFLAGS) -d -o build/grammar.c source/grammar.y

build/grammar.o: build/grammar
	$(CC) -c -o build/grammar.o build/grammar.c

clean:
	rm build/*
