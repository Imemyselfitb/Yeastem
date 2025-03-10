#pragma once

#include <vector>
#include <cwctype>

#include <iostream>

#include "Macros.h"
#include "Tokens.h"

YEASTEM_COMPILER_BEGIN

namespace Tokenizer
{
	void formatSpaces(const std::string& text, std::string& output);
	void formatStrings(const std::string& line, std::string& output);
	std::string formatStrings(const std::string& line);
	void clean(const std::string& text, std::vector<std::string>& outStrings);

	Token GetToken(char c, char nextChar);
	std::vector<Token> Tokenize(const std::string& FileData);
}

YEASTEM_COMPILER_END
