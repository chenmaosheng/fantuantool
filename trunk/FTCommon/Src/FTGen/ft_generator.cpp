#include "ft_generator.h"
#include <cstring>

bool IsUserDefinedType(const char* name)
{
	if (strcmp(name, "int8") == 0 ||
		strcmp(name, "uint8") == 0 ||
		strcmp(name, "int16") == 0 ||
		strcmp(name, "uint16") == 0 ||
		strcmp(name, "int32") == 0 ||
		strcmp(name, "uint32") == 0 ||
		strcmp(name, "int64") == 0 ||
		strcmp(name, "uint64") == 0 ||
		strcmp(name, "float32") == 0 ||
		strcmp(name, "float64") == 0)
	{
		return false;
	}

	return true;
}

void GenerateRecvInclude(const char* name, FILE* fp)
{
	fprintf(fp, "#ifndef _H_%s_RECV\n", name);
	fprintf(fp, "#define _H_%s_RECV\n\n", name);
	fprintf(fp, "#include \"common.h\"\n");
	for (int i = 0; i < myFile.includeCount; ++i)
	{
		fprintf(fp, "#include \"%s\"\n", myFile.includeFile[i]);
	}
	fprintf(fp, "\n");
	for (int i = 0; i < myFile.filterIndex; ++i)
	{
		Filter* filter = &myFile.filterSet[i];
		fprintf(fp, "struct %sRecv\n", filter->filterName);
		fprintf(fp, "{\n");
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			fprintf(fp, "    static void %s(void* pClient", filter->node[j].funcName);
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, ", const ftd%s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, ", const %s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
				else
				{
					if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
					{
						fprintf(fp, ", const char* %s", filter->node[j].paramSet[k].paramName);
					}
					else
					{
						if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
						{
							fprintf(fp, ", const ftd%s& %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
						}
						else
						{
							fprintf(fp, ", %s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
						}
					}
				}
			}
			fprintf(fp, ");\n");
		}
		fprintf(fp, "};\n\n");
	}
	fprintf(fp, "#endif");
}

void GenerateRecvCpp(const char* name, FILE* fp)
{
	fprintf(fp, "#include \"%s_recv.h\"\n", name);
	fprintf(fp, "#include \"packet_dispatch.h\"\n");
	fprintf(fp, "#include \"packet.h\"\n");
	fprintf(fp, "#include <malloc.h>\n\n");
	for (int i = 0; i < myFile.filterIndex; ++i)
	{
		Filter* filter = &myFile.filterSet[i];
		fprintf(fp, "namespace ft_%s_recv\n", filter->filterName);
		fprintf(fp, "{\n\n");
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			bool bNeedLength = false;
			fprintf(fp, "bool CALLBACK %s_Callback(void* pClient, InputStream& stream)\n", filter->node[j].funcName);
			fprintf(fp, "{\n");
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, "    ftd%s* %s;\n", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, "    %s* %s;\n", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
				else
				{
					if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
					{
						bNeedLength = true;
						fprintf(fp, "    char* %s;\n", filter->node[j].paramSet[k].paramName);
					}
					else
					{
						if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
						{
							fprintf(fp, "    ftd%s %s;\n", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
						}
						else
						{
							fprintf(fp, "    %s %s;\n", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
						}
						
					}
				}
			}

			if (bNeedLength)
			{
				fprintf(fp, "    uint16 iLength;\n\n");
			}

			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					fprintf(fp, "    uint16 %s_length;\n", filter->node[j].paramSet[k].paramName);
				}
			}

			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					fprintf(fp, "    %s_length = %s;\n", filter->node[j].paramSet[k].paramName, filter->node[j].paramSet[k].paramSize);
					fprintf(fp, "    if (!stream.Serialize(%s_length)) return false;\n", filter->node[j].paramSet[k].paramName);
					fprintf(fp, "    %s = (%s*)_malloca(sizeof(%s)*%s_length);\n", filter->node[j].paramSet[k].paramName, filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					fprintf(fp, "    if (!stream.Serialize(%s_length, %s)) return false;\n", filter->node[j].paramSet[k].paramName, filter->node[j].paramSet[k].paramName);
				}
				else
				{
					if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
					{
						fprintf(fp, "    if (!stream.Serialize(iLength)) return false;\n");
						fprintf(fp, "    %s = (char*)_malloca(iLength + 1);\n", filter->node[j].paramSet[k].paramName);
						fprintf(fp, "    if (!stream.Serialize(iLength, %s)) return false;\n", filter->node[j].paramSet[k].paramName);
						fprintf(fp, "    %s[iLength] = '\\0';\n", filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, "    if (!stream.Serialize(%s)) return false;\n", filter->node[j].paramSet[k].paramName);
					}
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
		fprintf(fp, "    NULL\n};\n\n");

		fprintf(fp, "struct %sPacketDispatch\n{\n", filter->filterName);
		fprintf(fp, "    %sPacketDispatch()\n", filter->filterName);
		fprintf(fp, "    {\n");
		fprintf(fp, "        DispatchFilterArray::GetFilter(%s).m_pFunc = func;\n", filter->filterId);
		fprintf(fp, "        DispatchFilterArray::GetFilter(%s).m_iFuncCount = sizeof(func)/sizeof(func[0]);\n", filter->filterId);
		fprintf(fp, "    }\n");
		fprintf(fp, "};\n\n");
		fprintf(fp, "}\n\n");//namespace end
		fprintf(fp, "static ft_%s_recv::%sPacketDispatch _%sPacketDispatch;\n\n", filter->filterName, filter->filterName, filter->filterName);
	}
}

