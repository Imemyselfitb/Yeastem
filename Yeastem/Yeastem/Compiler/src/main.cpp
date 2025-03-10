#include "YSS Compiler/Parser.h"

#include "FileReader.h"

YEASTEM_COMPILER_BEGIN

static const char* SampleText = " \
main[numArgs, args]: \
	example_func(); \
	funcAdd(1, 3 - 4); \
";

int EntryPoint()
{
	const char* path = "src\\Test\\YeastScript.ys";
	auto FileContent = getFileContents(path);
	ParseProgram(Tokenizer::Tokenize(FileContent));
	return 0;
}

YEASTEM_COMPILER_END

int main()
{
	Yeastem::Compiler::EntryPoint();
}
