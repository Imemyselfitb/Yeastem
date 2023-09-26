#pragma once

// Extract taken from `Parser.h`

struct DataType
{
	DataType() {}

	DataType(const DataType& n)
	{
		if (n.type == DataType_types::Node) this->node = n.node;
		if (n.type == DataType_types::String) this->str = n.str;
		if (n.type == DataType_types::Number) this->value = n.value;
	}

	DataType(RootNode& root) :type(DataType_types::Root), root(root) {}

	DataType(int value) :value((float)value), type(DataType_types::Number) {}
	DataType(float value) :value(value), type(DataType_types::Number) {}
	DataType(std::string str) :str(str), type(DataType_types::String) {}

	DataType(std::string str, PARSED_TYPE type)
	{
		if (type == PARSED_TYPE::NumberLiteral)
		{
			this->value = std::stof(str);
			this->type = DataType_types::Number;
			return;
		}
		this->type = DataType_types::String;
		this->str = str;
	}
	DataType(ParsedNode* node)
	{
		if (node->type == PARSED_TYPE::NumberLiteral)
			this->value = std::stof(node->str);
		else if (node->type == PARSED_TYPE::StringLiteral)
			this->str = node->str;
		else if (node->type == PARSED_TYPE::Var || node->type == PARSED_TYPE::ConstVar)
			this->str = node->str;
		else if (node->type == PARSED_TYPE::Name || node->type == PARSED_TYPE::KeyWord)
			this->str = node->str;
		else if (node->type == PARSED_TYPE::Func || node->type == PARSED_TYPE::FuncCall)
		{
			this->str = node->str;
			this->func = *(FunctionNode*)node;
		}
		else
		{
			this->node.type = node->type;
		}
	}

	~DataType() {}

	union
	{
		float value;
		std::string str = "";

		struct {
			ParsedNode node;
			const DataType* paramA;
			const DataType* paramB;
		};

		struct {
			union { FunctionNode func; RootNode root; };
			std::vector<DataType> body;
		};
	};

	enum class DataType_types
	{
		NONE = 0,
		Number,
		String,
		Var,
		ConstVar,
		Node,
		Function,
		Root
	} type;

};

DataType simplifyAndSolve(ParsedNode& node)
{
	if (node.type == PARSED_TYPE::BinaryExpression)
	{
		DataType* a = &simplifyAndSolve(node.params[0]);
		DataType* b = &simplifyAndSolve(node.params[1]);
		if (a->type == b->type && a->type == DataType::DataType_types::Number)
		{
			if (node.str == "+") return { a->value + b->value };
			if (node.str == "-") return { a->value - b->value };
			if (node.str == "*") return { a->value * b->value };
			if (node.str == "/") return { a->value / b->value };
		}
		return *a;
	}
	if (node.type == PARSED_TYPE::Assignment)
	{
		DataType n(&node);
		n.paramA = &simplifyAndSolve(node.params[0]);
		n.paramB = &simplifyAndSolve(node.params[1]);
		return n;
	}

	if (node.type == PARSED_TYPE::NumberLiteral)
		return { std::stof(node.str) };
	if (node.type == PARSED_TYPE::StringLiteral)
		return { node.str };

	return &node;
}

DataType simplifyAndSolve(FunctionNode& fn)
{
	DataType d(&fn);
	for (ParsedNode& node : fn.body.params)
	{
		d.body.push_back(simplifyAndSolve(node));
	}
	return d;
}

DataType simplifyAndSolve(RootNode& root)
{
	DataType d(root);
	for (FunctionNode& func : root.body)
	{
		d.body.push_back(simplifyAndSolve(func));
	}
	return d;
}
