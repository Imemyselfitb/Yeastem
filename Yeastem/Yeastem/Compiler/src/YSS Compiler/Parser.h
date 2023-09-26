#pragma once

#include "Tokens.h"
#include "Tokenizer.h"

#include <vector>
#include <iostream>

enum class PARSED_TYPE
{
	NONE = 0,
	NumberLiteral,
	StringLiteral,
	BinaryExpression,
	Assignment,
	Name, // Vars inside of expressions [assignment, function calls]. 
	Var,
	ConstVar,
	KeyWord, // Not defined yet
	Func,
	FuncBody,
	FuncCall,
	ROOT // PROGRAM
};

std::string get_PARSED_TYPE(PARSED_TYPE type)
{
	switch (type)
	{
	case PARSED_TYPE::NumberLiteral:
		return "Number";
	case PARSED_TYPE::StringLiteral:
		return "String";
	case PARSED_TYPE::BinaryExpression:
		return "[Binary] Expression";
	case PARSED_TYPE::Assignment:
		return "Assignment";
	case PARSED_TYPE::Name:
		return "Name";
	case PARSED_TYPE::Var:
		return "Variable";
	case PARSED_TYPE::ConstVar:
		return "Constant Variable";
	case PARSED_TYPE::KeyWord:
		return "KeyWord";
	case PARSED_TYPE::Func:
		return "Function Declaration";
	case PARSED_TYPE::FuncBody:
		return "Function Body";
	case PARSED_TYPE::FuncCall:
		return "Function Call";
	case PARSED_TYPE::ROOT:
		return "Program Root";
	default:
		return "NONE";
	}
}

namespace KeyWord
{
	enum KeyWord
	{
		NONE = 0,

		_const,
		_let, 
		_return
	};

	KeyWord convert(std::string val)
	{
		if (val == "let") return _let;
		if (val == "const") return _const;
		if (val == "ret") return _return;
		return NONE;
	}
}

bool isKeyWord(std::string name)
{
	return KeyWord::convert(name) != KeyWord::NONE;
}

struct DataType
{
	enum class AllTypes
	{
		Number,
		String
	} type;

	DataType() {}
	DataType(std::string STRING, AllTypes type) :type(type)
	{
		if (type == AllTypes::Number)
			this->val = std::stof(STRING);
		else if (type == AllTypes::String)
			this->str = STRING;
	}
	DataType(std::string STRING, PARSED_TYPE type)
	{
		if (type == PARSED_TYPE::NumberLiteral)
		{
			this->val = std::stof(STRING);
			this->type = AllTypes::Number;
		}
		else if (type == PARSED_TYPE::StringLiteral)
		{
			this->str = STRING;
			this->type = AllTypes::String;
		}
	}

	~DataType() {}

	void operator=(const DataType& other)
	{
		this->val = other.val;
		this->type = other.type;
	}

	union
	{
		float val;
		std::string str;
	};
};

bool trySolveOp(const DataType& a, const DataType& b, const char* opCode, DataType& result)
{
	if (a.type == b.type && a.type == DataType::AllTypes::Number)
	{
		if (opCode == "+")
		{
			result.type = DataType::AllTypes::Number;
			result.val = a.val + b.val;
			return true;
		}
		if (opCode == "-")
		{
			result.type = DataType::AllTypes::Number;
			result.val = a.val - b.val;
			return true;
		}
		if (opCode == "*")
		{
			result.type = DataType::AllTypes::Number;
			result.val = a.val * b.val;
			return true;
		}
		if (opCode == "/")
		{
			result.type = DataType::AllTypes::Number;
			result.val = a.val / b.val;
			return true;
		}
	}
}
#define printType true
class ParsedNode
{
public:
	ParsedNode() {};
	ParsedNode(PARSED_TYPE type) :type(type) {}
	ParsedNode(const std::string& str) :str(str) {}
	ParsedNode(const std::string& str, PARSED_TYPE type) :str(str), type(type) {}