void GenerateSendInclude(const char* name, FILE* fp)
{
	fprintf(fp, "#ifndef _H_%s_SEND\n", name);
	fprintf(fp, "#define _H_%s_SEND\n\n", name);
	fprintf(fp, "#include \"common.h\"\n\n");
	for (int i = 0; i < myFile.includeCount; ++i)
	{
		fprintf(fp, "#include \"%s\"\n", myFile.includeFile[i]);
	}
	fprintf(fp, "\n");
	for (int i = 0; i < myFile.filterIndex; ++i)
	{
		Filter* filter = &myFile.filterSet[i];
		fprintf(fp, "struct %sSend\n", filter->filterName);
		fprintf(fp, "{\n");
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			fprintf(fp, "    static int32 %s(void* pServer", filter->node[j].funcName);
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, ", const ftd%s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, ", const %s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
				else
				{
					if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
					{
						fprintf(fp, ", const char* %s", filter->node[j].paramSet[k].paramName);
					}
					else
					{
						if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
						{
							fprintf(fp, ", const ftd%s& %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
						}
						else
						{
							fprintf(fp, ", %s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
						}
					}
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
	for (int i = 0; i < myFile.filterIndex; ++i)
	{
		Filter* filter = &myFile.filterSet[i];
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			bool bNeedLength = false;
			fprintf(fp, "int32 %sSend::%s(void* pServer", filter->filterName, filter->node[j].funcName);
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, ", const ftd%s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, ", const %s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
				else
				{
					if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
					{
						bNeedLength = true;
						fprintf(fp, ", const char* %s", filter->node[j].paramSet[k].paramName);
					}
					else
					{
						if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
						{
							fprintf(fp, ", ftd%s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
						}
						else
						{
							fprintf(fp, ", %s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
						}
					}
				}
			}
			fprintf(fp, ")\n{\n");
			fprintf(fp, "    OutputStream stream;\n");
			if (bNeedLength)
			{
				fprintf(fp, "    uint16 iLength;\n");
			}

			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					fprintf(fp, "    uint16 %s_length;\n", filter->node[j].paramSet[k].paramName);
				}
			}

			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					fprintf(fp, "    %s_length = %s;\n", filter->node[j].paramSet[k].paramName, filter->node[j].paramSet[k].paramSize);
					fprintf(fp, "    if (!stream.Serialize(%s_length)) return -1;\n", filter->node[j].paramSet[k].paramName);
					fprintf(fp, "    if (!stream.Serialize(%s_length, %s)) return -1;\n", filter->node[j].paramSet[k].paramName, filter->node[j].paramSet[k].paramName, filter->node[j].paramSet[k].paramName);
				}
				else
				{
					if (strcmp(filter->node[j].paramSet[k].paramType, "string") == 0)
					{
						fprintf(fp, "    iLength = (uint16)strlen(%s);\n", filter->node[j].paramSet[k].paramName);
						fprintf(fp, "    if (!stream.Serialize(iLength)) return -1;\n");
						fprintf(fp, "    if (!stream.Serialize(iLength, %s)) return -1;\n", filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, "    if (!stream.Serialize(%s)) return -1;\n", filter->node[j].paramSet[k].paramName);
					}
				}
			}

			fprintf(fp, "    Sender::SendPacket(pServer, (%s<<8) | %d, stream.GetDataLength(), stream.GetBuffer());\n", filter->filterId, j);
			fprintf(fp, "    return 0;\n");
			fprintf(fp, "}\n");
		}
	}
}