bin_PROGRAMS = clok

AM_YFLAGS = -d -Wno-yacc
BUILT_SOURCES = grammar.hpp nodetypenames.h

clok_SOURCES = main.cpp parse.cpp lexer.lpp grammar.ypp nodetypenames.txt
clok_CPPFLAGS = -I../lib

nodetypenames.h: nodetypenames.txt
	xxd -i nodetypenames.txt > nodetypenames.h