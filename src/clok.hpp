#pragma once

#include <string>
#include <memory>

#include <plog/Log.h>

typedef size_t strings_t;

union ValueType {
	long long valI;
	double valF;
	strings_t valC;
};

/* Currently, these are in no particular order.
 * When released, the order will be normalised.
 */

enum NodeType {
	NONE,
	LOAD,
	RUN,
	FUNCDEF,
	BLOCK,
	TYPESINGLE,
	TYPEMULTI,
	TYPEFN,
	PARAMLIST,
	PARAM,
	DECL,
	LIBNAME,
	FILEPATH,
	QUALID,
	QUALIDOP,
	QUALPART,
	EXPRASSIG,
	EXPRBASIC,
	EXPRDECL,
	VALINT,
	VALFLOAT,
	VALSTR
};

struct Node {
	Node(int type, int children) : type(type), children(std::vector<std::unique_ptr<Node>>(children)) {}

	int type;
	union ValueType value;

	std::vector<std::unique_ptr<Node>> children;
};

extern std::unique_ptr<Node> parseResult;

strings_t getString(std::string string);