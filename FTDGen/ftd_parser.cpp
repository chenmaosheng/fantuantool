#include "ftd_parser.h"
#include <cstdio>
#include <cstring>

FtdFile myFile;

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

const char* CheckParam(const char* buffer, Node* node)
{
	char name[128] = {0};
	const char* temp_buffer;
	buffer = CheckIdentifier(buffer, name, sizeof(name));
	if (!buffer)
	{
		return NULL;
	}
	strcpy(node->paramSet[node->paramCount].paramType, name);

	buffer = CheckIdentifier(buffer, name, sizeof(name));
	if (!buffer)
	{
		return NULL;
	}
	strcpy(node->paramSet[node->paramCount].paramName, name);
	
	temp_buffer = CheckChar(buffer, '[');
	if (temp_buffer)
	{
		temp_buffer = CheckIdentifier(temp_buffer, name, sizeof(name));
		if (!temp_buffer)
		{
			return NULL;
		}
		temp_buffer = CheckChar(temp_buffer, ']');
		if (!temp_buffer)
		{
			return NULL;
		}
		strcpy(node->paramSet[node->paramCount].paramSize, name);
		node->paramSet[node->paramCount].paramSize[strlen(node->paramSet[node->paramCount].paramSize)+1] = '\0';
		buffer = temp_buffer;
	}
	else
	{
		node->paramSet[node->paramCount].paramSize[0] = '\0';
	}

	buffer = CheckChar(buffer, ';');
	if (!buffer)
	{
		return NULL;
	}

	node->paramCount++;
	return buffer;
}

const char* CheckNode(const char* buffer)
{
	char name[128] = {0};
	buffer = CheckKeyword(buffer, "struct", NULL);
	if (!buffer)
	{
		return NULL;
	}
	buffer = CheckIdentifier(buffer, name, sizeof(name));
	if (!buffer)
	{
		return NULL;
	}
	strcpy(myFile.nodeSet[myFile.nodeIndex].structName, name);

	buffer = CheckChar(buffer, '{');
	if (!buffer)
	{
		return NULL;
	}

	while (true)
	{
		const char* nextToken = CheckParam(buffer, &myFile.nodeSet[myFile.nodeIndex]);
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
	buffer = CheckChar(buffer, ';');
	if (!buffer)
	{
		return NULL;
	}
	myFile.nodeIndex++;

	return buffer;
}

bool ParserFtd(const char* buf)
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

		nextToken = CheckNode(buffer);
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