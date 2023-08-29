#include "YSS Compiler/Parser.h"

#include "FileReader.h"

int main()
{
	auto FileContent = getFileContents("C:\\dev\\Yeastem\\Yeastem\\Yeastem\\Yeastem\\Test\\YeastScript.ys");
	ParseProgram(Tokenize(FileContent));
}