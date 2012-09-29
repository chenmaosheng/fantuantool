/*****************************************************************************************
	filename:	ft_parser.h
	created:	09/29/2012
	author:		chen
	purpose:	parse ft file

*****************************************************************************************/

#ifndef _H_FT_PARSER
#define _H_FT_PARSER

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

#endif
