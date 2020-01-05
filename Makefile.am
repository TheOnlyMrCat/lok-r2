bin_PROGRAMS = clok

AM_CPPFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS)
AM_YFLAGS = -d -Wno-yacc
BUILT_SOURCES = src/grammar.hpp src/nodetypenames.h

clok_SOURCES = src/main.cpp src/bridge.cpp src/constructs.cpp src/lexer.lpp src/grammar.ypp src/nodetypenames.txt
clok_CPPFLAGS = -Ilib
clok_CXXFLAGS = -Wno-deprecated-register
clok_LDADD = $(BOOST_SYSTEM_LIB) $(BOOST_FILESYSTEM_LIB)

nodetypenames.h: nodetypenames.txt
	xxd -i nodetypenames.txt > nodetypenames.h

clean-local:
	rm -f src/grammar.cpp src/grammar.hpp src/grammar.output src/lexer.cpp src/location.hh

debug: YFLAGS += -Dparse.trace --report state
debug: CXXFLAGS += -O0 -DDEBUG
debug: all