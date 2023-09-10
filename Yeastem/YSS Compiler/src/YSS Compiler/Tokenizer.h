#pragma once

#include <vector>
#include <cwctype>

#include <iostream>

#include "Tokens.h"

std::string formatSpaces(const std::string &text)
{
	if (text.length() < 1) return "";

	std::string output = "";

	if (text[0] != *" ") output = text[0];
	if (text.length() > 1)
	{
		for (int i = 1; i < int(text.size() - 1); i++)
		{
			if (text[i] != *" ")
			{
				output += text[i];
				continue;
			}

			if (iswalnum((int)text[i - 1]) != 0 && iswalnum((int)text[i + 1]) != 0)
			{
				if (text[i - 1] != *" " && text[i + 1] != *" ") output += " ";
			}
			else if (text[i + 1] == *"_") output += " ";
		}
		if (text[text.length() - 1] != *" ") output += text[text.length() - 1];
	}

	return output;
}

std::string formatStrings(const std::string& line)
{
	bool inString = false;

#define StringHasChar(text, Char) (text.find(Char) <= text.size())
	if (StringHasChar(line, "\"") || StringHasChar(line, "\'"))
	{
		// Output text
		std::string output;

		// STRING Holds the Current Value
		std::string STRING;
		for (char CHAR : line)
		{
			if (CHAR == *"\"" || CHAR == *"\'")
			{
				if (inString)	output += STRING;
				else			output += formatSpaces(STRING);
				STRING = "";

				output += "\"";
				inString = !inString;

				continue;
			}

			STRING += CHAR;
		}
		output += formatSpaces(STRING);
		return output;
	}
	else
	{
		return formatSpaces(line);
	}
}

std::vector<std::string> clean(const std::string& text)
{
	std::vector<std::string> strings;

	std::string line = "";
	for (char CHAR : text)
	{
		if (CHAR != *";")// && CHAR != *"\n")
		{
			line += CHAR; 
			continue;
		}
		strings.push_back(formatStrings(line));
		line = "";
	}
	strings.push_back(formatStrings(line));
	return strings;
}

Token GetToken(char c, char nextChar)
{
	std::string str;
	str = c + nextChar;

	if (str == "==") return { "==", TOKEN_TYPE::EQ_EQUAL };
	if (str == "!=") return { "!=", TOKEN_TYPE::NOT_EQUAL };
	if (str == "<=") return { "<=", TOKEN_TYPE::LESS_EQUAL };
	if (str == ">=") return { ">=", TOKEN_TYPE::GREATER_EQUAL };

	if (c == *"<") return { "<", TOKEN_TYPE::LESS };
	if (c == *">") return { ">", TOKEN_TYPE::GREATER };

	if (c == *"(") return { "(", TOKEN_TYPE::LPAR };
	if (c == *"[") return { "[", TOKEN_TYPE::LSQB };
	if (c == *"{") return { "{", TOKEN_TYPE::LBRACE };
	if (c == *")") return { ")", TOKEN_TYPE::RPAR };
	if (c == *"]") return { "]", TOKEN_TYPE::RSQB };
	if (c == *"}") return { "}", TOKEN_TYPE::RBRACE };

	if (c == *"=") return { "=", TOKEN_TYPE::EQUAL };
	if (c == *"+") return { "+", TOKEN_TYPE::PLUS };
	if (c == *"-") return { "-", TOKEN_TYPE::MINUS };
	if (c == *"*") return { "*", TOKEN_TYPE::STAR };
	if (c == *"/") return { "/", TOKEN_TYPE::SLASH };
	if (c == *"\\") return { "\\", TOKEN_TYPE::BSLASH };

	if (c == *":") return { ":", TOKEN_TYPE::COLON };
	if (c == *";") return { ";", TOKEN_TYPE::SEMIC };
	if (c == *",") return { ",", TOKEN_TYPE::COMMA };
	if (c == *".") return { ".", TOKEN_TYPE::DOT };
	if (c == *"~") return { "~", TOKEN_TYPE::TILDE };

	if (c == *"&") return { "&", TOKEN_TYPE::AMPER };
	if (c == *"|") return { "|", TOKEN_TYPE::VBAR };
	if (c == *"^") return { "^", TOKEN_TYPE::CARROT };

	if (c == *"\"") return TOKEN_TYPE::STRING;
	if (isdigit(c)) return { &c, TOKEN_TYPE::NUMBER };
	if (iswalnum(c)) return { &c, TOKEN_TYPE::NAME };
	if (c == *"_") return { "_", TOKEN_TYPE::NAME };

	return TOKEN_TYPE::NONE;
}

std::vector<Token> Tokenize(const std::string& FileData)
{
	std::vector<std::string> rows = clean(FileData);

	std::vector<Token> TokenStream;

	// Used to copy values for data types: STRING, NUMBER
	enum class _copyType
	{
		False = 0, 
		Number,
		String,
		Name
	};
	_copyType _copyMode = _copyType::False;

	int lineNum = 0;
	for (std::string& line : rows)
	{
		for (int i = 0; i < (int)line.length(); i++)
		{
			if (_copyMode != _copyType::False)
			{
				switch (_copyMode)
				{
				case _copyType::Number:
					if (!isdigit(line[i]) && line[i] != *".") 
						_copyMode = _copyType::False;
					break;
				case _copyType::Name:
					if (!iswalnum(line[i]) && line[i] != *"_") 
						_copyMode = _copyType::False;
					break;
				case _copyType::String:
					if (line[i] == *"\"") {
						_copyMode = _copyType::False;
						continue;
					}
					break;
				}
				
				if (_copyMode != _copyType::False)
				{
					TokenStream[TokenStream.size() - 1].value += line[i];
					continue;
				}
			}

			Token tk = GetToken(line[i], line[i + 1]);
			if (tk.type != TOKEN_TYPE::NONE)
			{
				if (tk.type == TOKEN_TYPE::NUMBER) _copyMode = _copyType::Number;
				if (tk.type == TOKEN_TYPE::STRING) _copyMode = _copyType::String;
				if (tk.type == TOKEN_TYPE::NAME) _copyMode = _copyType::Name;
				TokenStream.push_back(tk);
			}
		}

		if(lineNum < int(rows.size() - 1)) TokenStream.push_back(TOKEN_TYPE::NEW_LINE);
		lineNum++;
	}

	TokenStream.push_back(TOKEN_TYPE::FILE_END);

	/*
	for (Token& tk : TokenStream)
	{
		std::cout << int(tk.type) << ": " << tk.value.c_str() << std::endl;
	}
	*/
	return TokenStream;
}