	virtual void prune()
	{
		if (this->params.size() == 1 && this->params[0].str == "")
		{
			this->params[0].prune();

			std::vector<ParsedNode> copy = this->params[0].params;
			this->params.clear();

			this->params = copy;
			return;
		}

		for (int i = 0; i < (int)this->params.size(); i++)
		{
			this->params[i].prune();

			if (this->params[i].params.size() < 1 && this->params[i].str == "")
			{
				this->params.erase(this->params.begin() + i);
			}
		}
	}

	virtual void toString(std::string& str, int tabs = 0)
	{
#define _tab "  "
		if (tabs < 0) tabs = 0;
		std::string TAB;
		for (int i = 0; i < tabs; i++) TAB += _tab;

		str += TAB + "{\n";

#if !printType
		str += (TAB + _tab) + "Value: " + this->str;
#else
		str += (TAB + _tab) + "Value: " + this->str +", \n";
		str += (TAB + _tab) + "Type: ";
		str += get_PARSED_TYPE(this->type);
#endif

		if (this->params.size() > 0)
		{
			str += ", \n" + (TAB + _tab);
			str += "Params: [";
			for (int i = 0; i < int(this->params.size()); i++)
			{
				str += "\n";
				this->params[i].toString(str, tabs + 2);
				if (i < int(this->params.size() - 1)) str += ", ";
			}
			str += "\n" + (TAB + _tab) + "]";
		}

		str += "\n" + TAB + "}";
	}
	std::string toString()
	{
		std::string str;
		this->toString(str);
		return str;
	}

	virtual bool tryResolve(DataType& loc)
	{
		switch (this->type)
		{
		case PARSED_TYPE::NumberLiteral:
			loc = { this->str, this->type };
			return true;
		case PARSED_TYPE::StringLiteral:
			loc = { this->str, this->type };
			return true;
		case PARSED_TYPE::BinaryExpression:
			if (true)
			{
				DataType a;
				if (!this->params[0].tryResolve(a)) return false;
				DataType b;
				if (!this->params[1].tryResolve(b)) return false;

				return trySolveOp(a, b, this->str.c_str(), loc);
			}
		default:
			return false;
		}

	}

public:
	std::string str = "";
	PARSED_TYPE type = PARSED_TYPE::NONE;

	std::vector<ParsedNode> params;
};

class FunctionNode : public ParsedNode
{
public:
	FunctionNode() :ParsedNode(PARSED_TYPE::Func) {}
	FunctionNode(const std::string& str) :ParsedNode(str, PARSED_TYPE::Func) {}

	std::string toString()
	{
		std::string str;
		this->toString(str);
		return str;
	}
	void toString(std::string& str, int tabs = 0) override 
	{
#define _tab "  "
		if (tabs < 0) tabs = 0;
		std::string TAB;
		for (int i = 0; i < tabs; i++) TAB += _tab;

		str += TAB + "{\n";

#if !printType
		str += (TAB + _tab) + "Value: " + this->str;
#else
		str += (TAB + _tab) + "Value: " + this->str + ", \n";
		str += (TAB + _tab) + "Type: ";
		str += get_PARSED_TYPE(this->type);
#endif

		if (this->params.size() > 0)
		{
			str += ", \n" + (TAB + _tab);
			str += "Params: [";
			for (int i = 0; i < int(this->params.size()); i++)
			{
				str += "\n";
				this->params[i].toString(str, tabs + 2);
				if (i < int(this->params.size() - 1)) str += ", ";
			}
			str += "\n" + (TAB + _tab) + "]";
		}

		str += ", \n" + (TAB + _tab);
		str += "Body: [";
		for (int i = 0; i < int(this->body.params.size()); i++)
		{
			str += "\n";
			this->body.params[i].toString(str, tabs + 2);
			if (i < int(this->body.params.size() - 1)) str += ", ";
		}
		str += "\n" + (TAB + _tab) + "]";

		str += "\n" + TAB + "}";
	}

