bin_PROGRAMS = clok

AM_YFLAGS = -d -Wno-yacc
BUILT_SOURCES = grammar.hpp nodetypenames.h location.hh

clok_SOURCES = main.cpp bridge.cpp lexer.lpp grammar.ypp nodetypenames.txt
clok_CPPFLAGS = -I../lib -Wno-deprecated-register

nodetypenames.h: nodetypenames.txt
	xxd -i nodetypenames.txt > nodetypenames.h