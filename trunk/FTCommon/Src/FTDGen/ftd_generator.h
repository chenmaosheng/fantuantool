/*****************************************************************************************
filename:	ftd_generator.h
created:	10/02/2012
author:		chen
purpose:	generate ftd files

*****************************************************************************************/

#ifndef _H_FTD_GENERATOR
#define _H_FTD_GENERATOR

#include "ftd_parser.h"
#include <cstdio>

extern void GenerateFtdDefine(const char* name, FILE* fp);
extern void GenerateFtdImpl(const char* name, FILE* fp);

#endif