	void prune() override
	{
		this->body.prune();
	}

	void appendBody(ParsedNode& node)
	{
		if (node.type != PARSED_TYPE::FuncBody) return;
		this->body = node;
	}

public:
	ParsedNode body; // Only used in functions
};

class RootNode
{
public:
	RootNode() {}

	void toString(std::string& str, int tabs = 0)
	{
		std::string tab = "";
		for (int i = 0; i < tabs; i++) tab += _tab;

		str += tab + "{\n";
		str += (tab + _tab) + "type: " + get_PARSED_TYPE(PARSED_TYPE::ROOT) + "\n";
		str += (tab + _tab) + "Body: [";
		for (FunctionNode& func : this->body)
		{
			str += "\n";
			func.toString(str, tabs + 2);
		}
		str += "\n" + (tab + _tab) + "]\n";
		str += tab + "}";
	}
	std::string toString()
	{
		std::string str;
		this->toString(str);
		return str;
	}

	void prune()
	{
		for (FunctionNode& node : this->body)
		{
			node.prune();
		}
	}
	void appendBody(FunctionNode& node)
	{
		this->body.push_back(node);
	}

public:
	std::vector<FunctionNode> body; // Only functions
	PARSED_TYPE type = PARSED_TYPE::ROOT;
};

class AST
{
public:
	AST() {}

	void prune()
	{
		for (RootNode root : body)
		{
			root.prune();
		}
	}

	void toString(std::string& str)
	{
		const std::string TAB = _tab;
		str += "{\n";
		str += TAB + "body: [";
		for (int i = 0; i < (int)this->body.size(); i++)
		{
			str += "\n";
			this->body[i].toString(str, 2);
			if(i < int(this->body.size() - 1)) str += ", ";
		}
		str += "\n" + TAB + "]\n";
		str += "}";
	}
	std::string toString()
	{
		std::string str;
		this->toString(str);
		return str;
	}

public:
	std::vector<RootNode> body;
};

void error(const char* String)
{
	std::cout << "ERROR: " << String << std::endl;
}

namespace Parser
{
#define isTkType(tk, is_type) tk.type == TOKEN_TYPE::is_type
#define isType(is_type) isTkType(tokens[current], is_type)

	void factor(const std::vector<Token>& tokens, int& current, ParsedNode& node);
	void term(const std::vector<Token>& tokens, int& current, ParsedNode& node);
	void expression(const std::vector<Token>& tokens, int& current, ParsedNode& node);
	ParsedNode name(const std::vector<Token>& tokens, int& current);
	// void condition(const std::vector<Token>& tokens, int& current);
	bool statement(const std::vector<Token>& tokens, int& current, ParsedNode& node);
	bool block(const std::vector<Token>& tokens, int& current, RootNode& root);
}

void Parser::factor(const std::vector<Token>& tokens, int& current, ParsedNode& node)
{
	if (isType(FILE_END)) return;

	if (tokens[current].type == TOKEN_TYPE::NAME)
	{
		node.params.push_back(name(tokens, current));
		current++;
		return;
	}
	if (tokens[current].type == TOKEN_TYPE::STRING)
	{
		node.params.push_back({ tokens[current].value, PARSED_TYPE::StringLiteral });
		current++;
		return;
	}
	if (tokens[current].type == TOKEN_TYPE::NUMBER)
	{
		node.params.push_back({ tokens[current].value, PARSED_TYPE::NumberLiteral });
		current++;
		return;
	}
	if (tokens[current].type == TOKEN_TYPE::LPAR)
	{
		current++;
		ParsedNode exp(PARSED_TYPE::BinaryExpression);
		expression(tokens, current, exp);
		node.params.push_back(exp);
		return;
	}

	if (tokens[current].type == TOKEN_TYPE::NEW_LINE)
		return;

	std::string errorInfo = "Syntax Error: factor ('";
	errorInfo += tokens[current].value;
	errorInfo += "').  ";

	error(errorInfo.c_str());
	current++;
}

