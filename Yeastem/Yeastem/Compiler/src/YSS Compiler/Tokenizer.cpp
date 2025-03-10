#include "Macros.h"
#include "Tokenizer.h"

YEASTEM_COMPILER_BEGIN namespace Tokenizer {

	Token GetToken(char c, char nextChar)
	{
		std::string str;
		str = c + nextChar;

		if (str == "==") return { "==", TOKEN_TYPE::EQ_EQUAL };
		if (str == "!=") return { "!=", TOKEN_TYPE::NOT_EQUAL };
		if (str == "<=") return { "<=", TOKEN_TYPE::LESS_EQUAL };
		if (str == ">=") return { ">=", TOKEN_TYPE::GREATER_EQUAL };

		switch (c)
		{
		case '<': return { "<", TOKEN_TYPE::LESS };
		case '>': return { ">", TOKEN_TYPE::GREATER };
		case '(': return { "(", TOKEN_TYPE::LPAR };
		case '[': return { "[", TOKEN_TYPE::LSQB };
		case '{': return { "{", TOKEN_TYPE::LBRACE };
		case ')': return { ")", TOKEN_TYPE::RPAR };
		case ']': return { "]", TOKEN_TYPE::RSQB };
		case '}': return { "}", TOKEN_TYPE::RBRACE };

		case '=': return { "=", TOKEN_TYPE::EQUAL };
		case '+': return { "+", TOKEN_TYPE::PLUS };
		case '-': return { "-", TOKEN_TYPE::MINUS };
		case '*': return { "*", TOKEN_TYPE::STAR };
		case '/': return { "/", TOKEN_TYPE::SLASH };
		case '\\': return { "\\", TOKEN_TYPE::BSLASH };

		case ':': return { ":", TOKEN_TYPE::COLON };
		case ';': return { ";", TOKEN_TYPE::SEMIC };
		case ',': return { ",", TOKEN_TYPE::COMMA };
		case '.': return { ".", TOKEN_TYPE::DOT };
		case '~': return { "~", TOKEN_TYPE::TILDE };

		case '&': return { "&", TOKEN_TYPE::AMPER };
		case '|': return { "|", TOKEN_TYPE::VBAR };
		case '^': return { "^", TOKEN_TYPE::CARROT };

		case '\"': return TOKEN_TYPE::STRING;
		case '_': return { "_", TOKEN_TYPE::NAME };
		}

		if (isdigit(c)) return { &c, TOKEN_TYPE::NUMBER };
		if (iswalnum(c)) return { &c, TOKEN_TYPE::NAME };

		return TOKEN_TYPE::NONE;
	}

	void formatSpaces(const std::string& text, std::string& output)
	{
		if (text.length() < 1) return;

		if (text[0] != ' ') output += text[0];
		if (text.length() > 1)
		{
			for (int i = 1; i < int(text.size() - 1); i++)
			{
				if (text[i] != ' ')
				{
					output += text[i];
					continue;
				}

				if (iswalnum((int)text[i - 1]) != 0 && iswalnum((int)text[i + 1]) != 0)
				{
					if (text[i - 1] != ' ' && text[i + 1] != ' ') output += " ";
				}
				else if (text[i + 1] == '_') output += " ";
			}
			if (text[text.length() - 1] != ' ') output += text[text.length() - 1];
		}
	}

	void formatStrings(const std::string& line, std::string& output)
	{
		bool insideString = false;

#define StringHasChar(text, Char) (text.find(Char) <= text.size())
		if (StringHasChar(line, '\"') || StringHasChar(line, '\''))
		{
			// STRING Holds the Current Value
			std::string STRING;
			for (char CHAR : line)
			{
				if (CHAR == '\"' || CHAR == '\'')
				{
					if (insideString)
						output += STRING;
					else
						formatSpaces(STRING, output);

					STRING = "";

					output += '\"';
					insideString = !insideString;

					continue;
				}

				STRING += CHAR;
			}
			formatSpaces(STRING, output);
		}
		else
			formatSpaces(line, output);
	}

	std::string formatStrings(const std::string& line)
	{
		std::string str;
		formatStrings(line, str);
		return str;
	}

	void clean(const std::string& text, std::vector<std::string>& outStrings)
	{
		std::string line = "";
		for (char CHAR : text)
		{
			if (CHAR != *";")// && CHAR != *"\n")
			{
				line += CHAR;
				continue;
			}
			outStrings.push_back(formatStrings(line));
			line = "";
		}
		outStrings.push_back(formatStrings(line));
	}

	std::vector<Token> Tokenize(const std::string& FileData)
	{
		std::vector<std::string> rows;
		clean(FileData, rows);

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

			if (lineNum < int(rows.size() - 1)) TokenStream.push_back(TOKEN_TYPE::NEW_LINE);
			lineNum++;
		}

		TokenStream.push_back(TOKEN_TYPE::FILE_END);

		return TokenStream;
	}

} YEASTEM_COMPILER_END
