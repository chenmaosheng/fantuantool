/*****************************************************************************************
filename:	ftd_parser.h
created:	10/02/2012
author:		chen
purpose:	parse ftd file

*****************************************************************************************/

#ifndef _H_FTD_PARSER
#define _H_FTD_PARSER

struct Node
{
	Node() { paramCount = 0; }
	char structName[32];
	struct Param
	{
		char paramType[32];
		char paramName[32];
		char paramSize[32];
	}paramSet[32];
	int paramCount;
};

struct FtdFile
{
	FtdFile() { nodeIndex = 0; includeCount = 0; }
	Node nodeSet[32];
	int nodeIndex;
	char includeFile[32][128];
	int includeCount;
};

extern FtdFile myFile;

extern const char* EscapeSpace(const char* buffer);
extern const char* CheckChar(const char* buffer, char c);
extern const char* CheckIdentifier(const char* buffer, char* value, int size);
extern const char* CheckString(const char* buffer, char* value, int size);
extern const char* CheckKeyword(const char* buffer, const char* identifier, char* value);
extern const char* CheckParam(const char* buffer, Node* node);
extern const char* CheckInclude(const char* buffer);
extern const char* CheckNode(const char* buffer);
extern bool ParserFtd(const char* buf);

#endif