ParsedNode Parser::name(const std::vector<Token>& tokens, int& current)
{
	if (isKeyWord(tokens[current].value))
	{
		return { tokens[current].value, PARSED_TYPE::KeyWord };
	}
	else
	{
		return { tokens[current].value, PARSED_TYPE::Name };
	}
}

void addParams(ParsedNode& node, ParsedNode& n)
{
	if (n.type != PARSED_TYPE::BinaryExpression) return node.params.push_back(n);

	if (n.params.size() > 1) // Type Binary Expression
	{
		node.params.push_back(n);
	}
	else if (n.params.size() > 0)
	{
		addParams(node, n.params[0]); // We know it has ONLY ONE child node
	}
}

void Parser::term(const std::vector<Token>& tokens, int& current, ParsedNode& node)
{
	if (isType(FILE_END)) return;

	ParsedNode n(PARSED_TYPE::BinaryExpression);

	factor(tokens, current, n);
	if (isType(RPAR))
	{
		current++;
		return addParams(node, n);
	}

	if (isType(STAR) || isType(SLASH))
	{
		n.str = tokens[current].value;

		current++;
		factor(tokens, current, n);
	}

	if (isType(STAR) || isType(SLASH))
	{
		ParsedNode pN(tokens[current].value, PARSED_TYPE::BinaryExpression);

		current++;
		expression(tokens, current, pN);
		pN.params.insert(pN.params.begin(), n);
		node.params.push_back(pN);
		return;
	}

	addParams(node, n);
}

void Parser::expression(const std::vector<Token>& tokens, int& current, ParsedNode& node)
{
	if (isType(FILE_END)) return;

	ParsedNode n(PARSED_TYPE::BinaryExpression);

	if (isType(LPAR))
	{
		current++;
		expression(tokens, current, n);
	}
	else
	{
		term(tokens, current, n);
	}

	if (isType(RPAR)) {
		current++;
		return addParams(node, n);
	}

	if(isType(PLUS) || isType(MINUS))
	{
		n.str = tokens[current].value;

		current++;
		term(tokens, current, n);
		if (isType(RPAR)) return addParams(node, n);
	}
	else if (isType(STAR) || isType(SLASH))
	{
		n.str = tokens[current].value;

		current++;
		factor(tokens, current, n);
		if (isType(RPAR)) return addParams(node, n);
	}

	if ((isType(PLUS) || isType(MINUS)) || (isType(STAR) || isType(SLASH)) )
	{
		ParsedNode pN(tokens[current].value, PARSED_TYPE::BinaryExpression);

		current++;
		expression(tokens, current, pN);
		pN.params.insert(pN.params.begin(), n);
		node.params.push_back(pN);
		return;
	}

	addParams(node, n);
}

/*
void Parser::condition(const std::vector<Token>& tokens, int& current)
{
	if (isType(FILE_END)) return;

	expression(tokens, current);
	if ((isType(EQ_EQUAL)  || isType(LESS)       || isType(GREATER))      || 
		(isType(NOT_EQUAL) || isType(LESS_EQUAL) || isType(GREATER_EQUAL) ))
	{
		current++;
		expression(tokens, current);
		return;
	}

	error("Syntax Error: condition()");
	current++;
}
*/

