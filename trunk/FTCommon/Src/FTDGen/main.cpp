#include <fstream>
#include <sstream>
#include "ftd_generator.h"

int main(int argc, char* argv[])
{
	char* file = argv[1];
	std::fstream stream(file);
	std::ostringstream str;
	str << stream.rdbuf();
	std::string buf = str.str();
	const char* buffer = buf.c_str();
	ParserFtd(buffer);

	char filename[128] = {0};
	const char* delim = ".";
	char* fileprefix = strtok(file, delim);
	_snprintf(filename, 128, "%s_define.h", fileprefix);
	FILE* fp = fopen(filename, "wt");
	GenerateFtdDefine(fileprefix, fp);

	_snprintf(filename, 128, "%s_impl.cpp", fileprefix);
	FILE* fp2 = fopen(filename, "wt");
	GenerateFtdImpl(fileprefix, fp2);

	return 0;
}