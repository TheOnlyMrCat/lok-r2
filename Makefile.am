bin_PROGRAMS = clok

AM_CPPFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS)
AM_YFLAGS = -d -Wno-yacc
BUILT_SOURCES = src/grammar.hpp src/nodetypenames.h
EXTRA_DIST = lib test src/nodetypenames.txt

clok_SOURCES = src/main.cpp src/bridge.cpp src/constructs.cpp src/extrapolate.cpp src/util.cpp src/lexer.lpp src/grammar.ypp src/nodetypenames.txt
clok_CPPFLAGS = -Ilib
clok_CXXFLAGS = -Wno-deprecated-register
clok_LDADD = $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)

TESTS = test/functions.lok test/expressions.lok test/load.lok test/statements.lok test/types.lok test/namespace.lok
TEST_EXTENSIONS = .lok
LOK_LOG_COMPILER = ./cloktest.sh

src/nodetypenames.h: src/nodetypenames.txt
	xxd -i src/nodetypenames.txt > src/nodetypenames.h

clean-local:
	rm -f src/grammar.cpp src/grammar.hpp src/grammar.output src/lexer.cpp src/location.hh

debug: YFLAGS += -Dparse.trace --report state
debug: LFLAGS += --debug
debug: CXXFLAGS += -O0 -DDEBUG
debug: AM_MAKEFLAGS += 'CXXFLAGS=$(CXXFLAGS)' 'LFLAGS=$(LFLAGS)'
debug: all