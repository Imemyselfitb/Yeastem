#pragma once

#include "Macros.h"

#include "Tokens.h"
#include "Tokenizer.h"

#include <vector>
#include <iostream>

YEASTEM_COMPILER_BEGIN

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

const char* get_PARSED_TYPE(PARSED_TYPE type);

namespace KeyWord
{
	enum class KeyWords
	{
		NONE = 0,

		Const,
		Let, 
		Return
	};

	KeyWords convert(const std::string& val);
}

inline bool isKeyWord(std::string name)
{
	return KeyWord::convert(name) != KeyWord::KeyWords::NONE;
}

struct DataType
{
	enum class AllTypes
	{
		None = 0,
		Number,
		String
	} type;

	DataType() :type(AllTypes::None) {}
	DataType(const std::string& STRING, AllTypes type);
	DataType(const std::string& STRING, PARSED_TYPE type);

	~DataType() {}

	void operator=(const DataType& other);

	// The Value Can Be Either a String (in which case "str" is used)
	// or a Number(in which case "val" is used)
	union 
	{
		float val = 0;
		std::string str;
	};
};

bool trySolveOp(const DataType& a, const DataType& b, const char* opCode, DataType& result);

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

	virtual void toString(std::string& str, int tabs = 0) const
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
	std::string toString() const
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
	void toString(std::string& str, int tabs = 0) const override 
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

	void toString(std::string& str, int tabs = 0) const
	{
		std::string tab = "";
		for (int i = 0; i < tabs; i++) tab += _tab;

		str += tab + "{\n";
		str += (tab + _tab) + "type: " + get_PARSED_TYPE(PARSED_TYPE::ROOT) + "\n";
		str += (tab + _tab) + "Body: [";
		for (const FunctionNode& func : this->body)
		{
			str += "\n";
			func.toString(str, tabs + 2);
		}
		str += "\n" + (tab + _tab) + "]\n";
		str += tab + "}";
	}
	std::string toString() const
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

#define error(String) std::cout << "ERROR: " << String << std::endl;

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

void ParseProgram(const std::vector<Token>& tokens);

YEASTEM_COMPILER_END
