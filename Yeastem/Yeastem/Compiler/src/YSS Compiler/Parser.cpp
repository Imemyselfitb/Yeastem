#include "Macros.h"
#include "Parser.h"

YEASTEM_COMPILER_BEGIN

const char* get_PARSED_TYPE(PARSED_TYPE type)
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
	return false;
}

KeyWord::KeyWords KeyWord::convert(const std::string& val)
{
	if (val == "let") return KeyWords::Let;
	if (val == "const") return KeyWords::Const;
	if (val == "ret") return KeyWords::Return;
	return KeyWords::NONE;
}

DataType::DataType(const std::string& STRING, AllTypes type) 
	:type(type)
{
	if (type == AllTypes::Number)
		this->val = std::stof(STRING);
	else if (type == AllTypes::String)
		this->str = STRING;
}
DataType::DataType(const std::string& STRING, PARSED_TYPE type)
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
void DataType::operator=(const DataType& other)
{
	this->val = other.val;
	this->type = other.type;
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

static void addParams(ParsedNode& node, ParsedNode& n)
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
static bool assignment(const std::vector<Token>& tokens, int& current, ParsedNode& node)
{
	int cur = current;
	ParsedNode child = Parser::name(tokens, cur);
	if (child.type == PARSED_TYPE::KeyWord)
	{
		if (child.str != "const" && child.str != "let")
			return false;
	}

	cur++;
	ParsedNode n("=", PARSED_TYPE::Assignment);

	ParsedNode NAME = Parser::name(tokens, cur);
	if (NAME.type != PARSED_TYPE::Name)
		return false;

	if (child.str == "let")
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
static bool func(const std::vector<Token>& tokens, int& current, FunctionNode& funcNode)
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
		current++;
		return true;
	}
	return false;
}
static bool funcCall(const std::vector<Token>& tokens, int& current, ParsedNode& node)
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
static bool returnCall(const std::vector<Token>& tokens, int& current, ParsedNode& node)
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

	if (isType(PLUS) || isType(MINUS))
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

	if ((isType(PLUS) || isType(MINUS)) || (isType(STAR) || isType(SLASH)))
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

	std::cout << "Start Blocky!\n";

	FunctionNode label;
	while (isType(NEW_LINE)) current++;

	if (func(tokens, current, label))
	{
		ParsedNode body(label.str, PARSED_TYPE::FuncBody);

		std::cout << "Start!\n";
		while (statement(tokens, current, body)) {}
		std::cout << "End!\n";
		current--;

		if (body.params.size() < 1) return false;
		label.appendBody(body);
		root.appendBody(label);
		return true;
	}
	std::cout << "End Blocky!\n";
	return false;
}

void ParseProgram(const std::vector<Token>& tokens)
{
	AST ast;

	int current = 0;

	RootNode root;
	std::cout << root.body.size() << "\n";
	while (current < int(tokens.size() - 1))
	{
		Parser::block(tokens, current, root);
		current++;
	}
	std::cout << root.body.size() << "\n";
	root.prune();
	ast.body.push_back(root);

	std::cout << ast.body[0].toString() << std::endl;

	std::cin.get();
}

YEASTEM_COMPILER_END