bool assignment(const std::vector<Token>& tokens, int& current, ParsedNode& node)
{
	int cur = current;
	ParsedNode child = Parser::name(tokens, cur);
	if (child.type == PARSED_TYPE::KeyWord)
	{
		if(child.str != "const" && child.str != "let")
			return false;
	}

	cur++;
	ParsedNode n("=", PARSED_TYPE::Assignment);

	ParsedNode NAME = Parser::name(tokens, cur);
	if (NAME.type != PARSED_TYPE::Name)
		return false;

	if(child.str == "let") 
		NAME.type = PARSED_TYPE::Var;
	else 
		NAME.type = PARSED_TYPE::ConstVar;

	n.params.push_back(NAME);

	cur++;
	if (isTkType(tokens[cur], EQUAL))
	{
		cur++;
		Parser::expression(tokens, cur, n);

		current = cur + 1;
		addParams(node, n);
		return true;
	}
	return false;
}
bool func(const std::vector<Token>& tokens, int& current, FunctionNode& funcNode)
{
	ParsedNode FUNC = Parser::name(tokens, current);
	if (FUNC.type != PARSED_TYPE::Name)
	{
		return false;
	}

	funcNode.str = FUNC.str;

	current++;
	if (isTkType(tokens[current], LSQB))
	{
		current++;
		while (!(isTkType(tokens[current], RSQB)))
		{
			ParsedNode n = Parser::name(tokens, current);
			current++;
			if (isTkType(tokens[current], COMMA)) current++;
			else break;
		}
		current++;
	}

	if (isTkType(tokens[current], COLON))
	{
		current ++;
		return true;
	}
	return false;
}
bool funcCall(const std::vector<Token>& tokens, int& current, ParsedNode& node)
{
	int cur = current;

	ParsedNode FuncName = Parser::name(tokens, cur);
	if (FuncName.type != PARSED_TYPE::Name)
		return false;
	
	cur++;
	if (!(isTkType(tokens[cur], LPAR))) return false;

	ParsedNode Function(FuncName.str, PARSED_TYPE::FuncCall);

	cur++;
	while (!(isTkType(tokens[cur], RPAR)))
	{
		Parser::expression(tokens, cur, Function);
		if (isTkType(tokens[cur], COMMA)) cur++;
		else break;
	}
	current = cur + 1;

	if (isType(NEW_LINE)) current++;
	node.params.push_back(Function);
	return true;
}
bool returnCall(const std::vector<Token>& tokens, int& current, ParsedNode& node)
{
	ParsedNode ReturnNode = Parser::name(tokens, current);
	if (ReturnNode.type != PARSED_TYPE::KeyWord || ReturnNode.str != "ret")
		return false;

	current++;
	Parser::expression(tokens, current, ReturnNode);

	if (!(isType(NEW_LINE))) return false;
	current++;

	node.params.push_back(ReturnNode);
	return true;
}

bool Parser::statement(const std::vector<Token>& tokens, int& current, ParsedNode& node)
{
	if (isType(FILE_END)) return false;

	while (isType(NEW_LINE)) current++;

	if (isType(NAME)) // Variables & KeyWords
	{
		if (assignment(tokens, current, node))
		{
			return true;
		}
		if (funcCall(tokens, current, node))
		{
			return true;
		}
		if (returnCall(tokens, current, node))
		{
			return true;
		}
	}
	return false;
}

bool Parser::block(const std::vector<Token>& tokens, int& current, RootNode& root)
{
	if (isType(FILE_END)) return false;

	FunctionNode label;
	while (isType(NEW_LINE)) current++;

	if (func(tokens, current, label))
	{
		ParsedNode body(label.str, PARSED_TYPE::FuncBody);

		while (statement(tokens, current, body)) {}
		current--;

		if (body.params.size() < 1) return false;
		label.appendBody(body);
		root.appendBody(label);
		return true;
	}
	return false;
}

void ParseProgram(const std::vector<Token>& tokens)
{
	AST ast;
	
	int current = 0;

	RootNode root;
	while (current < int(tokens.size() - 1))
	{
		Parser::block(tokens, current, root);
		current++;
	}
	root.prune();
	ast.body.push_back(root);

	std::cout << ast.body[0].toString() << std::endl;

	std::cin.get();
}
