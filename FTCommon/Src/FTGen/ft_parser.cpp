#include "ft_parser.h"
#include <cstdio>
#include <cstring>

FtFile myFile;

const char* EscapeSpace(const char* buffer)
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

		if (buffer[0] == ' ' || buffer[0] == '\n' || buffer[0] == '\t')
		{
			continue;
		}

		return buffer;
	}
}

const char* CheckChar(const char* buffer, char c)
{
	buffer = EscapeSpace(buffer);
	if (buffer[0] != c)
	{
		return NULL;
	}
	return buffer+1;
}

const char* CheckIdentifier(const char* buffer, char* value, int size)
{
	int index = 0;
	buffer = EscapeSpace(buffer);
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
	buffer = EscapeSpace(buffer);
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

const char* CheckKeyword(const char* buffer, const char* identifier, char* value)
{
	char str[128] = {0};
	buffer = EscapeSpace(buffer);
	buffer = CheckIdentifier(buffer, str, sizeof(str));
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
	buffer = CheckIdentifier(buffer, name, sizeof(name));
	if (!buffer)
	{
		return NULL;
	}

	strcpy(node->paramSet[node->paramCount].paramType, name);
	node->paramSet[node->paramCount].paramType[strlen(node->paramSet[node->paramCount].paramType)+1] = '\0';

	buffer = CheckIdentifier(buffer, name, sizeof(name));
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
	buffer = CheckKeyword(buffer, "include", NULL);
	if (!buffer)
	{
		return NULL;
	}
	buffer = CheckString(buffer, file, sizeof(file));
	if (!buffer)
	{
		return NULL;
	}

	strcpy(myFile.includeFile[myFile.includeCount++], file);
	return buffer;
}

const char* CheckFunction(const char* buffer)
{
	char name[128] = {0};
	buffer = CheckIdentifier(buffer, name, sizeof(name));
	if (!buffer)
	{
		return NULL;
	}
	strcpy(myFile.filterSet[myFile.filterIndex].node[myFile.filterSet[myFile.filterIndex].nodeCount].funcName, name);

	buffer = CheckChar(buffer, '(');
	if (!buffer)
	{
		return NULL;
	}
	while (true)
	{
		const char* nextToken = CheckChar(buffer, ')');
		if (nextToken)
		{
			break;
		}

		if (myFile.filterSet[myFile.filterIndex].node[myFile.filterSet[myFile.filterIndex].nodeCount].paramCount > 0)
		{
			buffer = CheckChar(buffer, ',');
			if (!buffer)
			{
				return NULL;
			}
		}

		buffer = CheckParamList(buffer, &myFile.filterSet[myFile.filterIndex].node[myFile.filterSet[myFile.filterIndex].nodeCount]);
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

	myFile.filterSet[myFile.filterIndex].nodeCount++;
	return buffer;
}

const char* CheckFilter(const char* buffer)
{
	char name[128] = {0};
	buffer = CheckKeyword(buffer, "filter", NULL);
	if (!buffer)
	{
		return NULL;
	}
	buffer = CheckChar(buffer, '(');
	if (!buffer)
	{
		return NULL;
	}
	buffer = CheckIdentifier(buffer, name, sizeof(name));
	if (!buffer)
	{
		return NULL;
	}
	strcpy(myFile.filterSet[myFile.filterIndex].filterName, name);

	buffer = CheckChar(buffer, ',');
	if (!buffer)
	{
		return NULL;
	}

	buffer = CheckIdentifier(buffer, name, sizeof(name));
	if (!buffer)
	{
		return NULL;
	}
	strcpy(myFile.filterSet[myFile.filterIndex].filterId, name);
	buffer = CheckChar(buffer, ')');
	if (!buffer)
	{
		return NULL;
	}

	buffer = CheckChar(buffer, '{');
	if (!buffer)
	{
		return NULL;
	}

	while (true)
	{
		const char* nextToken = CheckFunction(buffer);
		if (!nextToken)
		{
			break;
		}
		buffer = nextToken;
	}

	buffer = CheckChar(buffer, '}');
	if (!buffer)
	{
		return NULL;
	}
	myFile.filterIndex++;

	return buffer;
}

bool ParserFt(const char* buf)
{
	const char* buffer = EscapeSpace(buf);
	while (true)
	{
		const char* nextToken;
		nextToken = CheckInclude(buffer);
		if (nextToken)
		{
			buffer = nextToken;
			continue;
		}

		nextToken = CheckFilter(buffer);
		if (nextToken)
		{
			buffer = nextToken;
			continue;
		}

		buffer = EscapeSpace(buffer);
		break;
	}

	return true;
}