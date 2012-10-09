#include "ftd_generator.h"
#include <cstring>

void GenerateFtdDefine(const char* name, FILE* fp)
{
	fprintf(fp, "#ifndef _H_%s_DEFINE\n", name);
	fprintf(fp, "#define _H_%s_DEFINE\n\n", name);
	fprintf(fp, "#include \"common.h\"\n");
	for (int i = 0; i < myFile.includeCount; ++i)
	{
		fprintf(fp, "#include \"%s\"\n", myFile.includeFile[i]);
	}
	fprintf(fp, "\n");
	for (int i = 0; i < myFile.nodeIndex; ++i)
	{
		Node* node = &myFile.nodeSet[i];
		bool bContainString = false;
		for (int j = 0; j < node->paramCount; ++j)
		{
			if (strcmp(node->paramSet[j].paramType, "char") == 0)
			{
				bContainString = true;
				break;
			}
		}

		fprintf(fp, "struct ftd%s\n", node->structName);
		fprintf(fp, "{\n");
		for (int j = 0; j < node->paramCount; ++j)
		{
			if (node->paramSet[j].paramSize[0] != '\0')
			{
				fprintf(fp, "    %s %s[%s+1];\n", node->paramSet[j].paramType, node->paramSet[j].paramName, node->paramSet[j].paramSize);
			}
			else
			{
				fprintf(fp, "    %s %s;\n", node->paramSet[j].paramType, node->paramSet[j].paramName);
			}
		}
		fprintf(fp, "};\n\n");

		//if (bContainString)
		{
			fprintf(fp, "struct prd%s\n", node->structName);
			fprintf(fp, "{\n");
			for (int j = 0; j < node->paramCount; ++j)
			{
				if (node->paramSet[j].paramSize[0] != '\0')
				{
					if (strcmp(node->paramSet[j].paramType, "char") == 0)
					{
						fprintf(fp, "    TCHAR %s[%s+1];\n", node->paramSet[j].paramName, node->paramSet[j].paramSize);
					}
					else
					{
						fprintf(fp, "    %s %s[%s];\n", node->paramSet[j].paramType, node->paramSet[j].paramName, node->paramSet[j].paramSize);
					}
				}
				else
				{
					fprintf(fp, "    %s %s;\n", node->paramSet[j].paramType, node->paramSet[j].paramName);
				}
			}
			fprintf(fp, "};\n\n");
		}

		fprintf(fp, "extern int32 prd%s2ftd%s(const prd%s* pPrd, ftd%s* pFtd);\n", node->structName, node->structName, node->structName, node->structName);
		fprintf(fp, "extern int32 ftd%s2prd%s(const ftd%s* pFtd, prd%s* pPrd);\n", node->structName, node->structName, node->structName, node->structName);
	}
	fprintf(fp, "\n#endif");
}

void GenerateFtdImpl(const char* name, FILE* fp)
{
	fprintf(fp, "#include \"%s_define.h\"\n\n", name);

	for (int i = 0; i < myFile.nodeIndex; ++i)
	{
		Node* node = &myFile.nodeSet[i];
		fprintf(fp, "int32 prd%s2ftd%s(const prd%s* pPrd, ftd%s* pFtd)\n", node->structName, node->structName, node->structName, node->structName);
		fprintf(fp, "{\n");
		fprintf(fp, "    int32 iRet = 0;\n");
		for (int j = 0; j < node->paramCount; ++j)
		{
			if (node->paramSet[j].paramSize[0] != '\0')
			{
				if (strcmp(node->paramSet[j].paramType, "char") == 0)
				{
					fprintf(fp, "    iRet = WChar2Char(pPrd->%s, %s+1, pFtd->%s, %s+1);\n", node->paramSet[j].paramName, node->paramSet[j].paramSize, node->paramSet[j].paramName, node->paramSet[j].paramSize);
					fprintf(fp, "    if (iRet == 0)\n");
					fprintf(fp, "    {\n");
					fprintf(fp, "        return -1;\n");
					fprintf(fp, "    }\n");
					fprintf(fp, "    pFtd->%s[iRet] = \'\\0\';\n", node->paramSet[j].paramName);
				}
				else
				{
					fprintf(fp, "    iRet = prd%s2ftd%s(&pPrd->%s, &pFtd->%s);\n", node->paramSet[j].paramName, node->paramSet[j].paramName);
					fprintf(fp, "    if (iRet == 0)");
					fprintf(fp, "    {\n");
					fprintf(fp, "        return -1;\n");
					fprintf(fp, "    }\n");
				}
			}
			else
			{
				fprintf(fp, "    pFtd->%s = pPrd->%s;\n", node->paramSet[j].paramName, node->paramSet[j].paramName);
			}
		}
		fprintf(fp, "    return 0;\n");
		fprintf(fp, "}\n\n");

		fprintf(fp, "extern int32 ftd%s2prd%s(const ftd%s* pFtd, prd%s* pPrd)\n", node->structName, node->structName, node->structName, node->structName);
		fprintf(fp, "{\n");
		fprintf(fp, "    int32 iRet = 0;\n");
		for (int j = 0; j < node->paramCount; ++j)
		{
			if (node->paramSet[j].paramSize[0] != '\0')
			{
				if (strcmp(node->paramSet[j].paramType, "char") == 0)
				{
					fprintf(fp, "    iRet = Char2WChar(pFtd->%s, %s+1, pPrd->%s, %s+1);\n", node->paramSet[j].paramName, node->paramSet[j].paramSize, node->paramSet[j].paramName, node->paramSet[j].paramSize);
					fprintf(fp, "    if (iRet == 0)\n");
					fprintf(fp, "    {\n");
					fprintf(fp, "        return -1;\n");
					fprintf(fp, "    }\n");
					fprintf(fp, "    pPrd->%s[iRet] = _T(\'\\0\');\n", node->paramSet[j].paramName);
				}
				else
				{
					fprintf(fp, "    iRet = ftd%s2prd%s(&pFtd->%s, &pPrd->%s);\n", node->paramSet[j].paramName, node->paramSet[j].paramName);
					fprintf(fp, "    if (iRet == 0)");
					fprintf(fp, "    {\n");
					fprintf(fp, "        return -1;\n");
					fprintf(fp, "    }\n");
				}
			}
			else
			{
				fprintf(fp, "    pPrd->%s = pFtd->%s;\n", node->paramSet[j].paramName, node->paramSet[j].paramName);
			}
		}
		fprintf(fp, "    return 0;\n");
		fprintf(fp, "}\n\n");
	}
}
