#include <cstdio>
#include <fstream>
#include <sstream>
#include "ft_parser.h"

Filter filterSet[32];
int filterIndex = 0;
char includeFile[32][128];
int includeCount = 0;

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

		if (buffer[0] == ' ' || buffer[0] == '\n' || buffer[0] == '\t')
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

const char* CheckIdentifier(const char* buffer, char* value, int size)
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

const char* CheckKeyword(const char* buffer, const char* identifier, char* value)
{
	char str[128] = {0};
	buffer = escapeSpace(buffer);
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

	strcpy(includeFile[includeCount++], file);
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
	strcpy(filterSet[filterIndex].node[filterSet[filterIndex].nodeCount].funcName, name);

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

		if (filterSet[filterIndex].node[filterSet[filterIndex].nodeCount].paramCount > 0)
		{
			buffer = CheckChar(buffer, ',');
			if (!buffer)
			{
				return NULL;
			}
		}

		buffer = CheckParamList(buffer, &filterSet[filterIndex].node[filterSet[filterIndex].nodeCount]);
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

	filterSet[filterIndex].nodeCount++;
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
	strcpy(filterSet[filterIndex].filterName, name);

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
	strcpy(filterSet[filterIndex].filterId, name);
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
	filterIndex++;

	return buffer;
}

bool ParserFt(const char* buf)
{
	const char* buffer = escapeSpace(buf);
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

		buffer = escapeSpace(buffer);
		break;
	}

	return true;
}

void GenerateRecvInclude(const char* name, FILE* fp)
{
	fprintf(fp, "#ifndef _H_%s_RECV\n", name);
	fprintf(fp, "#define _H_%s_RECV\n\n", name);
	fprintf(fp, "#include \"server_common.h\"\n\n");
	for (int i = 0; i < includeCount; ++i)
	{
		fprintf(fp, "#include \"%s\"\n", includeFile[i]);
	}
	fprintf(fp, "\n");
	for (int i = 0; i < filterIndex; ++i)
	{
		Filter* filter = &filterSet[i];
		fprintf(fp, "struct %sRecv\n", filter->filterName);
		fprintf(fp, "{\n");
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			fprintf(fp, "    static void %s(void* pClient", filter->node[j].funcName);
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
				{
					fprintf(fp, ", const char* %s", filter->node[j].paramSet[k].paramName);
				}
				else
				{
					fprintf(fp, ", %s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
				}
			}
			fprintf(fp, ");\n");
		}
		fprintf(fp, "};\n");
	}
	fprintf(fp, "\n#endif");
}

void GenerateRecvCpp(const char* name, FILE* fp)
{
	fprintf(fp, "#include \"%s_recv.h\"\n", name);
	fprintf(fp, "#include \"packet_dispatch.h\"\n");
	fprintf(fp, "#include \"packet.h\"\n");
	fprintf(fp, "#include <malloc.h>\n\n");
	for (int i = 0; i < filterIndex; ++i)
	{
		Filter* filter = &filterSet[i];
		fprintf(fp, "namespace ft_%s_recv\n", filter->filterName);
		fprintf(fp, "{\n");
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			bool bNeedLength = false;
			fprintf(fp, "bool CALLBACK %s_Callback(void* pClient, InputStream& stream)\n", filter->node[j].funcName);
			fprintf(fp, "{\n");
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
				{
					bNeedLength = true;
					fprintf(fp, "    char* %s;\n", filter->node[j].paramSet[k].paramName);
				}
				else
				{
					fprintf(fp, "    %s %s;\n", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
				}

				if (bNeedLength)
				{
					fprintf(fp, "    uint16 iLength;\n\n");
				}
			}

			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
				{
					fprintf(fp, "    stream.Serialize(iLength);\n");
					fprintf(fp, "    %s = (char*)_malloca(iLength + 1);\n", filter->node[j].paramSet[k].paramName);
					fprintf(fp, "    stream.Serialize(iLength, %s);\n", filter->node[j].paramSet[k].paramName);
					fprintf(fp, "    %s[iLength] = '\\0';\n", filter->node[j].paramSet[k].paramName);
				}
				else
				{
					fprintf(fp, "    stream.Serialize(%s);\n", filter->node[j].paramSet[k].paramName);
				}
			}

			fprintf(fp, "    %sRecv::%s(pClient", filter->filterName, filter->node[j].funcName);
			for(int k = 0; k < filter->node[j].paramCount; ++k)
			{
				fprintf(fp, ", %s", filter->node[j].paramSet[k].paramName);
			}
			fprintf(fp, ");\n");
			fprintf(fp, "    return true;\n}\n");
		}

		fprintf(fp, "static DispatchFilter::Func func[] = \n{\n");
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			fprintf(fp, "    %s_Callback,\n", filter->node[j].funcName);
		}
		fprintf(fp, "    NULL\n};\n");

		fprintf(fp, "struct %sPacketDispatch\n{\n", filter->filterName);
		fprintf(fp, "    %sPacketDispatch()\n", filter->filterName);
		fprintf(fp, "    {\n");
		fprintf(fp, "        DispatchFilterArray::GetFilter(%s).m_pFunc = func;\n", filter->filterId);
		fprintf(fp, "        DispatchFilterArray::GetFilter(%s).m_iFuncCount = sizeof(func)/sizeof(func[0]);\n", filter->filterId);
		fprintf(fp, "    }\n");
		fprintf(fp, "}\n\n");
		fprintf(fp, "}\n\n");//namespace end
		fprintf(fp, "static ft_%s_recv::%sPacketDispatch _%sPacketDispatch;\n\n", filter->filterName, filter->filterName, filter->filterName);
	}
}

