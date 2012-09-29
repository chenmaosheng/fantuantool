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
	char returnType[32];
	char funcName[32];
	struct Param
	{
		char paramType[32];
		char paramName[32];
	}paramSet[32];
	int paramCount;
};

struct NodeSet
{
	NodeSet() { nodeCount = 0; includeCount = 0; }
	Node node[32];
	int nodeCount;
	char includeFile[32][128];
	int includeCount;
};

#endif
