#pragma once

#include <string>

enum class TOKEN_TYPE
{
	NONE = 0, 

	// GENERALISED DATA TYPES
	NEW_LINE, 
	INDENT, 
	DEDENT, 
	STRING, 
	NUMBER, 
	NAME, 

	// Parenthesis
	LPAR, // "(" 
	RPAR, // ")"
	LSQB, // "["
	RSQB, // "]"
	LBRACE, // "{"
	RBRACE, // "}"

	COLON, // ":"
	SEMIC, // ";"
	COMMA, // ","
	DOT, // "."
	TILDE, // "~"

	PLUS, // "+"
	MINUS, // "-"
	STAR, // "*"
	SLASH, // "/"
	BSLASH, // "\"
	AMPER, // "&"
	VBAR, // "|"
	CARROT, // "^"

	EQ_EQUAL, // "=="
	NOT_EQUAL, // "!="
	LESS_EQUAL, // "<="
	GREATER_EQUAL,  // ">="
	LESS, // "<"
	GREATER, // ">"

	EQUAL, // "="

	FILE_END
};

class Token
{
public:
	Token() {}
	Token(TOKEN_TYPE type) :type(type) {}
	Token(const char* value, TOKEN_TYPE type) :type(type), value(value) {}

	const char* value = "";
	TOKEN_TYPE type = TOKEN_TYPE::NONE;
};