void GenerateSendInclude(const char* name, FILE* fp)
{
	fprintf(fp, "#ifndef _H_%s_SEND\n", name);
	fprintf(fp, "#define _H_%s_SEND\n\n", name);
	fprintf(fp, "#include \"server_common.h\"\n\n");
	for (int i = 0; i < includeCount; ++i)
	{
		fprintf(fp, "#include \"%s\"\n", includeFile[i]);
	}
	fprintf(fp, "\n");
	for (int i = 0; i < filterIndex; ++i)
	{
		Filter* filter = &filterSet[i];
		fprintf(fp, "struct %sSend\n", filter->filterName);
		fprintf(fp, "{\n");
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			fprintf(fp, "    static int32 %s(void* pServer", filter->node[j].funcName);
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
				{
					fprintf(fp, ", const char* %s", filter->node[j].paramSet[k].paramName);
				}
				else
				{
					fprintf(fp, ", %s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
				}
			}
			fprintf(fp, ");\n");
		}
		fprintf(fp, "};\n");
	}
	fprintf(fp, "\n#endif");
}

void GenerateSendCpp(const char* name, FILE* fp)
{
	fprintf(fp, "#include \"%s_send.h\"\n", name);
	fprintf(fp, "#include \"packet_dispatch.h\"\n");
	fprintf(fp, "#include \"packet.h\"\n");
	fprintf(fp, "#include <malloc.h>\n\n");
	for (int i = 0; i < filterIndex; ++i)
	{
		Filter* filter = &filterSet[i];
		fprintf(fp, "namespace ft_%s_send\n", filter->filterName);
		fprintf(fp, "{\n");
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			bool bNeedLength = false;
			fprintf(fp, "int32 %s(void* pServer", filter->node[j].funcName);
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
				{
					bNeedLength = true;
					fprintf(fp, ", const char* %s", filter->node[j].paramSet[k].paramName);
				}
				else
				{
					fprintf(fp, ", %s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
				}
				fprintf(fp, ")\n{\n");
			}
			fprintf(fp, "    OutputStream stream;\n");
			if (bNeedLength)
			{
				fprintf(fp, "    uint16 iLength;\n");
			}

			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
				{
					fprintf(fp, "    iLength = (uint16)(%s);\n", filter->node[j].paramSet[k].paramName);
					fprintf(fp, "    stream.Serialize(iLength);\n");
					fprintf(fp, "    stream.Serialize(iLength, %s);\n", filter->node[j].paramSet[k].paramName);
				}
				else
				{
					fprintf(fp, "    stream.Serialize(%s);\n", filter->node[j].paramSet[k].paramName);
				}
			}

			fprintf(fp, "    Sender::SendPacket(pServer, (%s<<8) | %d, stream.GetDataLength(), stream.GetBuffer());\n", filter->filterId, j);
			fprintf(fp, "    return 0;\n");
			fprintf(fp, "}\n");
		}
		fprintf(fp, "}\n\n");//namespace end
	}
}

int main(int argc, char* argv[])
{
	char file[] = "login_client.ft";
	std::fstream stream(file);
	std::ostringstream str;
	str << stream.rdbuf();
	std::string buf = str.str();
	const char* buffer = buf.c_str();
	ParserFt(buffer);

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