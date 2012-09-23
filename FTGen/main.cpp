#include <cstdio>
#include <fstream>
#include <sstream>

const char* g_Type[] = {"int32", "int8", "uint8", "cpint8"};
char g_Space[] = {' ', '\t', '\n', '\r'};

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

const char* CheckStr(const char* buffer, const char* strToCheck, const char*& nextToken)
{
	if(strstr(buffer, strToCheck) != buffer)
	{
		nextToken = buffer;
		return NULL;
	}
	nextToken = buffer + strlen(strToCheck);
	return nextToken;
}

const char* CheckType(const char* buffer, const char*& nextToken)
{
	for (size_t i = 0; i < sizeof(g_Type)/sizeof(g_Type[0]); ++i)
	{
		nextToken = CheckStr(buffer, g_Type[i], nextToken);
		if (nextToken)
		{
			return g_Type[i];
		}
	}

	return NULL;
}

const char* escapeSpace(const char* buffer, const char*& nextToken)
{
	bool bHit = false;
	while (true)
	{
		bHit = false;
		for (size_t i = 0; i < sizeof(g_Space)/sizeof(g_Space[0]); ++i)
		{
			if (buffer[0] == g_Space[i])
			{
				buffer+=1;
				bHit = true;
				break;
			}
		}

		if (!bHit || buffer[0] == '\0')
		{
			nextToken = buffer;
			return buffer;
		}
	}

	//assert(false);
	return NULL;
}

const char* escapeSpecial(char specialChar, const char* buffer, const char*& nextToken)
{
	if (buffer[0] == specialChar)
	{
		buffer+=1;
		nextToken = buffer;
		return buffer;
	}

	return NULL;
}

const char* CheckIdentifier(const char* buffer, const char*& nextToken)
{
	const char* identifier = buffer;

	while (true)
	{
		if ((buffer[0] >= 'a' && buffer[0] <= 'z') ||
			(buffer[0] >= 'A' && buffer[0] <= 'Z') ||
			(buffer[0] >= '0' && buffer[0] <= '9'))
		{
			buffer+=1;
		}
		else
		{
			break;
		}
	}

	nextToken = buffer;
	return identifier == buffer ? NULL : identifier;
}

const char* CheckParamList(const char* buffer, const char*& nextToken, Node::Param* pParamSet, int& paramCount)
{
	paramCount = 0;
	do
	{
		const char* type = CheckType(buffer, nextToken);
		if (!type)
		{
			break;
		}

		strcpy_s(pParamSet[paramCount].paramType, sizeof(pParamSet[paramCount].paramType), type);
		pParamSet[paramCount].paramType[strlen(pParamSet[paramCount].paramType)+1] = '\0';

		if (!escapeSpace(nextToken, nextToken))
		{
			break;
		}

		int length = 0;
		const char* identifier = CheckIdentifier(nextToken, nextToken);
		if (!identifier)
		{
			break;
		}

		length = nextToken - identifier;
		strncpy(pParamSet[paramCount].paramName, identifier, length);
		pParamSet[paramCount].paramName[length+1] = '\0';

		escapeSpecial(',', nextToken, nextToken);

		escapeSpace(nextToken, nextToken);

		buffer = nextToken;

		paramCount++;

	}while (buffer[0] != ')');

	return buffer;
}

const char* CheckComment(const char* buffer, const char*& nextToken)
{
	const char* identifier = buffer;

	if (buffer[0] == '/' && buffer[1] == '/')
	{
		buffer+=2;
		while (buffer[0] != '\0' && buffer[0] != '\n')
		{
			buffer++;
		}
	}

	nextToken = buffer;
	return identifier == buffer ? NULL : buffer;
}

const char* CheckInclude(const char* buffer, const char*& nextToken)
{
	const char* identifier = buffer;
	if (buffer[0] == '#')
	{
		buffer++;
		while (buffer[0] != '\0' && buffer[0] != '\n')
		{
			buffer++;
		}
	}

	nextToken = buffer;
	return identifier == buffer ? NULL : identifier;
}

bool ParserFt(const char* buffer, NodeSet* nodeSet)
{
	const char* nextToken = buffer;
	Node* node = nodeSet->node;
	while (nextToken[0] != '\0')
	{
		while (true)
		{
			int length = 0;
			const char* identifier = CheckInclude(nextToken, nextToken);
			if (!identifier)
			{
				break;
			}

			length = nextToken - identifier;
			strncpy(nodeSet->includeFile[nodeSet->includeCount], identifier, length);
			nodeSet->includeFile[nodeSet->includeCount][length+1] = '\0';

			escapeSpace(nextToken, nextToken);
			nodeSet->includeCount++;
		}

		while (true)
		{
			const char* identifier = CheckComment(nextToken, nextToken);
			if (!identifier)
			{
				break;
			}
			
			escapeSpace(nextToken, nextToken);
		}

		const char* type = CheckType(nextToken, nextToken);
		if (type)
		{
			strcpy_s(node[nodeSet->nodeCount].returnType, sizeof(node[nodeSet->nodeCount].returnType), type);
			node[nodeSet->nodeCount].returnType[strlen(node[nodeSet->nodeCount].returnType)+1] = '\0';
		}

		if (!escapeSpace(nextToken, nextToken))
		{
		}

		int length = 0;
		const char* funcName = CheckIdentifier(nextToken, nextToken);
		if (funcName)
		{
			length = nextToken - funcName;
			strncpy(node[nodeSet->nodeCount].funcName, funcName, length);
			node[nodeSet->nodeCount].funcName[length+1] = '\0';
		}

		escapeSpace(nextToken, nextToken);

		if (!escapeSpecial('(', nextToken, nextToken))
		{
		}

		if (!CheckParamList(nextToken, nextToken, node[nodeSet->nodeCount].paramSet, node[nodeSet->nodeCount].paramCount))
		{
		}

		if(!escapeSpecial(')', nextToken, nextToken))
		{
		}

		escapeSpace(nextToken, nextToken);

		if(!escapeSpecial(';', nextToken, nextToken))
		{
		}

		escapeSpace(nextToken, nextToken);

		++nodeSet->nodeCount;
	}

	return true;
}

int main(int argc, char* argv[])
{
	std::fstream stream("test.ft");
	std::ostringstream str;
	str << stream.rdbuf();
	std::string buf = str.str();
	const char* buffer = buf.c_str();
	NodeSet nodeSet;
	ParserFt(buffer, &nodeSet);

	return 0;
}