#include "YSS Compiler/Parser.h"

#include "FileReader.h"


int main()
{
	const char* path = "C:\\dev\\Yeastem\\Yeastem\\Yeastem\\YSS Compiler\\src\\Test\\YeastScript.ys";
	auto FileContent = getFileContents(path);
	ParseProgram(Tokenize(FileContent));
	return 0;
}