CC ?= clang
CFLAGS := -Isource -Ibuild
DC ?= ldc2
DFLAGS := -unittest -I=source -J=source
LINKFLAGS :=
FLEX ?= flex
FLEXFLAGS :=
BISON ?= bison
BISONFLAGS :=

ifeq ($(DC),ldc2)
	DDEBUGFLAG := -d-debug
else
	DDEBUGFLAG := -debug
endif

OBJECTS := build/lex.o build/grammar.o build/app.o build/parsing.o build/interface.o

all: build $(OBJECTS)
	$(DC) $(LINKFLAGS) -of=clok $(OBJECTS)

build:
	test -d build || mkdir build

build/app.o: source/app.d
	$(DC) $(DFLAGS) -c -of=build/app.o source/app.d

build/parsing.o: source/parsing.d source/nodetypes.d
	$(DC) $(DFLAGS) -c -of=build/parsing.o source/parsing.d

build/interface.o: source/interface.c build/grammar.h
	$(CC) $(CFLAGS) -c -o build/interface.o source/interface.c

build/lex.o: source/lexer.l build/grammar.h
	$(FLEX) $(FLEXFLAGS) -o build/lex.yy.c source/lexer.l
	$(CC) $(CFLAGS) -c -o build/lex.o build/lex.yy.c

build/grammar.c: build/grammar.h
build/grammar.h: source/grammar.y source/nodetypes.d
	$(BISON) $(BISONFLAGS) -d -o build/grammar.c source/grammar.y

build/grammar.o: build/grammar.c
	$(CC) $(CFLAGS) -c -o build/grammar.o build/grammar.c

clean:
	rm build/*

debug: CFLAGS += -g
debug: DFLAGS += -gc $(DDEBUGFLAG)
debug: FLEXFLAGS += -d
debug: BISONFLAGS += --report=state --debug
debug: all
