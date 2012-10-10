#include "pr_generator.h"
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
	fprintf(fp, "#include \"server_common.h\"\n");
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
			fprintf(fp, "    static void %s(PEER_CLIENT pPeerClient", filter->node[j].funcName);
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, ", const prd%s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, ", const %s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
				else
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, ", const prd%s& %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, ", %s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
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
	fprintf(fp, "#include \"peer_dispatch.h\"\n");
	fprintf(fp, "#include \"peer_stream.h\"\n");
	fprintf(fp, "#include <malloc.h>\n\n");
	for (int i = 0; i < myFile.filterIndex; ++i)
	{
		Filter* filter = &myFile.filterSet[i];
		fprintf(fp, "namespace pr_%s_recv\n", filter->filterName);
		fprintf(fp, "{\n\n");
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			fprintf(fp, "bool CALLBACK %s_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)\n", filter->node[j].funcName);
			fprintf(fp, "{\n");
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, "    prd%s* %s;\n", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, "    %s* %s;\n", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
				else
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, "    prd%s %s;\n", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, "    %s %s;\n", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
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
					fprintf(fp, "    if (!stream.Serialize(%s_length, %s)) return false;\n", filter->node[j].paramSet[k].paramName, filter->node[j].paramSet[k].paramName);
				}
				else
				{
					fprintf(fp, "    if (!stream.Serialize(%s)) return false;\n", filter->node[j].paramSet[k].paramName);
				}
			}

			fprintf(fp, "    %sRecv::%s(pPeerClient", filter->filterName, filter->node[j].funcName);
			for(int k = 0; k < filter->node[j].paramCount; ++k)
			{
				fprintf(fp, ", %s", filter->node[j].paramSet[k].paramName);
			}
			fprintf(fp, ");\n");
			fprintf(fp, "    return true;\n}\n\n");
		}

		fprintf(fp, "static PeerClientDispatchFilter::Func func[] = \n{\n");
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			fprintf(fp, "    %s_Callback,\n", filter->node[j].funcName);
		}
		fprintf(fp, "    NULL\n};\n\n");

		fprintf(fp, "struct %sDispatch\n{\n", filter->filterName);
		fprintf(fp, "    %sDispatch()\n", filter->filterName);
		fprintf(fp, "    {\n");
		fprintf(fp, "        PeerClientDispatchFilterArray::GetFilter(%s).m_pFunc = func;\n", filter->filterId);
		fprintf(fp, "        PeerClientDispatchFilterArray::GetFilter(%s).m_iFuncCount = sizeof(func)/sizeof(func[0]);\n", filter->filterId);
		fprintf(fp, "    }\n");
		fprintf(fp, "};\n\n");
		fprintf(fp, "}\n\n");//namespace end
		fprintf(fp, "static pr_%s_recv::%sDispatch _%sDispatch;\n\n", filter->filterName, filter->filterName, filter->filterName);
	}
}

void GenerateSendInclude(const char* name, FILE* fp)
{
	fprintf(fp, "#ifndef _H_%s_SEND\n", name);
	fprintf(fp, "#define _H_%s_SEND\n\n", name);
	fprintf(fp, "#include \"server_common.h\"\n");
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
			fprintf(fp, "    static int32 %s(PEER_SERVER pPeerServer", filter->node[j].funcName);
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, ", const prd%s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, ", const %s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
				else
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, ", const prd%s& %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, ", %s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
			}
			fprintf(fp, ");\n");
		}
		fprintf(fp, "};\n\n");
	}
	fprintf(fp, "#endif");
}

void GenerateSendCpp(const char* name, FILE* fp)
{
	fprintf(fp, "#include \"%s_send.h\"\n", name);
	fprintf(fp, "#include \"peer_dispatch.h\"\n");
	fprintf(fp, "#include \"peer_stream.h\"\n");
	fprintf(fp, "#include <malloc.h>\n\n");
	for (int i = 0; i < myFile.filterIndex; ++i)
	{
		Filter* filter = &myFile.filterSet[i];
		for (int j = 0; j < filter->nodeCount; ++j)
		{
			bool bNeedLength = false;
			fprintf(fp, "int32 %sSend::%s(PEER_SERVER pPeerServer", filter->filterName, filter->node[j].funcName);
			for (int k = 0; k < filter->node[j].paramCount; ++k)
			{
				if (filter->node[j].paramSet[k].paramSize[0] != '\0')
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, ", const prd%s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, ", const %s* %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
				else
				{
					if (IsUserDefinedType(filter->node[j].paramSet[k].paramType))
					{
						fprintf(fp, ", prd%s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
					else
					{
						fprintf(fp, ", %s %s", filter->node[j].paramSet[k].paramType, filter->node[j].paramSet[k].paramName);
					}
				}
			}

			fprintf(fp, ")\n{\n");
			fprintf(fp, "    PeerOutputStream stream;\n");
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
					fprintf(fp, "    if (!stream.Serialize(%s)) return -1;\n", filter->node[j].paramSet[k].paramName);
				}
			}

			fprintf(fp, "    stream.SetId(%s, %d);\n", filter->filterId, j);
			fprintf(fp, "    return stream.Send(pPeerServer);\n");
			fprintf(fp, "}\n\n");
		}
	}
}