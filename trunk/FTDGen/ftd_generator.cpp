#include "ftd_generator.h"
#include <cstring>

void GenerateFtdDefine(const char* name, FILE* fp)
{
	fprintf(fp, "#ifndef _H_%s_DEFINE\n", name);
	fprintf(fp, "#define _H_%s_DEFINE\n\n", name);
	fprintf(fp, "#include \"server_common.h\"\n\n");
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
				fprintf(fp, "    %s %s[%s];\n", node->paramSet[j].paramType, node->paramSet[j].paramName, node->paramSet[j].paramSize);
			}
			else
			{
				fprintf(fp, "    %s %s;\n", node->paramSet[j].paramType, node->paramSet[j].paramName);
			}
		}
		fprintf(fp, "};\n\n");

		if (bContainString)
		{
			fprintf(fp, "struct prd%s\n", node->structName);
			fprintf(fp, "{\n");
			for (int j = 0; j < node->paramCount; ++j)
			{
				if (node->paramSet[j].paramSize[0] != '\0')
				{
					if (strcmp(node->paramSet[j].paramType, "char") == 0)
					{
						fprintf(fp, "    TCHAR %s[%s];\n", node->paramSet[j].paramName, node->paramSet[j].paramSize);
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
	}
	fprintf(fp, "\n#endif");
}