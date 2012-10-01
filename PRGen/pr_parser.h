/*****************************************************************************************
	filename:	pr_parser.h
	created:	10/01/2012
	author:		chen
	purpose:	parse pr file

*****************************************************************************************/

#ifndef _H_PR_PARSER
#define _H_PR_PARSER

struct Node
{
	char funcName[32];
	struct Param
	{
		char paramType[32];
		char paramName[32];
	}paramSet[32];
	int paramCount;
};

struct Filter
{
	Filter() { nodeCount = 0; }
	Node node[32];
	int nodeCount;
	char filterName[128];
	char filterId[128];
};

struct PrFile
{
	PrFile() { filterIndex = 0; includeCount = 0; }
	Filter filterSet[32];
	int filterIndex;
	char includeFile[32][128];
	int includeCount;
};

extern PrFile myFile;

extern const char* EscapeSpace(const char* buffer);
extern const char* CheckChar(const char* buffer, char c);
extern const char* CheckIdentifier(const char* buffer, char* value, int size);
extern const char* CheckString(const char* buffer, char* value, int size);
extern const char* CheckKeyword(const char* buffer, const char* identifier, char* value);
extern const char* CheckParamList(const char* buffer, Node* node);
extern const char* CheckInclude(const char* buffer);
extern const char* CheckFunction(const char* buffer);
extern const char* CheckFilter(const char* buffer);
extern bool ParserPr(const char* buf);

#endif
