/*****************************************************************************************
	filename:	ft_generator.h
	created:	10/01/2012
	author:		chen
	purpose:	generate ft files

*****************************************************************************************/

#ifndef _H_FT_GENERATOR
#define _H_FT_GENERATOR

#include "ft_parser.h"
#include <cstdio>

extern void GenerateRecvInclude(const char* name, FILE* fp);
extern void GenerateRecvCpp(const char* name, FILE* fp);
extern void GenerateSendInclude(const char* name, FILE* fp);
extern void GenerateSendCpp(const char* name, FILE* fp);

#endif
