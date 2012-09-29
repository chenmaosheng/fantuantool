#include <cstdio>
#include <fstream>
#include <sstream>
#include "ft_parser.h"

const char* g_Type[] = {"int32", "int8", "uint8", "cpint8"};
char g_Space[] = {' ', '\t', '\n', '\r'};
NodeSet nodeSet;

const char* escapeSpace(const char* buffer)
{
	for (;; buffer++)
	{
		if (buffer[0] == '/' && buffer[1] == '/')
		{
			buffer+=2;
			while (buffer[0] != '\0' && buffer[0] != '\n')
			{
				buffer++;
			}
		}

		if (buffer[0] == '\0')
		{
			return buffer;
		}

		if (buffer[0] == ' ' || buffer[0] == '\n')
		{
			continue;
		}

		return buffer;
	}
}

const char* CheckChar(const char* buffer, char c)
{
	buffer = escapeSpace(buffer);
	if (buffer[0] != c)
	{
		return NULL;
	}
	return buffer+1;
}

const char* CheckKeyword(const char* buffer, char* value, int size)
{
	int index = 0;
	buffer = escapeSpace(buffer);
	for (;; index++)
	{
		if (buffer[index] >= '0' && buffer[index] <= '9')
		{
			if (index == 0)
			{
				return NULL;
			}
			continue;
		}

		if ((buffer[index] >= 'a' && buffer[index] <= 'z') ||
			(buffer[index] >= 'A' && buffer[index] <= 'Z') || buffer[index] == '_')
		{
			continue;
		}

		break;
	}

	if (index == 0 || index + 1 > size)
	{
		return NULL;
	}

	memcpy(value, buffer, index);
	value[index] = '\0';
	return buffer+index;
}

const char* CheckString(const char* buffer, char* value, int size)
{
	int index = 0;
	buffer = escapeSpace(buffer);
	if (buffer[0] != '"')
	{
		return NULL;
	}
	for (index = 1;; index++)
	{
		if (buffer[index] == '\0')
		{
			return NULL;
		}

		if (buffer[index] == '"')
		{
			break;
		}
	}
	if (index + 1 > size)
	{
		return NULL;
	}

	memcpy(value, buffer+1, index);
	value[index-1] = '\0';
	return buffer+index+1;
}

const char* CheckIdentifier(const char* buffer, const char* identifier, char* value)
{
	char str[128] = {0};
	buffer = escapeSpace(buffer);
	buffer = CheckKeyword(buffer, str, sizeof(str));
	if (!buffer)
	{
		return NULL;
	}

	if (strcmp(str, identifier)!=0)
	{
		return NULL;
	}

	if (value)
	{
		strcpy(value, identifier);
	}
	return buffer;
}

const char* CheckParamList(const char* buffer, Node* node)
{
	char name[128] = {0};
	buffer = CheckKeyword(buffer, name, sizeof(name));
	if (!buffer)
	{
		return NULL;
	}
	
	strcpy(node->paramSet[node->paramCount].paramType, name);
	node->paramSet[node->paramCount].paramType[strlen(node->paramSet[node->paramCount].paramType)+1] = '\0';

	buffer = CheckKeyword(buffer, name, sizeof(name));
	if (!buffer)
	{
		return NULL;
	}

	strcpy(node->paramSet[node->paramCount].paramName, name);
	node->paramSet[node->paramCount].paramName[strlen(node->paramSet[node->paramCount].paramName)+1] = '\0';
	if (!buffer)
	{
		return NULL;
	}

	node->paramCount++;

	return buffer;
}

const char* CheckInclude(const char* buffer)
{
	char file[128] = {0};
	buffer = CheckChar(buffer, '#');
	if (!buffer)
	{
		return NULL;
	}
	buffer = CheckIdentifier(buffer, "include", NULL);
	if (!buffer)
	{
		return NULL;
	}
	buffer = CheckString(buffer, file, sizeof(file));
	if (!buffer)
	{
		return NULL;
	}

	strcpy(nodeSet.includeFile[nodeSet.includeCount++], file);
	return buffer;
}

const char* CheckFunction(const char* buffer)
{
	char name[128] = {0};
	buffer = CheckKeyword(buffer, name, sizeof(name));
	if (!buffer)
	{
		return NULL;
	}
	buffer = CheckChar(buffer, '(');
	if (!buffer)
	{
		return NULL;
	}
	strcpy(nodeSet.node[nodeSet.nodeCount].returnType, name);
	while (true)
	{
		const char* nextToken = CheckChar(buffer, ')');
		if (!nextToken)
		{
			return NULL;
		}

		if (nodeSet.node[nodeSet.nodeCount].paramCount > 0)
		{
			buffer = CheckChar(buffer, ',');
			if (!buffer)
			{
				return NULL;
			}
		}

		buffer = CheckParamList(buffer, &nodeSet.node[nodeSet.nodeCount]);
		if (!buffer)
		{
			return NULL;
		}
	}

	buffer = CheckChar(buffer, ')');
	if (!buffer)
	{
		return NULL;
	}
	buffer = CheckChar(buffer, ';');
	if (!buffer)
	{
		return NULL;
	}

	nodeSet.nodeCount++;
	return buffer;
}

const char* CheckAllFunctions(const char* buffer)
{
	while (true)
	{
		const char* nextToken = CheckFunction(buffer);
		if (!nextToken)
		{
			break;
		}
		buffer = nextToken;
	}

	return buffer;
}

bool ParserFt(const char* buf, NodeSet* nodeSet)
{
	const char* buffer = escapeSpace(buf);
	Node* node = nodeSet->node;
	while (true)
	{
		const char* nextToken;
		nextToken = CheckInclude(buffer);
		if (nextToken)
		{
			buffer = nextToken;
			continue;
		}

		nextToken = CheckAllFunctions(buffer);
		if (nextToken)
		{
			buffer = nextToken;
			continue;
		}

		buffer = escapeSpace(buffer);
		break;
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
	ParserFt(buffer, &nodeSet);

	return 0;
}