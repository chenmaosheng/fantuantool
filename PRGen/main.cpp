#include <fstream>
#include <sstream>
#include "pr_generator.h"

int main(int argc, char* argv[])
{
	char* file = argv[1];
	std::fstream stream(file);
	std::ostringstream str;
	str << stream.rdbuf();
	std::string buf = str.str();
	const char* buffer = buf.c_str();
	ParserPr(buffer);

	char filename[128] = {0};
	const char* delim = ".";
	char* fileprefix = strtok(file, delim);
	_snprintf(filename, 128, "%s_recv.h", fileprefix);
	FILE* fp = fopen(filename, "wt");
	GenerateRecvInclude(fileprefix, fp);

	_snprintf(filename, 128, "%s_recv.cpp", fileprefix);
	FILE* fp2 = fopen(filename, "wt");
	GenerateRecvCpp(fileprefix, fp2);

	_snprintf(filename, 128, "%s_send.h", fileprefix);
	FILE* fp3 = fopen(filename, "wt");
	GenerateSendInclude(fileprefix, fp3);

	_snprintf(filename, 128, "%s_send.cpp", fileprefix);
	FILE* fp4 = fopen(filename, "wt");
	GenerateSendCpp(fileprefix, fp4);

	return 0;
}